module ParserTypes

open FParsec;

type Expr = Number   of Position * float
          | String   of Position * string
          | Symbol   of Position * string
          | List     of Position * (Expr list)
          | Function of Position * (Expr list -> Expr)
          | Special  of Position * (Expr list -> Expr)