//
// Created by Nasir Alizade on 2024-02-06.
// Program: TDTEA.  : HT2021 - Labb 1
//
#include "Parse.h"

/**
*<REGEXP>   ::= <EXPR>
*<EXPR>     ::= <MATCH> [<EXPR>]
*<MATCH>    ::= <IGNORE> | <OR> |<GROUP> | <OUTPUTGROUP> | <WORD>
*<OR>       ::= <WORD> + <WORD>
*<IGNORE>   ::= <GROUP>\I | <REPEAT>\I | <WORD>\I
•<REPEAT>   ::= <CHAR>* | <CHAR><COUNT>
*<WORD>     ::= <REPEAT> | <CHAR><WORD> | <CHAR>
*<CHAR>     ::= <ANY> | NOT [{,},(,),.,\I,\O ]
*<ANY>      ::= .
*<GROUP>    ::= (MATCH)
*<COUNT>    ::= {< INTEGER>}
*<OUTPUTGROUP>::=\O{<INTEGER>｝
*<INTEGER>  ::= 0 .. 9
*/
// <EXPR>     ::= <MATCH> [<EXPR>]
expr_op *Parse::parse_expr() {
    auto match = parse_match();
    if (!match) {
        return nullptr;
    }
    auto expr = new expr_op();
    expr->add(match);
    auto next = parse_expr();
    if (next) {
        expr->add(next);
    }
    return expr;
}

// <MATCH> ::= <IGNORE> | <OR> |<GROUP> | <OUTPUTGROUP> | <WORD>
match_op *Parse::parse_match() {
    auto ignore = parse_ignore();
    if (ignore) {
        auto match = new match_op();
        match->add(ignore);
        return match;
    }
    auto orOp = parse_or();
    if (orOp) {
        auto match = new match_op();
        match->add(orOp);
        return match;
    }
    auto group = parse_group();
    if (group) {
        auto match = new match_op();
        match->add(group);
        return match;
    }
    auto outputGroup = parse_output_group();
    if (outputGroup) {
        auto match = new match_op();
        match->add(outputGroup);
        return match;
    }
    auto word = parse_word();
    if (word) {
        auto match = new match_op();
        match->add(word);
        return match;
    }
    return nullptr;
}

// <IGNORE> ::= <GROUP>\I | <ASTERISK>\I | <WORD>\I
ignore_case_op *Parse::parse_ignore() {
    auto start = lexer;
    auto group = parse_group();
    if (group) {
        if (lexer.get_current().type == SLASH) {
            lexer.advance();
            if (lexer.get_current().text[0] == 'I') {
                lexer.advance();
                auto ignore = new ignore_case_op();
                ignore->add(group);
                return ignore;
            }
        }
        lexer = start;
    }
    auto repeat = parse_repeat();
    if (repeat) {
        if (lexer.get_current().type == SLASH) {
            if (lexer.get_current().text[0] == 'I') {
                auto ignore = new ignore_case_op();
                ignore->add(repeat);
                return ignore;
            }
        }
        lexer = start;
    }
    auto word = parse_word();
    if (word) {
        if (lexer.get_current().type == SLASH) {
            if (lexer.get_current().text[0] == 'I') {
                auto ignore = new ignore_case_op();
                ignore->add(word);
                return ignore;
            }
        }
        lexer = start;
    }
    lexer = start;
    return nullptr;
}

//<GROUP> ::= (MATCH)
capture_group_op *Parse::parse_group() {
    if (lexer.get_current().type == L_PAR) {
        lexer.advance();
        auto match = parse_match();
        if (match) {
            if (lexer.get_current().type == R_PAR) {
                lexer.advance();
                auto group = new capture_group_op();
                group->add(match);
                return group;
            } else {
                throw std::runtime_error("Expected )");
            }
        }
    }
    return nullptr;
}

// <REPEAT>   ::= <CHAR>* | <CHAR><COUNT>
repeat_op *Parse::parse_repeat() {
    auto start = lexer;
    auto charOp = parse_char();
    if (charOp) {
        if (lexer.get_current().type == ASTERISK) {// ASTERISK
            lexer.advance();
            auto repeat = new repeat_op();
            repeat->add(charOp);
            return repeat;
        }
        auto count = parse_count();
        if (count) {
            auto repeat = new repeat_op();
            repeat->add(charOp);
            repeat->add(count);
            return repeat;
        }
    }
    lexer = start;
    return nullptr;
}
// <CHAR> ::= <ANY> | NOT [{,},(,),.,\I,\O ]
op *Parse::parse_char() {
    if (lexer.get_current().type == ANY_CHAR) {
        lexer.advance();

        return new any_char_op();
    }
    if (lexer.get_current().type == LETTER) {
        auto c = new char_op(lexer.get_current().text[0]);
        lexer.advance();
        return c;
    }
    return nullptr;
}
// <COUNT> ::= {< INTEGER>}
count_op *Parse::parse_count() {
    if (lexer.get_current().type == L_BRACKET) {
        lexer.advance();
        std::string number;
        while (lexer.get_current().type == LETTER) {
            number += lexer.get_current().text[0];
            lexer.advance();
        }
        if (lexer.get_current().type != R_BRACKET) {
            throw std::runtime_error("Expected }");
        }
        return new count_op(std::stoi(number));
    }
    return nullptr;
}
// <OR> ::= <WORD> + <WORD>
or_op *Parse::parse_or() {
    auto start = lexer;
    auto lhs = parse_word();
    if (lhs) {
        if (lexer.get_current().type == OR_OP) {
            lexer.advance();
            auto rhs = parse_word();
            if (rhs) {
                auto orOp = new or_op();
                orOp->add(lhs);
                orOp->add(rhs);
                return orOp;
            }
        }
    }
    lexer = start;
    return nullptr;
}
// <WORD> ::= <REPEAT> | <CHAR><WORD> | <CHAR>
word_op *Parse::parse_word() {
    auto repeat = parse_repeat();
    if (repeat) {
        auto word = new word_op();
        word->add(repeat);
        return word;
    }
    auto charOp = parse_char();
    if (charOp) {
        auto word = new word_op();
        word->add(charOp);
        auto next = parse_word();
        if (next) {
            word->add(next);
        }
        return word;
    }
    return nullptr;
}
// <OUTPUTGROUP>::=\O{<INTEGER>}
output_group_op *Parse::parse_output_group() {
    auto start = lexer;
    if (lexer.get_current().type == SLASH) {
        lexer.advance();
        if(lexer.get_current().text[0]=='O') {
            lexer.advance();
            if (lexer.get_current().type == L_BRACKET) {
                lexer.advance();
                std::string number;
                while (lexer.get_current().type == LETTER) {
                    number += lexer.get_current().text[0];
                    lexer.advance();
                }
                if (lexer.get_current().type != R_BRACKET) {
                    throw std::runtime_error("Expected }");
                }
                group_index_ = std::stoi(number);
                return new output_group_op(std::stoi(number));
            }
        }
    }
    lexer = start;
    return nullptr;
}

bool Parse::match_from_any_pos(const std::string &input) {
    char* first = const_cast<char*>(input.c_str());
    char* last = first + input.size();
    auto expr = parse_expr();
    if (!expr) {
        return false;
    }
    print_tree(expr);
    while (first != last) {
        auto start = first;
        if (expr->eval(first, last)) {
            if (group_index_ > 0) {
                std::cout << "Matched: " << capturedGroups[group_index_-1] << std::endl;
            } else {
                std::cout << "Matched: " << std::string(start, first) << std::endl;
            }
            return true;
        }
        first=start+1;
    }
    return false;
}
