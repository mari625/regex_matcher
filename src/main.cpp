#include "build_nfa.h"
#include "parser.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <cstdint>
#include <codecvt>
#include <locale>
#include <string>


std::u32string utf8_to_utf32(const std::string& utf8) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    return converter.from_bytes(utf8);
}

int main(int argc, char* argv[]) {
    std::string regex_filename = argv[1];
    std::string text_filename = argv[2];
    std::string out_filename = argv[3];

    RegexParser parser;

    std::ofstream out_file(out_filename);

    std::ifstream file(text_filename);

    std::string line_8b;

    std::vector<NFA> all_nfa;
    std::ifstream regex_file(regex_filename);
    std::string regex_8b;

    size_t counter = 0;
    while (std::getline(regex_file, regex_8b)) {
        std::u32string regex = utf8_to_utf32(regex_8b);
        //++counter;

        std::unique_ptr<ASTNode> ast_node = parser.Parse(regex);

        if (!ast_node) {
            all_nfa.emplace_back(NFA());
            continue;
        }

        NFA nfa = build(std::move(ast_node));
        all_nfa.push_back(std::move(nfa));

        //if (counter > 10) {
        //    break;
        //}
    }
    
    while (getline(file, line_8b)) {
        std::u32string line = utf8_to_utf32(line_8b);
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
