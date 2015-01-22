meme
====

A for shits and giggles Scheme interpreter in F#

Basic research for future 64kb intros.

status
====
- [x] F# S-expression parser
- [x] F# interpreter. Has if, let, letrec, let*, lambda, cons, car, cdr, list, eval, quote, unquote, macro, set!, begin, define. String, symbol, number primitives. But very little else. No continuation passing.
- [x] F# export of binary representation of parsed S-expression
- [x] Interop between C++ and F# parser/exporter
- [ ] Basic C++ interpreter [IN PROGRESS]
  - [ ] Garbage collector
  - [ ] Size optimization
  - [ ] Continuation passing style?
- [ ] Scheme to x86 compiler in Scheme
- [ ] IDE

compiler links I want to check out
====

"An Incremental Approach to Compiler Construction" outlines how to compile scheme to x86 asm. Ikarus Scheme is a implementation of these ideas. Official sites have decayed.

Heres an [Archive.org link] (http://web.archive.org/web/20100828212411/http://www.cs.indiana.edu/~aghuloum/) to Ghuloums original page.

[Ikarus Scheme] (https://www.google.dk/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=ikarus%20scheme)

[Most recent Ikarus source] (http://bazaar.launchpad.net/~aghuloum/ikarus/ikarus.dev/files) From this we could probably glean how the x86 machine code is emitted.

[Extended Tutorial] (http://web.archive.org/web/20100828212411/http://www.cs.indiana.edu/~aghuloum/compilers-tutorial-2006-09-16.pdf)

[A DAIMI-Scheme VM and JIT compiler in OCaml] (http://www.zerny.dk/dsvm.html)

But it looks like a CPS transforming compiler would make more sense, ala Guy Steele: RABBIT: A Compiler for SCHEME. And "Compiling with Continuations" by Appel.
