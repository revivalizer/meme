module Export

open ParserTypes
open ParserLogic
open ErrorHandling;
open ErrorHandlingExtensions;

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
        | Quote(_, e) -> visitTree fn acc e
        | Unquote(_, e) -> visitTree fn acc e
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

let parse_analyze_uniques str =
    match str |> parse expr |> bind analyzeUniques with
    | Success(uniques) -> uniques
    | Failure(f)   -> failwith f

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
    | Quote    = 5us
    | Unquote  = 6us

let generateBinaryExpression (res : AnalyzedExpr) =
    let rec generate expr =
        match expr with
        | String(_, s)   -> [uint16(NodeType.String); res.uniqueStrings.[s]]
        | Symbol(_, s)   -> [uint16(NodeType.Symbol); res.uniqueSymbols.[s]]
        | Number(_, n)   -> [uint16(NodeType.Number); res.uniqueNumbers.[n]]
        | Quote(_, e)    -> uint16(NodeType.Quote) :: (generate e)
        | Unquote(_, e)  -> uint16(NodeType.Unquote) :: (generate e)
        | List(_, l)     -> [uint16(NodeType.List)] @ (l |> List.map generate |> List.concat) @ [uint16(NodeType.EOC)]
    Success {
        binaryExpr = (generate res.expr)
        expr = res.expr
        uniqueStrings = res.uniqueStrings
        uniqueSymbols = res.uniqueSymbols
        uniqueNumbers = res.uniqueNumbers
    }