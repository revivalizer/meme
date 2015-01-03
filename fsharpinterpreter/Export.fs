module Export

open ParserTypes
open ParserLogic
open ErrorHandling;
open ErrorHandlingExtensions;

type AnalyzedExpr = {
    expr                   : Expr
    uniqueStrings          : Map<string,int>
    uniqueSymbols          : Map<string,int>
    uniqueNumbers          : Map<float,int>
}

let addUniqueToMap (map : Map<'a, int>) v =
    match map.ContainsKey v with
    | true -> map
    | false -> map.Add(v, map.Count)

let analyzeUniques expr =
    let rec visitTree fn acc expr = 
        match expr with
        | List(_, l) -> List.fold (visitTree fn) acc l
        | Quote(_, e) -> visitTree fn acc e
        | Unquote(_, e) -> visitTree fn acc e
        | e -> fn acc e
    let makeUniqueStrings (map : Map<string, int>) e =
        match e with
        | String(_, s) -> addUniqueToMap map s
        | _ -> map
    let makeUniqueSymbols (map : Map<string, int>) e =
        match e with
        | Symbol(_, s) -> addUniqueToMap map s
        | _ -> map
    let makeUniqueNumbers (map : Map<float, int>) e =
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
