// Learn more about F# at http://fsharp.net
// See the 'F# Tutorial' project for more help.

open FParsec;
open ErrorHandling;
open ErrorHandlingExtensions;
open ParserTypes;
open ParserLogic;
open System;
open System.IO
open System.IO.Pipes
open System.Text

let test f str =
    match f str with
    | Success(result)   -> printfn "Success: %A" result
    | Failure(errorMsg) -> printfn "Failure: %s" errorMsg

let readMessage (pipeStream : NamedPipeServerStream) =
    let rec readBytes revByteList = 
        let revByteList' = byte(pipeStream.ReadByte()) :: revByteList
        if pipeStream.IsMessageComplete then revByteList'
        else readBytes revByteList'
    readBytes [] |> List.rev |> List.toArray |> System.Text.Encoding.ASCII.GetString

[<EntryPoint>]
let main argv = 
    printfn "[F#] NamedPipeServerStream thread created."
    //let pipeServer = new NamedPipeServerStream("memeparser", PipeDirection.InOut, 4)
    let pipeServer = new NamedPipeServerStream("memeparser", PipeDirection.InOut, 4, PipeTransmissionMode.Message)
    let rec loop() =
        //wait for connection
        printfn "[F#] Wait for a client to connect"
        pipeServer.WaitForConnection()

        printfn "[F#] Client connected."
        try
            // Stream for the request. 
            //use sr = new StreamReader(pipeServer)
            // Stream for the response. 
            use sw = new StreamWriter(pipeServer, AutoFlush = true)

            // Read request from the stream. 
            //let echo = sr.ReadLine();
            let echo = readMessage pipeServer

            printfn "[F#] Request message: %s" echo

            // Write response to the stream.
            echo |> sprintf "[F#]: %s" |> sw.WriteLine

            pipeServer.Disconnect()
            //if true then loop()
        with
        | _ as e -> printfn "[F#]ERROR: %s" e.Message
    loop()
    printfn "[F#] Client Closing."
    Console.Read() |> ignore
    pipeServer.Close()
    0
