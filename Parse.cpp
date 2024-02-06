//
// Created by Nasir Alizade on 2024-02-06.
//
#include "Parse.h"

// <word> ::= <char><char> | <char>
op *Parse::parse_word() {
    auto word = new word_op();
    auto c = parse_character();
    while (c) {
        word->add(c);
        c = parse_character();
    }
    return word;
}

op *Parse::parse_character() {
    if (lexer.get_current().type == LETTER && isalnum(lexer.get_current().text[0])) {
        auto *c = new char_op(lexer.get_current().text[0]);
        lexer.advance();
        return c;
    }
    return nullptr;
}
