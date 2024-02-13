//
// Created by Nasir Alizade on 2024-02-06.
//
#include "Parse.h"

/**
 * expr   ::= <Subexpression> ('+' <Subexpression>)*
 * @return
 */
op *Parse::parse_expr() {
    auto lhs = parse_sub_expr();
    return lhs;
}

//Subexpression ::= element
// | element Modifier
// | element Modifier Subexpression
// | element Subexpression
op *Parse::parse_sub_expr() {
    auto lhs = parse_element();
    if (!lhs) {
        throw std::runtime_error("Expected element");
    }

    // Handle modifiers and subexpressions recursively
    while (true) {
        auto modif = parse_operation(lhs);
        if (modif) {
            lhs = modif;  // Apply modifier to the left-hand side
        } else if (lexer.get_current().type != END) {
            // Parse a subexpression, wrapping in a Subexpression object if necessary
            auto subExpr = parse_sub_expr();
            if (subExpr) {

                auto lhs1 = new Subexpression();
                lhs1->add(lhs);
                lhs1->add(subExpr);
                lhs = lhs1;
                return lhs;
            }
        } else {
            break;  // End of input
        }
    }

    return lhs;
}


bool Parse::match_from_any_pos(std::string &input) {

    auto first = input.begin();
    auto last = input.end();
    auto expr = parse_expr();

    return 0;
}

int Parse::get_number() {
    if (lexer.get_current().type == L_BRACKET) {
        lexer.advance();
        std::string number;
        while (lexer.get_current().type != R_BRACKET) {
            number += lexer.get_current().text;
            lexer.advance();
        }
        lexer.advance();
        return std::stoi(number);
    }
    throw std::runtime_error("Expected {");
}

op *Parse::parse_element() {
    auto tk = lexer.get_current().type;
    if (tk == L_PAR) {
        lexer.advance();
        auto expr = parse_expr();
        if (lexer.get_current().type != R_PAR) {
            throw std::runtime_error("Expected )");
        }
        lexer.advance();
        return expr;
    } else if (tk == ANY_CHAR) {
        lexer.advance();
        return new any_char_op();
    } else if (tk == LETTER) {
        auto c = lexer.get_current().text[0];
        lexer.advance();
        return new char_op(c);
    } else {
        return nullptr;
    }
}

op *Parse::parse_operation(op *lhs) {
    auto start = lexer;
    switch (lexer.get_current().type) {
        case REPEAT: {
            lexer.advance();
            auto repeatOp = new repeat_op();
            repeatOp->add(lhs);
            return repeatOp;
        }
        case OR_OP: {
            lexer.advance();
            auto rhs = parse_element();
            if (!rhs) {
                throw std::runtime_error("Expected element");
            }
            auto orOp = new or_op();
            orOp->add(lhs);
            orOp->add(rhs);
            return orOp;
        }
        case L_BRACKET: {
            lexer.advance();
            auto count = get_number();
            auto countOp = new exact_op(count);
            countOp->add(lhs);
            return countOp;
        }
        case SLASH: {
            lexer.advance();
            return handleSlash(lhs);
        }
        default:
            return nullptr;
    }
}

op *Parse::handleSlash(op *lhs) {
    char next = lexer.get_current().text[0];
    if (next == 'I') {
        lexer.advance();
        auto iOp = new ignore_case_op(lhs);
        return iOp;
    } else if (next == 'O') {
        lexer.advance();
        int n = get_number();
        auto output = new output_group_op(n);
        lhs->add(output);
        return lhs;
    } else {
        throw std::runtime_error("Expected I or O");
    }

}

