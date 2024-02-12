//
// Created by Nasir Alizade on 2024-02-06.
//
#include "Parse.h"

/**
 * expr   ::= <Subexpression> ('+' <Subexpression>)*
 * @return
 */
op *Parse::parse_expr() {
  auto lhs = parse_subexpression();
  auto sub_expr = new Subexpression();
  sub_expr->add(lhs);
  while (lexer.get_current().type == OR_OP) {
    lexer.advance();
    auto rhs = parse_subexpression();
    auto or_op_p = new or_op();
    or_op_p->add(sub_expr->children.back());
    sub_expr->children.pop_back();
    or_op_p->add(rhs);
    sub_expr->add(or_op_p);
    lhs = sub_expr;
  }
  return lhs;
}

/**
 * Subexpression  ::= Element (operation Element)*
 * @return
 */
op *Parse::parse_subexpression() {
  auto lhs = parse_element();
  auto sub_expr = new Subexpression();
  sub_expr->add(lhs);
  while (lexer.get_current().type != END && lexer.get_current().type != OR_OP &&
         lexer.get_current().type != R_PAR) {
    auto operation = parse_operation(sub_expr);
    if (operation) {
      sub_expr = dynamic_cast<Subexpression *>(operation);
    } else {
      auto element = parse_element();
      if (element) {
        sub_expr->add(element);
      }
    }
  }
  if (sub_expr->children.size() == 1) {
    auto single = sub_expr->children.front();
    sub_expr->children.clear();
    delete sub_expr;
    return single;
  }
  return sub_expr;
}

/**
 * element   ::= component | component'{number}'
 * @return
 */
op *Parse::parse_element() {
  auto component = parse_component();
  if (lexer.get_current().type == L_BRACKET) {
    int number = get_number();
    auto count_op = new exact_op(number);
    count_op->add(component);
    return count_op;
  }
  return component;
}

/**
 * Operation  ::= <Subexpression>* | <Subexpression> \I |
 * <Subexpression>\O{number}
 * @return
 */
op *Parse::parse_operation(op *lhs) {
  if (lexer.get_current().type == REPEAT) {
    lexer.advance();
    auto repeat_op_p = new repeat();
    repeat_op_p->add(lhs->children.back());
    lhs->children.pop_back();
    lhs->add(repeat_op_p);
    return lhs;
  } else if (lexer.get_current().type == SLASH) {
    lexer.advance();
    char next = lexer.get_current().text[0];
    if (next == 'I') { // ignore case '\I'
      lexer.advance();
      auto ignore_op_p = new ignore_case_op(lhs->children.back());
      ignore_op_p->set_ignore_case(true);
      lhs->children.pop_back();
      lhs->add(ignore_op_p);
    } else if (next == 'O') { // output group '\O{number}'
      lexer.advance();
      int groupNum = get_number();
      auto output_group_op_p = new output_group_op(groupNum);
      output_group_op_p->add(lhs);
      lhs->add(output_group_op_p);
    }
    return lhs;
  }
  return nullptr;
}

/**
 * component ::= '(' expr ')' | ANY_CHAR | char | ...
 * @return
 */
op *Parse::parse_component() {
  auto tk = lexer.get_current();
  if (tk.type == L_PAR) {
    lexer.advance();
    auto expr = parse_expr();
    if (lexer.get_current().type != R_PAR) {
      std::cerr << "missing )\n";
    }
    auto cap = new capture_group_op();
    cap->add(expr);
    lexer.advance();
    return cap;
  } else if (tk.type == ANY_CHAR) {
    lexer.advance();
    return new any_char_op();
  } else if (is_valid_char(tk.text[0])) {
    char c = tk.text[0];
    lexer.advance();
    return new char_op(c);
  }
  return nullptr;
}

bool Parse::is_valid_char(char c) {
  return c != '*' && c != '+' && c != '(' && c != ')' && c != '{' && c != '}' &&
         c != '\\' && c != '.';
}

int Parse::get_number() {
  if (lexer.get_current().type != L_BRACKET) {
    std::cerr << "missing {\n";
    return 0;
  }
  lexer.advance();
  std::string number;
  while (lexer.get_current().type == LETTER) {
    number += lexer.get_current().text;
    lexer.advance();
  }
  if (lexer.get_current().type != R_BRACKET) {
    std::cerr << "missing }\n";
  }
  lexer.advance();
  return std::stoi(number);
}

bool Parse::match_from_any_pos(std::string &input) {
  char *first = &input[0];
  char *last = &input[input.size()];
  auto pattern = parse_expr();

  while (first != last) {
    char *temp = first;
    if (pattern->eval(first, last)) {
      matchedSubstring = std::string(temp, first);
      if (group_index_ > 0) {
        std::cout << capturedGroups[group_index_ - 1] << std::endl;
        return true;
      }
      std::cout << "Matched: " << matchedSubstring << std::endl;
      return true;
    }
    first = temp + 1;
  }
  return false;
}
