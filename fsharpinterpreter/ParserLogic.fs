﻿module ParserLogic

open FParsec;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;

let lineComment = skipChar ';' >>. skipRestOfLine true 

let ws = many (lineComment <|> spaces1)
let str_ws s = pstring s .>> ws

let floatLiteral = getPosition .>>. pfloat .>> ws |>> Number
let intLiteral = getPosition .>>. pint32 .>> ws |>> (fun (p, f) -> Number(p, float(f)))

let numberLiteral = floatLiteral <|> intLiteral

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
let list       = getPosition .>> str_ws "(" .>>. (many expr) .>> str_ws ")" |>> List
let infix_list = getPosition .>> str_ws "{" .>>. (many expr) .>> str_ws "}" |>> InfixList
let expr' = atom <|> list <|> infix_list
let quoted_expr = getPosition .>> skipChar '\'' .>>. expr' |>> (fun (p, e) -> List(p, [Symbol(p, "quote"); e]))
let unquoted_expr = getPosition .>> skipChar ',' .>>. expr' |>> (fun (p, e) -> List(p, [Symbol(p, "unquote"); e]))
do exprImpl := ws >>. (quoted_expr <|> unquoted_expr <|> expr')

let parse p str =
    match run p str with
    | ParserResult.Success(result, _, _)   -> Success(result)
    | ParserResult.Failure(errorMsg, _, _) -> Failure(errorMsg)

let rec stringify expr =
    match expr with
    | List(p, l)      -> "(" + (l |> List.map stringify |> Seq.ofList |> String.concat " ") + ")"
    | InfixList(p, l) -> "{" + (l |> List.map stringify |> Seq.ofList |> String.concat " ") + "}"
    | Number(_, n)    -> sprintf "%A" n
    | String(_, s)    -> sprintf "%s" s
    | Symbol(_, s)    -> sprintf "%s" s

let rec infixExpand (list : Expr list) = // recurse over list
    match list with
    | [e1; s; e2]           ->  infixRearrange e1 s e2
    | e1 :: s :: e2 :: rest -> (infixRearrange e1 s e2) :: rest |> infixExpand
    | o -> failwith (sprintf "Malformed infix at %A." (pos (List.head o)))
and infixRearrange e1 s e2 = // convert infix to prefix
    match e1 with
        | List(p, h :: t) ->
            match s,h with
            | Symbol(_, s1), Symbol(_, s2) when s1=s2 -> List(p, [h] @ t @ [e2])    // symbols match, concat e2 into e1 list
            | _                                       -> List(pos e1, [s; e1; e2])  // symbols don't match, make new list
        | e -> List(pos e1, [s; e1; e2]) 

let infixExpandWalk expr = 
    let rec walk expr =
        match expr with
        | List(p, l)      -> List(p, l |> List.map walk)
        | InfixList(p, l) -> infixExpand l |> walk
        | e -> e
    Success (walk expr)

// This line is necceasry to compile without value restriction errors!
let expr_parse = parse expr
