//
// Created by Nasir Alizade on 2024-02-02.
//

#ifndef LABB1_V4_LEXER_H
#define LABB1_V4_LEXER_H

#include <iostream>
/**
 * Enum for the different types of tokens
 * @param LETTER A-Z, a-z, 0-9,except for * + ( ) { } \
 * @param L_PAR (
 * @param R_PAR )
 * @param L_BRACKET {
 * @param R_BRACKET }
 * @param REPEAT *
 * @param ANY_CHAR .
 * @param OR_OP +
 * @param SLASH \
 * @param OUTPUT_GROUP \\O{n}
 * @param IGNORE_CASE \\I
 * @param END end of the string
 */
enum tokenType {
    LETTER,
    L_PAR,
    R_PAR,
    L_BRACKET,
    R_BRACKET,
    REPEAT,
    ANY_CHAR,
    OR_OP,
    SLASH,
    END
};

class Lexer {
private:
    std::string::iterator first;
    std::string::iterator last;

    struct Token {
        tokenType type;
        std::string text;
    };

    Token current;
    Token prev;
    Token next;

public:
    Lexer(std::string::iterator first, std::string::iterator last) : first(first), last(last) {
        current = next_token();
        next = next_token();
    }

    Token next_token() {
        if (first == last) {
            return {END, ""};
        }

        Token tk;
        switch (*first) {
            case '*':
                tk = {REPEAT, "*"};
                break;
            case '.':
                tk = {ANY_CHAR, "."};
                break;
            case '+':
                tk = {OR_OP, "+"};
                break;
            case '(':
                tk = {L_PAR, "("};
                break;
            case ')':
                tk = {R_PAR, ")"};
                break;
            case '{':
                tk = {L_BRACKET, "{"};
                break;
            case '}':
                tk = {R_BRACKET, "}"};
                break;
            case '\\':
                tk = {SLASH, "\\"};
                break;
            default:
                tk = {LETTER, std::string(first, first + 1)};
        };

        ++first;
        return tk;
    }

    void advance() {
        prev = current;
        current = next;
        next = next_token();
    }

    Token get_current() const {
        return current;
    }

    Token get_prev() const {
        return prev;
    }

    Token get_next() const {
        return next;
    }
};

#endif //LABB1_V4_LEXER_H
