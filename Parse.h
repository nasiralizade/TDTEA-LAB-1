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
    expr_op* parse_expr();
    or_op* parse_or();
    ignore_case_op* parse_ignore();
    repeat_op* parse_repeat();
    word_op* parse_word();
    op* parse_char();
    capture_group_op* parse_group();
    output_group_op* parse_output_group();
    match_op* parse_match();
    count_op* parse_count();
    bool match_from_any_pos(const std::string &input);



private:
    Lexer lexer;
    std::string matchedSubstring;

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
