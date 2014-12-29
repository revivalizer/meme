module EvalTest

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

let toAstExpr evalExpr = 
    let rec convert expr =
        match expr with
        | EvalExpr.Number(v) -> MakeNumber(v)
        | EvalExpr.String(v) -> MakeString(v)
        | EvalExpr.Symbol(v) -> MakeSymbol(v)
        | EvalExpr.List(v)   -> MakeList(v |> List.map convert)
        | o -> failwith "Illegal expression in conversion to AST" 
    convert evalExpr

let AssertEqualNum str expectedNum =
    match parse_eval str with
    | Number(n) -> Assert.Equal(n, expectedNum)
    | _         -> Assert.True false

let AssertEqual str expected = 
    let astExpr = parse_eval str |> toAstExpr
    Assert.Equal(astExpr, expected)

let MakeNumList nums =
    List.map MakeNumber nums |> MakeList

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

[<Fact>]
let ``Lists`` () =
    AssertEqual "(list 1 2 3)" (MakeNumList [1.0; 2.0; 3.0]) // list
    AssertEqual "(list 1 2 3)" (MakeNumList [1.0; 2.0; 3.0]) // list 
    AssertEqual "(car (list 1 2 3))" (MakeNumber 1.0) // car 
    AssertEqual "(cdr (list 1 2 3))" (MakeNumList [2.0; 3.0]) // cdr 
    AssertEqual "(cons 1 (list 2 3))" (MakeNumList [1.0; 2.0; 3.0])  // cons 
    AssertEqual "(cons 1 (cons 2 (cons 3 ())))" (MakeNumList [1.0; 2.0; 3.0]) // cons x3 
    AssertEqual "(let ((a 1) (b 2) (c 3)) (list a b c))"  (MakeNumList [1.0; 2.0; 3.0]) // list 
    AssertEqual "(let ((a (list 1 2 3))) (car a))" (MakeNumber 1.0) // car 
    AssertEqual "(let ((a (list 1 2 3))) (cdr a))" (MakeNumList [2.0; 3.0]) // cdr

[<Fact>]
let ``Quote`` () =
    let x23 = (MakeList [MakeSymbol "*"; MakeNumber 2.0; MakeNumber 3.0])
    AssertEqual "(quote (* 2 3))" x23 // quote primitive 
    AssertEqual "'(* 2 3)" x23 // quote primitive with sugar 
    AssertEqual "(eval '(* 2 3))" (MakeNumber 6.0) // eval quoted expression 
    AssertEqual "(quote (* 2 (- 5 2)))" (MakeList [MakeSymbol "*"; MakeNumber 2.0; MakeList [MakeSymbol "-"; MakeNumber 5.0; MakeNumber 2.0]])// quote nested 
    AssertEqual "(quote (* 2 (unquote (- 5 2))))" x23 // quote nested unquote 
    AssertEqual "'(* 2 ,(- 5 2))" x23 // quote nested unquote with sugar 
    AssertEqual "(quote (quote 1 2 3))" (MakeList [MakeSymbol "quote"; MakeNumber 1.0; MakeNumber 2.0; MakeNumber 3.0]) // quote special form 
    AssertEqual "(let ((x 'rain) (y 'spain) (z 'plain)) '(the ,x in ,y falls mainly on the ,z))" (["the"; "rain"; "in"; "spain"; "falls"; "mainly"; "on"; "the"; "plain";] |> List.map MakeSymbol |> MakeList) // quote/unquote 
    AssertEqual "(let ((* -)) (eval '(* 3 3)))" (MakeNumber 9.0) // eval uses top-level environment 