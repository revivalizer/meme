module ExportTests

open ParserTypes
open ParserLogic
open Xunit;
open ErrorHandling;
open ErrorHandlingExtensions;
open Export

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
    AssertTrueWithUniques "(1.0)" (fun u -> u.uniqueNumbers.[1.0]=0us)
    let l1 = "(((1 2) 2 3) 3 1)"
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.Count=3)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 1.0)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 2.0)
    AssertTrueWithUniques l1 (fun u -> u.uniqueNumbers.ContainsKey 3.0)