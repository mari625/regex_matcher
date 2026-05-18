#include "build_nfa.h"
#include "parser.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>


int main(int argc, char* argv[]) {
    std::string regex_filename = argv[1];
    std::string text_filename = argv[2];
    std::string out_filename = argv[3];

    RegexParser parser;

    std::ofstream out_file(out_filename);

    std::ifstream file(text_filename, std::ios::binary);

    std::string line;

    std::vector<NFA> all_nfa;
    std::ifstream regex_file(regex_filename);
    std::string regex;

    size_t counter = 0;
    while (std::getline(regex_file, regex)) {
        ++counter;

        std::unique_ptr<ASTNode> ast_node = parser.Parse(regex);

        if (!ast_node) {
            all_nfa.emplace_back(NFA());
            continue;
        }

        NFA nfa = build(std::move(ast_node));
        all_nfa.push_back(std::move(nfa));

        /*if (counter > 10) {
            break;
        }*/
    }
    
    while (getline(file, line)) {
        std::vector<int64_t> result;

        for (NFA& nfa: all_nfa) {
            if (nfa.get_states().empty()) {
                result.push_back(-1);
            } else {
                result.push_back(nfa.count_matches(line));
            }
        }

        for (size_t i = 0; i < result.size(); ++i) {
            out_file << result[i] << " ";
        }
        out_file << '\n';
    }

    out_file.close(); 

    return 0;
}
