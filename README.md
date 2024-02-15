# TDTEA LAB 1

## Description

This project is a parser for a custom language, developed as part of the HT2024 Lab 1. It is implemented in C++.

### Grammar

The grammar of the language is as follows:

```
match ::= expression
expression ::= subexpression '+' subexpression | subexpression
Subexpression ::= element | element Modifier | element Modifier Subexpression | element Subexpression
element ::= character | any_char | group
Modifier ::= REPEAT | COUNT | IGNORE_CASE | OUTPUT_GROUP
REPEAT ::= element'*'
Count ::= element'{''number''}'
IGNORE_CASE ::= element'\I'
OUTPUT_GROUP ::= element'\O{''number''}'
character ::= a | b | c | ... | z | A | B | C | ... | Z | 0 | 1 | 2 | ... | 9
any_char ::= .
group ::= (expression)
```

## Getting Started

### Dependencies

* A modern C++ compiler (C++11 or later)
* CLion 2023.3.3 or any other IDE that supports C++.

### Installing

* Clone the repository from GitHub
* Open the project in your IDE

### Executing program

* Compile and run the `main.cpp` file

## Code Overview

The main components of the parser are:

* `Parse.cpp`: Contains the main parsing logic. It includes methods to parse expressions, subexpressions, elements,
  modifiers, and handle special characters.
* `main.cpp`: The entry point of the application.

## Help

If you encounter any issues, please open an issue on GitHub.

## Authors

[@nasiralizade](https://github.com/nasiralizade)

## Version History

* 0.1
    * Initial Release

## License