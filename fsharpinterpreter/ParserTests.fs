module ParserTests

open FParsec;
open Xunit;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;
open ParserLogic;

let AssertParseSuccess p str =
    Assert.True(success (parse (p .>> eof) str))

let AssertParseFailure p str =
    Assert.True(failure (parse (p .>> eof) str))

[<Fact>]
let ``Simple expression parser`` () =
    AssertParseSuccess numberLiteral "1"
    AssertParseSuccess numberLiteral "0x21"
    AssertParseSuccess numberLiteral "1 "
    AssertParseSuccess stringLiteral "\"1\" "
    AssertParseSuccess expr "(* 5 9)"
