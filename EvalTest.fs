﻿module EvalTest

open Xunit;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;
open ParserLogic;
open EvalTypes;
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
    AssertEqualNum "(- 10 5 5)" 0.0
    AssertEqualNum "(if 0 1 2)" 2.0
    AssertEqualNum "(if 1 1 2)" 1.0
    AssertEqualNum "(if \"a\" 1 2)" 1.0
    AssertEqualNum "(if \"\" 1 2)" 2.0

[<Fact>]
let ``Let`` () =
    AssertEqualNum "(let ((x 2)) x)" 2.0
    AssertEqualNum "(let ((x (* 2 3))) (- 8 x))" 2.0

[<Fact>]
let ``Lambda`` () =
    AssertEqualNum "(let ((square (lambda (x) (* x x)))) (square 4))" 16.0
    AssertEqualNum "(let ((times3 (let ((n 3)) (lambda (x) (* n x))))) (times3 4))" 12.0
    AssertEqualNum "(let ((times3 (let ((makemultiplier (lambda (n) (lambda (x) (* n x))))) (makemultiplier 3)))) (times3 5))" 15.0

[<Fact>]
let ``Recursive Let`` () =
    AssertEqualNum "(letrec ((factorial (lambda (n) (if n (* n (factorial (- n 1))) 1)))) (factorial 4))" 24.0

[<Fact>]
let ``Sequential Let`` () =
    AssertEqualNum "(let ((a 1) (b 2)) (let ((a b) (b a)) b))" 1.0               // let binds in parallel (should work in earlier versions too)
    AssertEqualNum "(let ((a 1) (b 2)) (let* ((a b) (b a)) b))" 2.0              // let* binds sequentially
    AssertEqualNum "(let ((a 5)) (let ((b (* a 2))) (let ((c (- b 3))) c)))" 7.0 // poor-man's sequential expressions
    AssertEqualNum "(let* ((a 5) (b (* a 2)) (c (- b 3))) c)" 7.0                // let* sequential expressions