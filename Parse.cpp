//
// Created by Nasir Alizade on 2024-02-06.
//
#include "Parse.h"

/**
 * expr   ::= Sequence ('+' Sequence)*
 * @return
 */
op *Parse::parse_expr() {
    auto seq_op_p = new Sequence();
    while (lexer.get_current().type != END && lexer.get_current().type != OR_OP) {
        auto seq = parse_sequence();
        seq_op_p->add(seq);
    }
    while (lexer.get_current().type == OR_OP) {
        lexer.advance();
        auto rhs = parse_sequence();
        auto or_op_p = new or_op();
        or_op_p->add(seq_op_p);
        or_op_p->add(rhs);
        seq_op_p = reinterpret_cast<Sequence *>(or_op_p);
    }
    return seq_op_p;
}

/**
 * Sequence  ::= Element ('*' Element)*
 * @return
 */
op *Parse::parse_sequence() {

    auto lhs = parse_element();
    auto seq_op_p = new Sequence();
    seq_op_p->add(lhs);
    while (lexer.get_current().type == REPEAT) {
            lexer.advance();
            auto repeat_op_p = new repeat();
            repeat_op_p->add(seq_op_p->children.back());
            seq_op_p->children.pop_back();
    }
    return seq_op_p;
}

/**
 * Element   ::= Component ('{n}' Component)*
 * @return
 */
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

/**
 * Component  ::= CHAR | '.' | '(' Pattern ')' | Pattern '\I' | Pattern '\O{' DIGIT '}'
 * @return
 */
op *Parse::parse_component() {

    if (lexer.get_current().type == L_PAR) {
        lexer.advance();
        auto expr = parse_expr();
        auto group = new capture_group_op();
        group->add(expr);
        lexer.advance();
        return group;
    } else if (lexer.get_current().type == ANY_CHAR) {
        lexer.advance();
        return new any_char_op();
    } else if (lexer.get_current().type == SLASH) {
        lexer.advance();
        char next = lexer.get_current().text[0];
        if (next == 'I') {
            lexer.advance();
            return new ignore_case_op(parse_component());
        } else if (next == 'O') {
            lexer.advance();
            lexer.advance(); //skip the next '{'
            std::string groupNum;
            while (lexer.get_current().type == LETTER) {
                groupNum += lexer.get_current().text;
                lexer.advance();
            }
            lexer.advance(); //skip the next '}'

            return new output_group_op(std::stoi(groupNum));
        }
    } else if (lexer.get_current().type == LETTER) {
        auto letter = lexer.get_current().text[0];
        lexer.advance();
        return new char_op(letter);
    }
    throw std::runtime_error("Invalid character");
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
            if (group_index_ > 0) {
                std::cout << "Group index: " << group_index_ << std::endl;
                if (group_index_ > capturedGroups.size()) {
                    std::cerr << "Invalid group index\n";
                    return false;
                }
                std::cout << "Captured group: " << capturedGroups[group_index_ - 1] << std::endl;
                return true;
            }
            std::cout << "Matched: " << matchedSubstring << std::endl; // Skriv ut den matchade delsträngen
            return true;
        }
        first = temp + 1;
    }
    delete pattern;
    return false;
}








