module EvalLogic

open ParserTypes
open ErrorHandling;
open ErrorHandlingExtensions;

let Multiply args =
    let product acc = function
        Number(_, b) -> acc * b
        | o -> failwith (sprintf "Malformed multiplication argument at %A" (pos o))
    Number (pos (List.head args), List.fold product 1.0 args)

let environment = 
    Map.ofList [ 
        "*", Function(Multiply) 
        ]

let rec eval expression =
    match expression with
    | Number(_) as lit -> lit
    | String(_) as lit -> lit
    | Symbol(p, s) -> environment.[s]
    | List(p, []) -> List(p, [])
    | List(p, h :: t) ->
        match eval h with
        | Function(f) -> apply f t
        | Special(s) -> s t
        | o -> failwith (sprintf "Malformed expression at %A" (pos o))
    | _ -> failwith "Malformed expression."
and apply fn args = fn (List.map eval args)