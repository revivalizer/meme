module ParserTypes

open FParsec;

[<StructuralEquality;StructuralComparison>]
type Expr = Number   of Position * float
          | String   of Position * string
          | Symbol   of Position * string
          | List     of Position * (Expr list)
          | Function of (Expr list -> Expr)
          | Special  of (Expr list -> Expr)

let dummyPosition = Position("", -1L, -1L, -1L)

let pos o =
    match o with
    | Number(p, _)   -> p
    | String(p, _)   -> p
    | Symbol(p, _)   -> p
    | List(p, _)     -> p
    | Function(_) -> dummyPosition
    | Special(_)  -> dummyPosition

let MakeNumber n =
    Number(dummyPosition, n)
