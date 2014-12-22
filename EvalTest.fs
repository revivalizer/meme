module EvalTest

open Xunit;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;
open ParserLogic;
open EvalLogic;

let AssertEvalEqual str res =
    match parse expr str with
       | Success(ast) -> Assert.True((eval ast) = res)
       | Failure(f)   -> failwith f