module EvalTest

open Xunit;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;
open ParserLogic;
open EvalLogic;

let parse_eval str =
    match parse expr str with
       | Success(ast) -> eval ast
       | Failure(f)   -> failwith f

let AssertEqualNum str expectedNum =
    match parse_eval str with
    | Number(n) -> Assert.Equal(n, expectedNum)
    | _         -> Assert.True false


[<Fact>]
let ``Simple expression eval`` () =
    AssertEqualNum "1" 1.0
    AssertEqualNum "(* 4 7)" 28.0
    AssertEqualNum "(  * 4 7)" 28.0
    AssertEqualNum "(  *   4     7    )" 28.0
    AssertEqualNum "(  *   -4     -7    )" 28.0
    AssertEqualNum "(* 2 3 4 5)" 120.0
