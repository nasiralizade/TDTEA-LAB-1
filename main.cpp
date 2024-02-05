#include <iostream>
#include "Ops.h"
#include "lexer.h"

using IT = std::string::iterator;

op *get_parser_tree(IT &first, IT last);
op *parse_expr(IT &first, IT last);
op *parse_basic_RE(IT &first, IT last);
op *parse_element(IT &first, IT last);
op *parse_basic_element(IT &first, IT last);
op *parse_group(IT &first, IT last);
op *parse_word(IT &first, IT last);
op *parse_char(IT &first, IT last);
op *parse_operator(IT &first, IT last);
op *parse_repeat(IT &first, IT last);
op *parse_or(IT &first, IT last);
op *parse_count(IT &first, IT last);
op *parse_case_insensitivity(IT &first, IT last);
op *parse_capture(IT &first, IT last);



int main() {
    // Input string
    std::string input = "(lo+be)";
    std::string x = "loo couldn't be here";
    IT first = input.begin();
    IT last = input.end();
    auto tree = get_parser_tree(first, last);

    auto start = x.begin();
    auto end = x.end();
    char *first_char = &*start;
    char *last_char = &*end;
    auto result = tree->eval(first_char, last_char);
    print_tree(tree);
    if (result) {
        std::cout << "Matched" << std::endl;
    } else {
        std::cout << "Not Matched" << std::endl;
    }

    return 0;
}

op *get_parser_tree(IT &first, IT last) {
    return parse_expr(first, last);
}

// <expr> ::=  <basic-RE> "+" <expr> | <basic-RE>
op *parse_expr(IT &first, IT last) {
    auto lhs = parse_basic_RE(first, last);
    if (!lhs) {
        return nullptr;
    }
    token tk = next_token(first, last);
    if (first != last && tk.type == OR_OP) {
        first++;
        auto right = parse_expr(first, last);
        if (!right) {
            return nullptr;
        }
        auto or_op_p = new or_op();
        or_op_p->add(lhs);
        or_op_p->add(right);
        return or_op_p;
    }
    return lhs;
}

// <basic-RE> ::=  <element> <basic-RE> | <element>
op *parse_basic_RE(IT &first, IT last) {
    auto element = parse_element(first, last);
    if (!element) {
        return nullptr;
    }
    auto next_element = parse_basic_RE(first, last);
    if (next_element) {
        auto basic_RE_p = new basic_RE();
        basic_RE_p->add(element);
        basic_RE_p->add(next_element);
        return basic_RE_p;
    }
    return element;
}

// <element> ::= <operator> <basic-element> | <basic-element>
op *parse_element(IT &first, IT last) {
    auto start = first;
    auto op = parse_operator(first, last);
    if (op) {
        auto basic_element = parse_basic_element(first, last);
        if (!basic_element) {
            return op;
        }
        auto element = new element_op();
        element->add(op);
        element->add(basic_element);
        return element;
    }
    first = start;
    return parse_basic_element(first, last);
}

// <operator> ::= <repeat> | <or> | <count> | <case-insensitivity> | <capture>
op *parse_operator(IT &first, IT last) {
    auto start = first;
    auto repeat = parse_repeat(first, last);
    if (repeat) {
        return repeat;
    }
    first = start;
    auto or_op = parse_or(first, last);
    if (or_op) {
        return or_op;
    }
    first = start;
    auto count = parse_count(first, last);
    if (count) {
        return count;
    }
    first = start;
    auto case_insensitivity = parse_case_insensitivity(first, last);
    if (case_insensitivity) {
        return case_insensitivity;
    }
    first = start;
    return parse_capture(first, last);
}

op *parse_capture(IT &first, IT last) {
    token tk = next_token(first, last);
    if (first != last && tk.type == OUTPUT_GROUP) {
        std::advance(first, 3); // skip \O{
        std::string number;
        while (first != last && next_token(first, last).type != R_BRACKET) {
            number += next_token(first, last).text;
            first++;
        }
        if (next_token(first, last).type != R_BRACKET) {
            return nullptr;
        }
        first++;
        return new capture_group_op(std::stoi(number));
    }
    return nullptr;
}

op *parse_case_insensitivity(IT &first, IT last) {
    token tk = next_token(first, last);
    if (first != last && tk.type == IGNORE) {
        first++;
        first++;
        return new case_sensitive_op();
    }
    return nullptr;
}

op *parse_count(IT &first, IT last) {
    if (first == last || next_token(first, last).type != L_BRACKET) {
        return nullptr;
    }
    first++;
    std::string number;
    while (first != last && next_token(first, last).type != R_BRACKET) {
        number += next_token(first, last).text;
        first++;
    }
    if (next_token(first, last).type != R_BRACKET) {
        return nullptr;
    }
    first++;
    return new count_op(std::stoi(number));
}

op *parse_or(IT &first, IT last) {
    if (first != last && next_token(first, last).type == OR_OP) {
        first++;
        return new or_op();
    }
    return nullptr;
}

op *parse_repeat(IT &first, IT last) {
    token tk = next_token(first, last);
    auto prev = first - 1;
    if (first != last && tk.type == MULTI_OP) {
        first++;
        auto repeat = new repeat_op();
        repeat->add(parse_basic_element(prev, last));
        return repeat;
    }
    return nullptr;
}

// <basic-element> ::= <group> | <word>
op *parse_basic_element(IT &first, IT last) {
    auto group = parse_group(first, last);
    if (group) {
        return group;
    }
    return parse_word(first, last);
}

// <word> ::= <word-char> <word-char>
op *parse_word(IT &first, IT last) {
    auto word = new word_op();
    while (first != last) {
        auto c = parse_char(first, last);
        if (c) {
            word->add(c);
        } else {
            break;
        }

    }
    if (!word->children.empty()) {
        return word;
    }
    delete word;
    return nullptr;
}

op *parse_char(IT &first, IT last) {
    token tk = next_token(first, last);
    if (tk.type == LETTER) {
        auto c = new char_op(tk.text[0]);
        first++;
        return c;
    }
    if (tk.type == ANY_CHAR) {
        auto c = new any_char_op();
        first++;
        return c;
    }

    return nullptr;
}

// <group> ::= "(" <expr> ")"
op *parse_group(IT &first, IT last) {
    token tk = next_token(first, last);
    if (first == last || tk.type != L_PAR) {
        return nullptr;
    }
    first++;
    auto expr = parse_expr(first, last);
    if (!expr) {
        return nullptr;
    }
    tk = next_token(first, last);
    if (first == last || tk.type != R_PAR) {
        return nullptr;
    }
    first++;
    auto group = new group_op();
    group->add(expr);
    return group;
}
