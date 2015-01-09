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

    let malformed str p =
        failwith (sprintf "Malformed %s at %A." str p)

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
            | Expr.Number(pos, v)  -> AddToPosMap (EvalExpr.Number(v)) pos
            | Expr.String(pos, v)  -> AddToPosMap (EvalExpr.String(v)) pos
            | Expr.Symbol(pos, v)  -> AddToPosMap (EvalExpr.Symbol(v)) pos
            | Expr.List(pos, v)    -> AddToPosMap (EvalExpr.List(v |> List.map convert)) pos
        convert parseexpr
    
    let NumericBinaryOp f args =
        let binop a b =
            match a,b with
            | Number(a),Number(b) -> Number(f a b)
            | Number(a),b         -> malformed "numeric argument" (pos b)
            | a,b                 -> malformed "numeric argument" (pos a)
        List.reduce binop args

    let extend env bindings = (ref (Map.ofList bindings) :: env)

    let lookup p env symbol =
        match List.tryPick (fun (frame : Frame) -> Map.tryFind symbol frame.Value) env with
        | Some(e) -> e
        | None    -> failwith (sprintf "No binding for '%s' at %A." symbol p)

    let rec eval (env : Environment) (expression : EvalExpr) =
        match expression with
        | Number(_) as lit -> lit
        | String(_) as lit -> lit
        | Symbol(s) as symbol -> (lookup (pos symbol) env s).Value
        | List([]) -> List([])
        | List(h :: t) ->
            match eval env h with
            | Function(f) -> apply f env t
            | Special(s) -> s env t
            | o -> malformed "expression" (pos o)
        | _ -> failwith "Malformed expression."
    and apply fn env args = fn (List.map (eval env) args)
    and if' env args = 
        match args with
        | [condition;t;f] -> match eval env condition with
                             | Number(0.0) -> eval env f
                             | String("")  -> eval env f
                             | List([])    -> eval env f
                             | _           -> eval env t
        | _ -> malformed "if arguments" (pos (List.head args))
    and let' env args =
        match args with
        | [List(bindings);body] -> 
            let bind args =
                match args with
                | List([Symbol(s);e]) -> s,ref (eval env e)
                | o -> malformed "let expression" (pos o)
            let env' = List.map bind bindings |> extend env 
            eval env' body
        | o -> malformed "let expression" (pos (List.head o))
    and letrec env args =
        match args with
        | [List(bindings);body] -> 
            let dummy = Function(fun _ -> failwith (sprintf "Cannot evaluate dummy values at %A" (pos (List.head args))))
            let bind args =
                match args with
                | List([Symbol(s);e]) -> s,ref dummy
                | o -> malformed "letrec expression" (pos o)
            let env' = List.map bind bindings |> extend env 
            let update = function List([Symbol(s); e]) -> (env'.Head.Value.Item s) := (eval env' e) 
                                  | o -> malformed "letrec binding" (pos o)
            List.iter update bindings 
            eval env' body
        | o -> malformed "let expression" (pos (List.head o))
    and lambda env args =
        match args with
        | [List(parameters);body] ->
            let closure env' args =
                let bindings = List.zip parameters args
                let bind arg =
                    match arg with
                    | Symbol(p),e -> p,ref (eval env' e)
                    | o -> malformed "lambda call" (pos (fst o))
                let env'' = List.map bind bindings |> extend env
                eval env'' body
            Special(closure)
        | o -> malformed "lambda expression" (pos (List.head args))
    and letstar env = function
        | [List(bindings); body] ->
            let bind env binding = 
                match binding with 
                | List([Symbol(s); e]) -> [s, ref (eval env e)] |> extend env
                | o -> malformed "let* binding" (pos o)
            let env' = List.fold bind env bindings 
            eval env' body 
        | o -> malformed "let*" (pos (List.head o))
    and cons = function [h; List(t)] -> List(h :: t) | o -> malformed "cons" (pos (List.head o))
    and car = function [List(h :: _)] -> h | o -> malformed "car" (pos (List.head o))
    and cdr = function [List(_ :: t)] -> List(t) | o -> malformed "cdr" (pos (List.head o))
    and lst args = List(args) 
    and quote env =
        let rec unquote expr =
            match expr with
            | List([Symbol("unquote"); e]) -> eval env e 
            | List(Symbol("unquote") :: _) -> malformed "unquote" (pos expr) // too many args 
            | List(h :: t) -> List(h :: (List.map unquote t)) // recurse 
            | e -> e
        function [e] -> unquote e | o -> malformed "quote" (pos (List.head o))
    and eval' env = function [args] -> args |> eval env |> eval globalenvironment | o -> malformed "eval" (pos (List.head o))
    and macro env = function 
        | [List(parameters); body] -> 
            let closure env' args = 
                // bind parameters to actual arguments (but unevaluated, unlike lambda) 
                let bindings = List.zip parameters args 
                let bind = function Symbol(p), a -> p, ref a | o -> malformed "macro" (pos (fst o))
                let env'' = List.map bind bindings |> extend env // extend the captured definition-time environment 
                eval env'' body |> eval env' // eval against bound args, then again in the caller's environment 
            Special(closure) 
        | o -> malformed "macro" (pos (List.head o))
    and set env args = 
        match args with
        | [Symbol(s); e] -> 
            (lookup (pos (List.head args)) env s) := eval env e 
            Dummy(sprintf "Set %s" s) 
        | o -> malformed "set!" (pos (List.head o))
    and begin' env = List.fold (fun _ e -> eval env e) (Dummy("Empty 'begin'"))
    and define' (env : Environment) = function 
        | [Symbol(s); e] -> 
            let def = ref (Dummy("Dummy 'define'")) 
            env.Head := Map.add s def env.Head.Value 
            def := eval env e 
            Dummy(sprintf "Defined %s" s) 
        | o -> malformed "define" (pos (List.head o))
    and globalenvironment = 
        [ ref (Map.ofList 
            [ 
                "*", ref (Function(NumericBinaryOp (*)));
                "-", ref (Function(NumericBinaryOp (-)));
                "if", ref (Special(if'));
                "let", ref (Special(let'));
                "letrec", ref (Special(letrec));
                "let*", ref (Special(letstar));
                "lambda", ref (Special(lambda));
                "cons", ref (Function(cons)) 
                "car", ref (Function(car)) 
                "cdr", ref (Function(cdr)) 
                "list", ref (Function(lst))
                "quote", ref (Special(quote))
                "eval", ref (Special(eval'))
                "macro", ref (Special(macro))
                "set!", ref (Special(set))
                "begin", ref (Special(begin'))
                "define", ref (Special(define'))
            ]) ]


    let evalexpr = GenEvalExpr expr
    eval globalenvironment evalexpr 