#include <iostream>
#include "Ops.h"
#include "lexer.h"

using IT = std::string::iterator;

op *get_parser_tree(IT &first, IT last);

op *parse_expr(IT &first, IT last);

op *parse_basic_RE(IT &first, IT last);

op *parse_operator(IT &first, IT last);

op *parse_repeat(IT &first, IT last);

op *parse_basic_element(IT &first, IT last);

op *parse_char(IT &first, IT last);

op *parse_group(IT &first, IT last);

op *parse_any_char(IT &first, IT last);

op *parse_or_op(IT &first, IT last);

op *parse_caseS(IT &first, IT last);

op *parse_capture(IT &first, IT last);

/*
<RE> ::= <expr>
<expr> ::= <basic-RE> | <operator>
<basic-RE> ::= <operator> [<expr>]
<operator> ::= <repeat> | <or>| <count>| <case-insensitivity> | <capture> |<basic-element>
<repeat> ::= <basic-element> "*"
<or> ::= <basic-element> "+" <basic-element>
<count> ::= <basic-element> "{" <number> "}"
<case-insensitivity> ::= <basic-element> "\I"
<capture> ::= <basic-element> "\O{number}"
<basic-element> ::=  <group> | <any_char> | <word>
<group> ::= "(" <basic-RE> ")"
<any_char> ::= "."
<word> ::= <character> | <character> <word>
<character> ::= any non metacharacter
 */


// <character> ::= any non metacharacter

op *parse_char(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type == tokenType::LETTER && is_valid_char(*first)) {
        auto *c = new char_op(*first);
        first++;
        return c;
    }
    if (tk.type == tokenType::ANY_CHAR) {
        first++;
        auto *any_char = new any_char_op();
        return any_char;
    }
    return nullptr;
}

op *parse_group(IT &first, IT last) {
    auto start = first;
    token tk = next_token(first, last);
    if (tk.type != tokenType::L_PAR) {
        return nullptr;
    }
    first++;
    auto *expr = parse_basic_RE(first, last);
    tk = next_token(first, last);
    if (tk.type != tokenType::R_PAR) {
        first = start;
        std::cerr << "Error:  expected ')' in group \n";
        return nullptr;
    }
    first++;
    auto *group = new group_op();
    group->add(expr);
    return group;
}

op *parse_any_char(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type != tokenType::ANY_CHAR) {
        return nullptr;
    }
    first++;
    auto *any_char = new any_char_op();
    return any_char;
}

// <word> ::= <character> | <character> <word>
op *parse_word(IT &first, IT last) {
    auto start = first;
    auto word = new word_op();
    while (first != last) {
        auto c = parse_char(first, last);
        if (!c) {
            break;
        }
        word->add(c);
    }
    if (!word->children.empty()) {
        return word;
    }
    first = start;
    delete word;
    return nullptr;
}

// <basic-element> ::=  <group> | <word> |
op *parse_basic_element(IT &first, IT last) {
    auto start = first;
    auto group_any_char_word = parse_group(first, last);
    if (!group_any_char_word) {
        group_any_char_word = parse_word(first, last);
        if (!group_any_char_word) {
            first = start;
            return nullptr;
        }
    }
    return group_any_char_word;
}

// <repeat> ::= <basic-element> "*"
op *parse_repeat(IT &first, IT last) { // TODO: fix
    auto start = first;
    auto basic_element = parse_basic_element(first, last);
    token tk = next_token(first, last);
    if (!basic_element) {
        first = start;
        return nullptr;
    }

    if (tk.type != tokenType::MULTI_OP) {
        first = start;
        return nullptr;
    }
    first++;
    auto star = new repeat_op();
    star->add(basic_element);
    return star;
}

// <or> ::= <basic-element> "+" <basic-element>
op *parse_or_op(IT &first, IT last) {
    auto start = first;
    auto lhs = parse_basic_element(first, last);
    if (!lhs) {
        return nullptr;
    }
    token tk = next_token(first, last);
    if (tk.type != tokenType::OR_OP) {
        first = start;
        return nullptr;
    }
    first++;
    auto rhs = parse_basic_element(first, last);
    if (!rhs) {
        first = start;
        return nullptr;
    }
    auto or_op_p = new or_op();
    or_op_p->add(lhs);
    or_op_p->add(rhs);
    return or_op_p;
}

// <case-insensitivity> ::= <basic-element> "\I"
op *parse_caseS(IT &first, IT last) {
    auto start = first;
    auto *basic_element = parse_basic_element(first, last);
    if (!basic_element) {
        first = start;
        return nullptr;
    }
    token tk = next_token(first, last);
    if (tk.type != tokenType::IGNORE) {
        first = start;
        return nullptr;
    }
    first++;
    first++;
    auto caseS = new case_sensitive_op();
    caseS->add(basic_element);
    return caseS;
}

// <capture> ::= <basic-element> "\O{number}"
op *parse_capture(IT &first, IT last) {
    auto start = first;
    auto *basic_element = parse_basic_element(first, last);
    if (!basic_element) {
        first = start;
        return nullptr;
    }
    token tk = next_token(first, last);
    if (tk.type != tokenType::OUTPUT_GROUP) {
        first = start;
        return nullptr;
    }
    first++;
    first++;
    tk = next_token(first, last);
    if (tk.type != tokenType::L_BRACKET) {
        first = start;
        return nullptr;
    }
    first++;
    tk = next_token(first, last);
    if (tk.type != tokenType::LETTER) {
        first = start;
        return nullptr;
    }
    first++;
    tk = next_token(first, last);
    int number = std::stoi(tk.text);
    if (tk.type != tokenType::R_BRACKET) {
        first = start;
        return nullptr;
    }
    first++;
    auto capture1 = new capture_group_op(number);
    capture1->add(basic_element);
    return capture1;
}

op *parse_count(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type != tokenType::L_BRACKET) {
        return nullptr;
    }
    first++;
    tk = next_token(first, last);
    if (tk.type != tokenType::LETTER) {
        return nullptr;
    }
    std::string number;
    while (tk.type == tokenType::LETTER) {
        number += tk.text;
        first++;
        tk = next_token(first, last);
    }
    if (tk.type != tokenType::R_BRACKET) {
        return nullptr;
    }
    first++;
    auto count = new count_op(std::stoi(number));
    return count;

}

// <operator> ::= <repeat> | <or>| <count>| <case-insensitivity> | <capture> |<basic-element>
// repeat =star
op *parse_operator(IT &first, IT last) {
    auto start = first;
    auto repeat_or_count_caseS_cap_elem = parse_repeat(first, last);
    if (!repeat_or_count_caseS_cap_elem) {
        repeat_or_count_caseS_cap_elem = parse_or_op(first, last);
        if (!repeat_or_count_caseS_cap_elem) {
            repeat_or_count_caseS_cap_elem = parse_count(first, last);
            if (!repeat_or_count_caseS_cap_elem) {
                repeat_or_count_caseS_cap_elem = parse_caseS(first, last);
                if (!repeat_or_count_caseS_cap_elem) {
                    repeat_or_count_caseS_cap_elem = parse_capture(first, last);
                    if (!repeat_or_count_caseS_cap_elem) {
                        repeat_or_count_caseS_cap_elem = parse_basic_element(first, last);
                        if (!repeat_or_count_caseS_cap_elem) {
                            first = start;
                            return nullptr;
                        }
                    }
                }
            }
        }
    }
    return repeat_or_count_caseS_cap_elem;
}

// <basic-RE> ::= <operator> [<expr>]
op *parse_basic_RE(IT &first, IT last) {
    auto op = parse_operator(first, last);
    if (first != last) {
        auto expr= parse_expr(first, last);
        if (op) {
            auto pragma = new program();
            pragma->add(op);
            pragma->add(expr);
            return pragma;
        }
    }
    return op;
}

// <expr> ::= <basic-RE> | <operator>
op *parse_expr(IT &first, IT last) {
    auto *basic_RE = parse_basic_RE(first, last);
    if (basic_RE) {
        return basic_RE;
    }
    auto *op = parse_operator(first, last);
    if (op) {
        return op;
    }
    return nullptr;
}

//<RE> ::= <expr>
op *get_parser_tree(IT &first, IT last) {
    auto expr = parse_expr(first, last);
    auto *p = new program();
    p->add(expr);
    return p;
}

int main() {
    // Input string
    std::string input = "l*";
    auto first = input.begin();
    auto last = input.end();
    std::string text = "Waterloo I was defeated, you won the war Waterloo promise to love you for ever more Waterloo couldn't escape if I wanted to Waterloo knowing my fate is to be with you Waterloo finally facing my Waterloo";
    std::string text1 = "abcdefg";
    // Create the parser tree
    auto tree = get_parser_tree(first, last);
    char *begin = &*text.begin();
    char *end = &*text.end();
    // Evaluate the input against the regular expression
    if (tree->eval(begin, end)) {
        std::cout << "The input matches the regular expression." << std::endl;
    } else {
        std::cout << "The input does not match the regular expression." << std::endl;
    }

    // Cleanup (in real code, use smart pointers to manage memory automatically)


    return 0;
}
