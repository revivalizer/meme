﻿module ParserTypes

open FParsec;

type Expr = Number        of Position * float
          | String        of Position * string
          | Symbol        of Position * string
          | List          of Position * (Expr list)
          | InfixList     of Position * (Expr list)

let dummyPosition = Position("", -1L, -1L, -1L)

let pos o =
    match o with
    | Number(p, _)        -> p
    | String(p, _)        -> p
    | Symbol(p, _)        -> p
    | List(p, _)          -> p
    | InfixList(p, _)     -> p

let MakeNumber n =
    Number(dummyPosition, n)

let MakeString str =
    String(dummyPosition, str)

let MakeSymbol s =
    String(dummyPosition, s)

let MakeList l =
    List(dummyPosition, l)
