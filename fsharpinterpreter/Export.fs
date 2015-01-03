module Export

open ParserTypes
open ParserLogic
open Xunit;
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

let AssertTrueWithUniques str fn =
    Assert.True(str |> parse_analyze_uniques |> fn)

[<Fact>]
let ``Generate unique strings`` () =
    AssertTrueWithUniques "(\"test\")" (fun u -> u.uniqueStrings.Count=1)
    AssertTrueWithUniques "(\"test\" \"test\")" (fun u -> u.uniqueStrings.Count=1)
    AssertTrueWithUniques "(\"test\" \"test\" \"test\" \"test\" \"test\" \"test\")" (fun u -> u.uniqueStrings.Count=1)
    AssertTrueWithUniques "(\"test\" (\"test\" (\"test\" \"test\") \"test\") \"test\")" (fun u -> u.uniqueStrings.Count=1)
    AssertTrueWithUniques "(\"test1\" (\"test2\" (\"test3\" \"test1\") \"test2\") \"test3\")" (fun u -> u.uniqueStrings.Count=3)
    let l1 = "(\"test1\" (\"test2\" (\"test3\" \"test3\") \"test2\") \"test1\")" 
    AssertTrueWithUniques l1 (fun u -> u.uniqueStrings.Count=3)
    AssertTrueWithUniques l1 (fun u -> u.uniqueStrings.ContainsKey "test1")
    AssertTrueWithUniques l1 (fun u -> u.uniqueStrings.ContainsKey "test2")
    AssertTrueWithUniques l1 (fun u -> u.uniqueStrings.ContainsKey "test3")

[<Fact>]
let ``Generate unique symbols`` () =
    AssertTrueWithUniques "(test)" (fun u -> u.uniqueSymbols.Count=1)
    let l1 = "(((test1 test2) test2 test3) test3 test1)"
    AssertTrueWithUniques l1 (fun u -> u.uniqueSymbols.Count=3)
    AssertTrueWithUniques l1 (fun u -> u.uniqueSymbols.ContainsKey "test1")
    AssertTrueWithUniques l1 (fun u -> u.uniqueSymbols.ContainsKey "test2")
    AssertTrueWithUniques l1 (fun u -> u.uniqueSymbols.ContainsKey "test3")

[<Fact>]
let ``Generate unique numbers`` () =
    AssertTrueWithUniques "(1.0)" (fun u -> u.uniqueNumbers.Count=1)
    AssertTrueWithUniques "(1.0)" (fun u -> u.uniqueNumbers.[1.0]=0)
    let l1 = "(((1 2) 2 3) 3 1)"
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.Count=3)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 1.0)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 2.0)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 3.0)