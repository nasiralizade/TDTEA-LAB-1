//
// Created by Nasir Alizade on 2024-02-06.
//

#ifndef LABB1_V4_PARSE_H
#define LABB1_V4_PARSE_H

#include "lexer.h"
#include "Ops.h"
#include <iostream>

using IT = std::string::iterator;

class Parse {
public:
    Parse(IT first, IT last) : lexer(first, last) {}

    ~Parse() = default;
    op *parse_expr();
    op *parse_sequence();

    op *parse_element();

    op *parse_component();

    bool is_valid_char(char c);

    bool match_from_any_pos(std::string &input);

    std::string matchedSubstring;


private:

    bool is_output_set = false;
    Lexer lexer;
};


#endif //LABB1_V4_PARSE_H
