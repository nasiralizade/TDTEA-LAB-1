#include <iostream>
#include "Ops.h"
#include "lexer.h"
#include "parsings.h"




int main() {
    // Input string
    std::string input = "ac*";
    std::string x = "accc";
    IT first = input.begin();
    IT last = input.end();
    auto tree = parse_expr(first, last);
    print_tree(tree);

    char *first1 = x.data();
    char *last2 = x.data() + x.size();
    auto result = tree->eval(first1, last2);

    if (result) {
        std::cout << "Parsed successfully" << std::endl;
    } else {
        std::cout << "Parsing failed" << std::endl;
    }


    return 0;
}
