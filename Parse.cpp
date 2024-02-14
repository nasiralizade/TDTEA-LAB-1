//
// Created by Nasir Alizade on 2024-02-06.
//
#include "Parse.h"

/**
 * expr   ::= <Subexpression> '+' <Subexpression> | <Subexpression>
 * @return
 */
op *Parse::parse_expr() {
    auto lhs = parse_sub_expr();
    if (lexer.get_current().type == OR_OP) {
        lexer.advance();
        auto orOp = new or_op();
        orOp->add(lhs);
        auto rhs = parse_sub_expr();
        if (!rhs) {
            throw std::runtime_error("Expected subexpression");
        }
        orOp->add(rhs);
        lhs = orOp;
    }
    return lhs;
}

//Subexpression ::= element
// | element Modifier
// | element Modifier Subexpression
// | element Subexpression
op *Parse::parse_sub_expr() {
    auto lhs = parse_element();
    if (lhs) {
        while (true) {
            auto modif = parse_operation(lhs);
            if (modif) {
                lhs = modif;  // Apply modifier to the left-hand side

            } else if (lexer.get_current().type != END && lexer.get_current().type != R_PAR &&
                       lexer.get_current().type != OR_OP) {
                auto subExpr = parse_sub_expr();
                if (subExpr) {
                    auto lhs1 = new Subexpression();
                    lhs1->add(lhs);
                    lhs1->add(subExpr);
                    lhs = lhs1;
                    return lhs;
                }
            } else {
                break;
            }
        }
        return lhs;
    }

    return nullptr;
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
        auto group = new capture_group_op();
        group->add(expr);
        return group;
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
        case L_BRACKET: {
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
        auto iOp = new ignore_case_op();
        iOp->add(lhs);
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

bool Parse::match_from_any_pos(std::string &input) {

    char *first = &input[0];
    char *last = &input[input.size()];
    auto expr = parse_expr();
    print_tree(expr);
    while (first != last) {
        if (expr->eval(first, last)) {
            std::cout << "Matched: " << std::string(&input[0], first) << std::endl;
            return true;
        }
        first++;
    }
    return false;
}

