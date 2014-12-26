module EvalTypes

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
              | Special  of (Position -> Map<string, EvalExpr> list -> EvalExpr list -> EvalExpr)

type ExprPosMap() =
    inherit Dictionary<EvalExpr, Position>(HashIdentity.Reference)
    member this.Add' name section =
        base.Add(name, section)
        this

