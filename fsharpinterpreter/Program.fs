// Learn more about F# at http://fsharp.net
// See the 'F# Tutorial' project for more help.

open FParsec;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;
open ParserLogic;

let test f str =
    match f str with
    | Success(result)   -> printfn "Success: %A" result
    | Failure(errorMsg) -> printfn "Failure: %s" errorMsg

[<EntryPoint>]
let main argv = 
    test (parse numberLiteral) "sin().axe().cos(3)"
    printfn "%A" argv
    0 // return an integer exit code
