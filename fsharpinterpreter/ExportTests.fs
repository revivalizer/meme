module ExportTests

open ParserTypes
open ParserLogic
open Xunit;
open ErrorHandling;
open ErrorHandlingExtensions;
open Export

let AssertTrueWithUniques str fn =
    Assert.True(str |> parse expr |> bind analyzeUniques |> extractOrFail |> fn)

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
    AssertTrueWithUniques "(1.0)" (fun u -> u.uniqueNumbers.[1.0]=0us)
    let l1 = "(((1 2) 2 3) 3 1)"
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.Count=3)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 1.0)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 2.0)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 3.0)

let AssertBinaryRepEqual str rep =
    let r = (str |> parse expr |> bind analyzeUniques |> bind generateBinaryRepresentation |> extractOrFail).binaryExpr 
    Assert.True((r = rep))

[<Fact>]
let ``Binary representation`` () =
    let makeString s =
        [uint16(NodeType.String); uint16(s)]
    let makeSymbol s =
        [uint16(NodeType.Symbol); uint16(s)]
    let makeNumber n =
        [uint16(NodeType.Number); uint16(n)]
    let makeList l =
        [uint16(NodeType.List)] @ l @ [uint16(NodeType.EOC)] 

    AssertBinaryRepEqual "test" (makeSymbol 0)
    AssertBinaryRepEqual "(\"test\")" (makeList (makeString 0))
    AssertBinaryRepEqual "(test1 test2 test3)" (makeList (makeSymbol 0 @ makeSymbol 1 @ makeSymbol 2))
    AssertBinaryRepEqual "(test1 (test2 (test3)))" (makeList (makeSymbol 0 @ (makeList (makeSymbol 1 @ (makeList (makeSymbol 2))))))
