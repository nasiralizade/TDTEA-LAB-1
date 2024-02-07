//
// Created by Nasir Alizade on 2024-02-06.
//
#include "Parse.h"

op *Parse::parse_expr() {
    auto lhs = parse_sequence();
    while (lexer.get_current().type == OR_OP) {
        lexer.advance();
        auto rhs = parse_sequence();
        auto or_op_p = new or_op();
        or_op_p->add(lhs);
        or_op_p->add(rhs);
        lhs = or_op_p;
    }
    return lhs;
}

/**
 * Sequence  ::= Element ('*' Element)*
 * @return
 */
op *Parse::parse_sequence() {
    auto lhs = parse_element();
    auto seq_op_p = new Sequence();
    seq_op_p->add(lhs);
    while (lexer.get_current().type != END && lexer.get_current().type != OR_OP && lexer.get_current().type != R_PAR) {
        if (lexer.get_current().type == REPEAT) {
            lexer.advance();
            auto repeat_op_p = new repeat();
            repeat_op_p->add(seq_op_p->children.back());
            seq_op_p->children.pop_back();
            seq_op_p->add(repeat_op_p);
        } else {
            auto next = parse_element();
            seq_op_p->add(next);
        }
    }
    if (seq_op_p->children.size() == 1) {
        auto single = seq_op_p->children.front();
        seq_op_p->children.clear();
        delete seq_op_p;
        return single;
    }
    return seq_op_p;
}

op *Parse::parse_element() {
    auto component = parse_component();
    if (lexer.get_current().type == L_BRACKET) {
        lexer.advance();
        std::string number;
        while (lexer.get_current().type == LETTER) {
            number += lexer.get_current().text;
            lexer.advance();
        }
        if (lexer.get_current().type != R_BRACKET) {
            std::cerr << "missing }\n";
        }
        lexer.advance();
        auto count_op = new exact_op(std::stoi(number));
        count_op->add(component);
        return count_op;
    }
    return component;
}

op *Parse::parse_component() {
    auto tk = lexer.get_current();
    if (tk.type == L_PAR) {
        lexer.advance();
        auto expr = parse_expr();
        lexer.advance();
        return expr;
    } else if (tk.type == ANY_CHAR) {
        lexer.advance();
        return new any_char_op();
    } else if (tk.type == SLASH) {
        lexer.advance();
        char next = lexer.get_current().text[0];
        if (next == 'I') {
            lexer.advance();
            return new ignore_case_op(parse_component());
        } else if (next == 'O') {
            lexer.advance();
            lexer.advance(); //skip the next {
            std::string groupNum;
            while (lexer.get_current().type == LETTER) {
                groupNum += lexer.get_current().text;
                lexer.advance();
            }
            lexer.advance(); //skip the next }
            return new output_group_op(parse_component(), std::stoi(groupNum));
        }
    } else if (is_valid_char(tk.text[0])) {
        char c = tk.text[0];
        lexer.advance();
        return new char_op(c);
    }
    throw std::runtime_error("Unspeckted token in parse_component");
}

bool Parse::is_valid_char(char c) {
    return c != '*' && c != '+' && c != '(' && c != ')' && c != '{' && c != '}' && c != '\\';
}

bool Parse::match_from_any_pos(std::string &input) {
    char *first = &input[0];
    char *last = &input[input.size()];
    auto pattern = parse_expr();
    while (first != last) {
        char *temp = first;
        if (pattern->eval(first, last)) {
            delete pattern;
            matchedSubstring = std::string(temp, first);
            std::cout << "Matched: " << matchedSubstring << std::endl; // Skriv ut den matchade delsträngen
            return true;
        }
        first = temp + 1;
    }
    delete pattern;
    return false;
}








