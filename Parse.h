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

    op *parse_word();

    op *parse_character();

private:
    Lexer lexer;
};


#endif //LABB1_V4_PARSE_H
