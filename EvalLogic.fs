module EvalLogic

open ParserTypes
open FParsec
open EvalTypes
open ErrorHandling;
open ErrorHandlingExtensions;

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

    let extend env bindings = (Map.ofList bindings) :: env 

    let lookup pos env symbol =
        match List.tryPick (Map.tryFind symbol) env with 
        | Some(e) -> e
        | None    -> failwith (sprintf "No binding for '%s' at %A." symbol pos)

    let rec eval (env : Map<string, EvalExpr> list) (expression : EvalExpr) =
        match expression with
        | Number(_) as lit -> lit
        | String(_) as lit -> lit
        | Symbol(s) as symbol -> lookup (pos symbol) env s
        | List([]) -> List([])
        | List(h :: t) ->
            match eval env h with
            | Function(f) -> apply f env t
            | Special(s) -> s (pos h) env t
            | o -> failwith (sprintf "Malformed expression at %A" (pos o))
        | _ -> failwith "Malformed expression."
    and apply fn env args = fn (List.map (eval env) args)
    and if' pos env args = 
        match args with
        | [condition;t;f] -> match eval env condition with
                             | Number(0.0) -> eval env f
                             | String("")  -> eval env f
                             | List([])    -> eval env f
                             | _           -> eval env t
        | _ -> failwith (sprintf "Malformed if expression at %A" pos)
    and let' pos2 env args =
        match args with
        | [List(bindings);body] -> 
            let bind args =
                match args with
                | List([Symbol(s);e]) -> s,(eval env e)
                | o -> failwith (sprintf "Malformed let expression at %A" (pos o))
            let env' = List.map bind bindings |> extend env 
            eval env' body
        | o -> failwith (sprintf "Malformed let expression at %A" (pos (List.head o)))
    and lambda pos2 env args =
        match args with
        | [List(parameters);body] ->
            let closure (p : Position) env' args =
                let bindings = List.zip parameters args
                let bind arg =
                    match arg with
                    | Symbol(p),e -> p,(eval env' e)
                    | o -> failwith (sprintf "Malformed lambda call at %A" (pos (fst o)))
                let env'' = List.map bind bindings |> extend env
                eval env'' body
            Special(closure)
        | o -> failwith (sprintf "Malformed lambda expression at %A" pos2)
        

    and globalenvironment = 
        [ Map.ofList [ 
            "*", Function(NumericBinaryOp (*));
            "-", Function(NumericBinaryOp (-));
            "if", Special(if');
            "let", Special(let');
            "let", Special(let');
            "lambda", Special(lambda);
            ] ]


    let evalexpr = GenEvalExpr expr
    eval globalenvironment evalexpr 