//
// Created by Nasir Alizade on 2024-02-06.
//

#ifndef LABB1_V4_PARSE_H
#define LABB1_V4_PARSE_H

#include "Ops.h"
#include "lexer.h"
#include <iostream>

using IT = std::string::iterator;

class Parse {
public:
  /**
   * Constructor for Parse class that takes two iterators as input and creates a
   * lexer object
   * @param first  iterator to the first element of the string
   * @param last   iterator to the last element of the string
   */
  Parse(IT first, IT last) : lexer(first, last) {}
  ~Parse() = default;
  /**
   * expr::= <Subexpression> ('+' <Subexpression>)*
   * @return
   */
  op *parse_expr();
  /**
   * Subexpression  ::= Element (operation Element)*
   * @return
   */
  op *parse_subexpression();
  /**
   * element   ::= component | exact_op
   * @return
   */
  op *parse_element();
  /**
   * operation   ::= <Operation> <Element>
   * @param lhs  left hand side of the operation
   * @return
   */
  op *parse_operation(op *lhs);
  /**
   * component   ::= component ::= '(' expr ')' | ANY_CHAR | char | ...
   * @return
   */
  op *parse_component();
  /**
   *
   * @param c  character to be checked
   * @return  true if the character is valid, false otherwise
   */
  bool is_valid_char(char c);
  /**
   *  get_number   ::= <number>
   * @return  the number that is matched
   */
  int get_number();
  /**
   * match_from_any_pos   ::= <input>
   * @param input  the string to be matched
   * @return  true if the string is matched, false otherwise
   */

  bool match_from_any_pos(std::string &input);

private:
  Lexer lexer;
  std::string matchedSubstring;
};

#endif // LABB1_V4_PARSE_H
