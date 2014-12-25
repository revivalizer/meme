module EvalLogic

open ParserTypes
open FParsec
open ErrorHandling;
open ErrorHandlingExtensions;

open System.Collections.Generic;

type EvalExpr = Number   of float
              | String   of string
              | Symbol   of string
              | List     of (EvalExpr list)
              | Function of (EvalExpr list -> EvalExpr)
              | Special  of (EvalExpr list -> EvalExpr)

type ExprPosMap() =
    inherit Dictionary<EvalExpr, Position>(HashIdentity.Reference)
    member this.Add' name section =
        base.Add(name, section)
        this

let eval expr = 
    let posMap = new ExprPosMap()

    let pos o =
       posMap.[o] 

    // Convert parse types to eval types
    (* You could debate wether it's neccesary to split into parse and eval types.
       I didn't like that Special and Function had to be part of parser types, because
       they don't come out of the parsing stage. *)
    let GenEvalExpr parseexpr = 
        let AddToPosMap expr pos =
            posMap.Add(expr, pos)
            expr
        let rec convert parseexpr =
            match parseexpr with
            | Expr.Number(pos, v) -> AddToPosMap (EvalExpr.Number(v)) pos
            | Expr.String(pos, v) -> AddToPosMap (EvalExpr.String(v)) pos
            | Expr.Symbol(pos, v) -> AddToPosMap (EvalExpr.Symbol(v)) pos
            | Expr.List(pos, v)   -> AddToPosMap (EvalExpr.List(v |> List.map convert)) pos
        convert parseexpr
    
    let NumericBinaryOp f args =
        let binop a b =
            match a,b with
            | Number(a),Number(b) -> Number(f a b)
            | Number(a),b         -> failwith (sprintf "Malformed multiplication argument at %A" (pos b))
            | a,b                 -> failwith (sprintf "Malformed multiplication argument at %A" (pos a))
        List.reduce binop args

    let environment = 
        Map.ofList [ 
            "*", Function(NumericBinaryOp (*));
            "-", Function(NumericBinaryOp (-));
            ]

    let rec eval (expression : EvalExpr) =
        match expression with
        | Number(_) as lit -> lit
        | String(_) as lit -> lit
        | Symbol(s) -> environment.[s]
        | List([]) -> List([])
        | List(h :: t) ->
            match eval h with
            | Function(f) -> apply f t
            | Special(s) -> s t
            | o -> failwith (sprintf "Malformed expression at %A" (pos o))
        | _ -> failwith "Malformed expression."
    and apply fn args = fn (List.map eval args)

    let evalexpr = GenEvalExpr expr
    eval evalexpr