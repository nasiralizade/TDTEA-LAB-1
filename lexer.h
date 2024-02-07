//
// Created by Nasir Alizade on 2024-02-02.
//

#ifndef LABB1_V4_LEXER_H
#define LABB1_V4_LEXER_H

#include <iostream>
/**
 * Enum for the different types of tokens
 */
enum tokenType{
    LETTER,
    L_PAR,
    R_PAR,
    L_BRACKET,
    R_BRACKET,
    REPEAT,
    ANY_CHAR,
    OR_OP,
    SLASH,
    OUTPUT_GROUP,
    IGNORE_CASE,
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

/**
 * Function to check if the character is valid
 * @param c the character
 * @return true if the character is valid
 * valid characters are all characters except for * + ( ) { } \

bool is_valid_char(char c) {
    return c != '*' && c != '+' && c != '(' && c != ')' && c != '{' && c != '}' && c != '\\';
}

void print_tree(op *root, const std::string &prefix = "", const std::string &children_prefix = "") {
    if (root == nullptr) {
        return;
    }

    // Print the current node's ID with the current prefix
    std::cout << prefix << root->id() << std::endl;

    // Process all children except the last one, adding branches and adjusting the children_prefix
    for (size_t i = 0; i < root->children.size(); ++i) {
        auto next_prefix = children_prefix + (i < root->children.size() - 1 ? "├── " : "└── ");
        auto next_children_prefix = children_prefix + (i < root->children.size() - 1 ? "│   " : "    ");
        print_tree(root->children[i], next_prefix, next_children_prefix);
    }
}
*/
#endif //LABB1_V4_LEXER_H
