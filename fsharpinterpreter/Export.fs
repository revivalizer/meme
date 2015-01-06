module Export

open ParserTypes
open ParserLogic
open ErrorHandling;
open ErrorHandlingExtensions;
open System.IO;

type AnalyzedExpr = {
    expr                   : Expr
    uniqueStrings          : Map<string,uint16>
    uniqueSymbols          : Map<string,uint16>
    uniqueNumbers          : Map<float,uint16>
}

let addUniqueToMap (map : Map<'a, uint16>) v =
    match map.ContainsKey v with
    | true -> map
    | false -> map.Add(v, uint16(map.Count))

let analyzeUniques expr =
    let rec visitTree fn acc expr = 
        match expr with
        | List(_, l) -> List.fold (visitTree fn) acc l
        | e -> fn acc e
    let makeUniqueStrings (map : Map<string, uint16>) e =
        match e with
        | String(_, s) -> addUniqueToMap map s
        | _ -> map
    let makeUniqueSymbols (map : Map<string, uint16>) e =
        match e with
        | Symbol(_, s) -> addUniqueToMap map s
        | _ -> map
    let makeUniqueNumbers (map : Map<float, uint16>) e =
        match e with
        | Number(_, n) -> addUniqueToMap map n
        | _ -> map
    Success {
        expr = expr;
        uniqueStrings = visitTree makeUniqueStrings Map.empty expr;
        uniqueSymbols = visitTree makeUniqueSymbols Map.empty expr;
        uniqueNumbers = visitTree makeUniqueNumbers Map.empty expr;
    }

type BinaryExpr = {
    binaryExpr             : uint16 list
    expr                   : Expr
    uniqueStrings          : Map<string,uint16>
    uniqueSymbols          : Map<string,uint16>
    uniqueNumbers          : Map<float,uint16>
}

type NodeType =
    | EOC      = 0us // end of children marker for node
    | List     = 1us
    | String   = 2us
    | Symbol   = 3us
    | Number   = 4us

let generateBinaryRepresentation (res : AnalyzedExpr) =
    let rec generate expr =
        match expr with
        | String(_, s)   -> [uint16(NodeType.String); res.uniqueStrings.[s]]
        | Symbol(_, s)   -> [uint16(NodeType.Symbol); res.uniqueSymbols.[s]]
        | Number(_, n)   -> [uint16(NodeType.Number); res.uniqueNumbers.[n]]
        | List(_, l)     -> [uint16(NodeType.List)] @ (l |> List.map generate |> List.concat) @ [uint16(NodeType.EOC)]
    Success {
        binaryExpr = (generate res.expr)
        expr = res.expr
        uniqueStrings = res.uniqueStrings
        uniqueSymbols = res.uniqueSymbols
        uniqueNumbers = res.uniqueNumbers
    }

let generateBinaryData (res : BinaryExpr) =
    (* Header format:
        uint32   : size of struct in bytes
        uint16   : num elements in tree rep
        uint16   : num strings
        uint16   : num symbols
        uint16   : num numbers
        uint16*  : pointer to binary tree representation
        char*    : pointer to zero-terminated string list
        char*    : pointer to zero-terminated symbols list
        uint32   : dummy data, for 64 bit alignment
        double*  : pointer to number array
        double array: numbers
        uint16 array: tree rep
        char array: string list
        char array: symbol list
    *)
    let writeHeader (writer : BinaryWriter) res =
        let computeAccumulatedKeyLength map =
            map |> Map.fold (fun state (key : string) value -> state + key.Length + 1) 0 // assume one extra byte for zero termination for every string
        let headerSize = 32
        let treeSize   = res.binaryExpr.Length * 2
        let stringSize = res.uniqueStrings |> computeAccumulatedKeyLength
        let symbolSize = res.uniqueSymbols |> computeAccumulatedKeyLength
        let numberSize = res.uniqueNumbers.Count * 8
        let numberPos = headerSize
        let treePos = numberPos + numberSize
        let stringPos = treePos + treeSize
        let symbolPos = stringPos + stringSize
        let totalSize = headerSize + numberSize + treeSize + stringSize + symbolSize
        writer.Write(uint32(totalSize))
        writer.Write(uint16(res.binaryExpr.Length))
        writer.Write(uint16(res.uniqueStrings.Count))
        writer.Write(uint16(res.uniqueSymbols.Count))
        writer.Write(uint16(res.uniqueNumbers.Count))
        writer.Write(uint32(treePos))
        writer.Write(uint32(stringPos))
        writer.Write(uint32(symbolPos))
        writer.Write(uint32(numberPos))
        writer.Write(uint32(0)) // dummy
    let writeList (writer : BinaryWriter) (list : List<'a>) bytesPerElement =
        let dataSize = list.Length * bytesPerElement
        let data:byte[] = Array.zeroCreate dataSize
        System.Buffer.BlockCopy(list |> List.toArray, 0, data, 0, data.Length)
        writer.Write(data)
    let mapToOrderedList (map : Map<'a, uint16>) =
        Map.toList map |> List.sortBy (fun (key, v) -> v)
    let appendZeroTerminator list =
        list |> List.map (fun str -> str + "\0")
    let toCharList strList =
        strList |> List.fold (fun state s -> state @ [for c in s -> c]) [] 
    let stream = new MemoryStream()
    let writer = new BinaryWriter(stream, System.Text.Encoding.ASCII)
    writeHeader writer res
    writeList writer (res.uniqueNumbers |> mapToOrderedList |> List.map fst) 8
    writeList writer res.binaryExpr 2
    writeList writer (res.uniqueStrings |> mapToOrderedList |> List.map fst |> appendZeroTerminator |> toCharList) 1
    writeList writer (res.uniqueSymbols |> mapToOrderedList |> List.map fst |> appendZeroTerminator |> toCharList) 1
    Success stream.GetBuffer

let strToBinaryRep str =
    str |> parse expr |> bind analyzeUniques |> bind generateBinaryRepresentation |> bind generateBinaryData |> extractOrFail