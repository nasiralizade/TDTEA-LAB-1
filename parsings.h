//
// Created by Nasir Alizade on 2024-02-05.
//

#ifndef LABB1_V4_PARSINGS_H
#define LABB1_V4_PARSINGS_H

#include "lexer.h"
#include "Ops.h"

using IT = std::string::iterator;

op *parse_basic_RE(IT &first, IT last);

op *parse_repeat(IT &first, IT last);

op *parse_or(IT &first, IT last);

op *parse_count(IT &first, IT last);

op *parse_case_insensitivity(IT &first, IT last);

op *parse_capture(IT &first, IT last);

op *parse_operator(IT &first, IT last);

op *parse_element(IT &first, IT last);

op *parse_expr(IT &first, IT last);


/*
 -----------------------------------------------------------------------------------------------------------------------------------------
<RE> ::= <expr>
<expr> ::=  <basic-RE> <expr> | <basic-RE>
<basic-RE> ::=  <element> <basic-RE> | <element>
<element> ::=  <basic-element> <operator> | <or> | <basic-element>
<basic-element> ::= <group>  | <word>
<group> ::= "(" <expr> ")"
<or> ::= <basic-element> "+" <basic-element>
<operator> ::= <repeat>  | <count> | <case-insensitivity> | <capture>
<repeat> ::= "*"
<count> ::= "{" <number> "}"
<case-insensitivity> ::= "\I"
<capture> ::= "\O{number}"
<any_char> ::= "."
<word> ::= <word-char> <word-char> | <word-char> <word>
<word-char> ::= <character> | <any_char>
<character> ::= any non-metacharacter // any character except n\,\,{,},(,),[,],\,^,,|,*,+,
-----------------------------------------------------------------------------------------------------------------------------------------
 */
op *parse_group(IT &first, IT last);

op *parse_word(IT &first, IT last);

op *parse_character(IT &first, IT last);

// <basic-element> ::= <group> |  <word>
op *parse_basic_element(IT &first, IT last) {
    auto group = parse_group(first, last);
    if (group) {
        return group;
    }
    auto word = parse_word(first, last);
    if (word) {
        return word;
    }
    return nullptr;
}

// <word> ::= <word-char> <word | <word-char>
op *parse_word(IT &first, IT last) {
    auto c = parse_character(first, last);
    if (c) {
        auto word = parse_word(first, last);
        if (word) {
            auto word_op_p = new word_op();
            word_op_p->add(c);
            word_op_p->add(word);
            return word_op_p;
        }
        return c;
    }
    return nullptr;
}

op *parse_character(IT &first, IT last) {
    token tk = next_token(first, last);

    if (tk.type == LETTER && is_valid_char(tk.text[0])) {
        first++;
        return new char_op(tk.text[0]);
    }
    if (tk.type == ANY_CHAR) {
        first++;
        return new any_char_op();
    }
    return nullptr;
}

op *parse_group(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type == L_PAR) {
        first++;
        auto expr = parse_expr(first, last);
        if (expr) {
            tk = next_token(first, last);
            if (tk.type == R_PAR) {
                first++;
                return expr;
            }
        }
    }
    return nullptr;
}

op *parse_repeat(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type == REPEAT) {
        first++;
        return new repeat_op();
    }
    return nullptr;
}

// <or> ::= <basic-element> "+" <basic-element>
op *parse_or(IT &first, IT last) {
    auto lhs = parse_basic_element(first, last);
    if (!lhs) {
        return nullptr;
    }
    token tk = next_token(first, last);
    if (tk.type == OR_OP) {
        first++;
        auto rhs = parse_basic_element(first, last);
        if (rhs) {
            auto or_op_p = new or_op();
            or_op_p->add(lhs);
            or_op_p->add(rhs);
            return or_op_p;
        }
    }
    return nullptr;
}

op *parse_count(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type == L_BRACKET) {
        first++;
        std::string number;
        while (first != last && next_token(first, last).type != R_BRACKET) {
            number += next_token(first, last).text;
            first++;
        }
        if (next_token(first, last).type == R_BRACKET) {
            first++;
            return new count_op(std::stoi(number));
        }
    }
    return nullptr;
}

op *parse_case_insensitivity(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type == IGNORE_CASE) {
        first++;
        first++;
        return new case_sensitive_op();
    }
    return nullptr;
}

op *parse_capture(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type == OUTPUT_GROUP) {
        first++;
        first++;
        std::string number;
        while (first != last && next_token(first, last).type != R_BRACKET) {
            number += next_token(first, last).text;
            first++;
        }
        if (next_token(first, last).type == R_BRACKET) {
            first++;
            return new capture_group_op(std::stoi(number));
        }
    }
    return nullptr;
}

// <operator> ::= <repeat>  | <count> | <case-insensitivity> | <capture>
op *parse_operator(IT &first, IT last) {
    auto operator_p = parse_repeat(first, last);
    if (!operator_p) {
        operator_p = parse_count(first, last);
        if (!operator_p) {
            operator_p = parse_case_insensitivity(first, last);
            if (!operator_p) {
                operator_p = parse_capture(first, last);
            }
            if (!operator_p) {
                return nullptr;
            }
        }
    }
    return operator_p;
}


// <element> ::=   <operator> | <or> | <basic-element>
op *parse_element(IT &first, IT last) {
    auto basic_element = parse_basic_element(first, last);
    if (!basic_element) {
        return nullptr;
    }
    auto operator_p = parse_operator(first, last);
    if (operator_p) {
        auto element_op_p = new element_op();
        element_op_p->add(operator_p);
        element_op_p->add(basic_element);
        return element_op_p;
    }
    return basic_element;
}

// <basic-RE> ::=  <element> <basic-RE> | <element>
op *parse_basic_RE(IT &first, IT last) {
    auto element = parse_element(first, last);
    if (!element) {
        return nullptr;
    }
    token tk = next_token(first, last);
    if (first != last) {
        auto rhs = parse_basic_RE(first, last);
        if (rhs) {
            auto basic_RE_op_p = new basic_RE();
            basic_RE_op_p->add(element);
            basic_RE_op_p->add(rhs);
        }
    }
    return element;
}

// <expr> ::=  <basic-RE> <expr> | <basic-RE>
op *parse_expr(IT &first, IT last) {
    auto lhs = parse_basic_RE(first, last);
    if (!lhs) {
        return nullptr;
    }
    auto expr = parse_expr(first, last);
    if (expr) {
        auto expr_op_p = new expr_op();
        expr_op_p->add(lhs);
        expr_op_p->add(expr);
    }
    return lhs;
}


// <RE> ::= <expr>
op *get_parser_tree(std::string::iterator &first, std::string::iterator last) {
    return parse_expr(first, last);
}

#endif //LABB1_V4_PARSINGS_H
