#include <iostream>
#include "Parse.h"


int main() {
    std::string input = "abc";
    auto first = input.begin();
    auto last = input.end();
    Parse parse(first, last);
    auto word = parse.parse_word();
    std::cout << word->id() << std::endl;

    return 0;
}
