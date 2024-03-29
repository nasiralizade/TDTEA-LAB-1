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

    op *parse_expr();

    op *parse_sub_expr();

    op *parse_element();

    /**
     * operation   ::= <Operation> <Element>
     * @param lhs  left hand side of the operation
     * @return
     */
    op *parse_modifier(op *lhs);

    /**
     *
     * @param c  character to be checked
     * @return  true if the character is valid, false otherwise
     */
    bool is_valid_char(char c);

    int get_number();

    bool match_from_any_pos(std::string &input);

private:
    Lexer lexer;
    std::string matchedSubstring;

    op *handleSlash(op *lhs);

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
};

#endif // LABB1_V4_PARSE_H
