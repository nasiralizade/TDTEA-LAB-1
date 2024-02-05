//
// Created by Nasir Alizade on 2024-02-04.
//

#ifndef LABB1_V4_TEST_H
#define LABB1_V4_TEST_H
<RE> :: =
<expr>
<expr> :: =
<basic-RE> | <basic-RE> "|" <expr>  // Use '|' for OR operation between chains
<basic-RE> ::= <element> | <element> <basic-RE>  // A chain is a sequence of units
<element> :: =
<basic-element> | <operator> < basic - element>  // A unit is a basic element or an operator applied to a basic element
<basic-element> ::= <group> | <word>  // Basic elements include groups or words
<group> :: = "(" < expr > ")"  // Grouping allows nested expressions
             < operator>:: =
<repeat> | <or> | <count> | <case-insensitivity> | <capture>
<repeat> :: = "*"  // Repeat the previous element 0 or more times
              < or
> ::= "+"  // Use '+' for one or more occurrences of the previous element (consider using '|' if representing OR operation)
<count> :: = "{" < number > "}"  // Repeat the previous element a specific number of times
             <
case-insensitivity> ::= "\I"  // Marks the previous element as case-insensitive
<capture> :: = "\O{number}"  // Captures the previous element for future reference
               < any_char > :: = "."  // Represents any character
                                 < word > :: =
<word-char> <word-char> | <word-char> <word>  // A word is at least two characters or any_char
<word-char> ::= <character> | <any_char>  // word-char can be any character or any_char
<character> :: = any
non-metacharacter  // Represents a specific character
#endif //LABB1_V4_TEST_H
