﻿module ParserLogic

open FParsec;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;

let ws = spaces
let str_ws s = pstring s .>> ws

let numberLiteral = getPosition .>>. pfloat .>> ws |>> Number

let isSymbolChar = isAnyOf "!$%&*+-./:<=>?@^_~"

let ident =
    let isAsciiIdStart c =
        isAsciiLetter c || isSymbolChar c

    let isAsciiIdContinue c =
        isAsciiLetter c || isSymbolChar c || isDigit c

    identifier (IdentifierOptions(isAsciiIdStart    = isAsciiIdStart,
                                  isAsciiIdContinue = isAsciiIdContinue))

let symbol = getPosition .>>. ident .>> ws |>> Symbol

let expr, exprImpl = createParserForwardedToRef()

let stringLiteral =
    let normalChar = satisfy (fun c -> c <> '\\' && c <> '"')
    let unescape c = match c with
                     | 'n' -> '\n'
                     | 'r' -> '\r'
                     | 't' -> '\t'
                     | c   -> c
    let escapedChar = pstring "\\" >>. (anyOf "\\nrt\"" |>> unescape)
    getPosition .>>. between (pstring "\"") (pstring "\"" .>> ws)
            (manyChars (normalChar <|> escapedChar)) |>> String

let atom = numberLiteral <|> symbol <|> stringLiteral
let list = getPosition .>> str_ws "(" .>>. (many expr) .>> str_ws ")" |>> List
let expr' = atom <|> list
let quoted_expr = getPosition .>> skipChar '\'' .>>. expr' |>> Quote
let unquoted_expr = getPosition .>> skipChar ',' .>>. expr' |>> Unquote
do exprImpl := quoted_expr <|> unquoted_expr <|> expr'

let parse p str =
    match run p str with
    | ParserResult.Success(result, _, _)   -> Success(result)
    | ParserResult.Failure(errorMsg, _, _) -> Failure(errorMsg)

// This line is necceasry to compile without value restriction errors!
let expr_parse = parse expr