#include <iostream>
#include "Parse.h"

int main() {
    std::string input = "lo* could.{3}";
    std::string text = "Waterloo I was defeated, you won the war Waterloo promise to love you for ever more Waterloo couldn't escape if I wanted to Waterloo knowing my fate is to be with you Waterloo finally facing my Waterloo";
    auto first = input.begin();
    auto last = input.end();
    Parse parse(first, last);
    auto result = parse.match_from_any_pos(text);


    std::cout << "Match: " << result << std::endl;
    return 0;
}
