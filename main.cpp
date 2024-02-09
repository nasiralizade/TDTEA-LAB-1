#include <fstream>
#include <iostream>

#include "Parse.h"

int group_index_ = 0;
std::vector<std::string> capturedGroups;

int main(int argc, char *argv[]) {
  std::string pattern;
  std::string input;
  if (argc < 3) {
    std::cout << "Using default pattern and input" << std::endl;
    pattern = "promise to (Love+Hate)\\I you\\O{1}";
    input =
        "Waterloo I was defeated, you won the war Waterloo promise to love you "
        "for ever more Waterloo couldn't escape if I wanted to Waterloo "
        "knowing my fate is to be with you Waterloo finally facing my Waterloo";
  } else {
    pattern = argv[1];
    input = argv[2];
  }

  Parse parse(pattern.begin(), pattern.end());
  auto result = parse.match_from_any_pos(input);

  std::cout << "Match: " << result << std::endl;
    return 0;
}
