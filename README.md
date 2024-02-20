# TDTEA LAB 1

## Description

This project is a parser for a custom language, developed as part of the HT2024 Lab 1. It is implemented in C++.

### Grammar

The grammar of the language is as follows:

```
<REGEXP>      ::= <EXPR>
<EXPR>        ::= <MATCH> [<EXPR>]
<MATCH>       ::= <IGNORE> | <OR> |<GROUP> | <OUTPUTGROUP> | <WORD>
<OR>          ::= <WORD> + <WORD>
<IGNORE>      ::= <GROUP>\I | <REPEAT>\I | <WORD>\I
<REPEAT>      ::= <CHAR>* | <CHAR><COUNT>
<WORD>        ::= <REPEAT> | <CHAR><WORD> | <CHAR>
<CHAR>        ::= <ANY> | NOT [{,},(,),.,\I,\O ]
<ANY>         ::= .
<GROUP>       ::= (MATCH)
<COUNT>       ::= {<INTEGER>}
<OUTPUTGROUP> ::= \O{<INTEGER>｝
<INTEGER>     ::= 0 .. 9
```


