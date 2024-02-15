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
            auto modif = parse_modifier(lhs);
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


// number::={digit}+
int Parse::get_number() {
    if (lexer.get_current().type == L_BRACKET) {
        lexer.advance();
        std::string number;
        while (lexer.get_current().type != R_BRACKET) {
            if (isdigit(lexer.get_current().text[0])) {
                number += lexer.get_current().text;
            } else {
                throw std::runtime_error("Expected number");
            }
            lexer.advance();
        }
        if (lexer.get_current().type != R_BRACKET) {
            throw std::runtime_error("Expected }");
        }
        lexer.advance();
        return std::stoi(number);
    }
    throw std::runtime_error("Expected {");
}

// element ::= character | any_char | group
op *Parse::parse_element() {
    auto tk = lexer.get_current().type;
    if (tk == L_PAR) { // group:== '(' expr ')'
        lexer.advance();
        auto expr = parse_expr();
        if (lexer.get_current().type != R_PAR) {
            throw std::runtime_error("Expected )");
        }
        lexer.advance();
        auto group = new capture_group_op();
        group->add(expr);
        return group;
    } else if (tk == ANY_CHAR) { // any_char:== '.'
        lexer.advance();
        return new any_char_op();
    } else if (tk == LETTER) { // character:== <letter>
        auto c = lexer.get_current().text[0];
        lexer.advance();
        return new char_op(c);
    } else {
        return nullptr;
    }
}

//
op *Parse::parse_modifier(op *lhs) {
    switch (lexer.get_current().type) {
        case REPEAT: { // *
            lexer.advance();
            auto repeatOp = new repeat_op();
            repeatOp->add(lhs);
            return repeatOp;
        }
        case L_BRACKET: { // {n}
            auto count = get_number();
            auto countOp = new exact_op(count);
            countOp->add(lhs);
            return countOp;
        }
        case SLASH: {// \I or \O
            lexer.advance();
            return handleSlash(lhs);
        }
        default:
            return nullptr;
    }
}

//
op *Parse::handleSlash(op *lhs) {
    char next = lexer.get_current().text[0];
    if (next == 'I') {
        lexer.advance();
        auto IgnoreCaseOp = new ignore_case_op();
        IgnoreCaseOp->add(lhs);
        return IgnoreCaseOp;
    } else if (next == 'O') {
        lexer.advance();
        int n = get_number();
        auto output = new output_group_op(n);
        group_index_ = n;
        lhs->add(output);
        return lhs;
    } else {
        throw std::runtime_error("Expected I or O");
    }

}

bool Parse::match_from_any_pos(std::string &input) {
    std::string line(80, '-');
    char *first = &input[0];
    char *last = &input[input.size()];
    auto expr = parse_expr();
    if (!expr) {
        throw std::runtime_error("Expected expression");
    }
    std::cout << line << std::endl;
    print_tree(expr);
    while (first != last) {
        char *temp = first;
        if (expr->eval(first, last)) {
            matchedSubstring = std::string(temp, first);
            if (group_index_ > 0) {
                std::cout << line << "\n" << capturedGroups[group_index_ - 1] << std::endl;
                return true;
            }
            std::cout << line << "\nMatched: " << matchedSubstring << std::endl;
            return true;
        }
        first = temp + 1;
    }
    return false;
}

