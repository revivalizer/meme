module Export

open ParserTypes
open ParserLogic
open Xunit;
open ErrorHandling;
open ErrorHandlingExtensions;

let GenerateHeader expr =
    let rec visitTree fn acc expr = 
        match expr with
        | List(_, l) -> List.fold (visitTree fn) acc l
        | Quote(_, e) -> visitTree fn acc e
        | Unquote(_, e) -> visitTree fn acc e
        | e -> fn acc e
    let makeUniqueStr (map : Map<string, int>) e =
        match e with
        | String(_, s) -> map.Add(s, map.Count)
        | _ -> map
    visitTree makeUniqueStr Map.empty expr

let parse_generate_header str =
    match parse expr str with
       | Success(ast) -> GenerateHeader ast
       | Failure(f)   -> failwith f

[<Fact>]
let ``Generate unique strings`` () =
    Assert.True((parse_generate_header "(\"test\")").Count=1)
    Assert.True((parse_generate_header "(\"test\" \"test\")").Count=1)