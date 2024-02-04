//
// Created by Nasir Alizade on 2024-02-02.
//

#ifndef LABB1_V4_LEXER_H
#define LABB1_V4_LEXER_H
#include <string>
enum tokenType{
    LETTER,
    L_PAR,
    R_PAR,
    L_BRACKET,
    R_BRACKET,
    MULTI_OP,
    ANY_CHAR,
    OR_OP,
    SLASH,
    OUTPUT_GROUP,
    IGNORE,
    END
};

struct token {
    tokenType type;
    std::string text;
};
token next_token( std::string::iterator& first,  std::string::iterator& last) {
    if( first == last) {
        return { END, ""};
    }
    token tk;
    switch(*first) {
        case '*':
            tk = {MULTI_OP, "*"};
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
            if (++first != last) {
                if (*first == 'I') {
                    tk = {IGNORE, "\\I"};
                } else if (*first == 'O') {
                    tk = {OUTPUT_GROUP, "\\O"};
                } else {
                    tk = {SLASH, "\\"};
                }
                break;
            }
        default:
            tk = {LETTER, std::string(first, first+1)};
    };
    return tk;
}

bool is_valid_char(char c) {
    return c != '*' && c != '+' && c != '(' && c != ')' && c != '{' && c != '}' && c != '\\';
}

#endif //LABB1_V4_LEXER_H
