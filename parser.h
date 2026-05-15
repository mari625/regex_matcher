#include AST.h

#include <string>
#include <memory>
#include <unordered_set>
#include <cctype>


class RegexParser {
private:
    std::string_view input;
    size_t pos = 0;
    unordered_set<char> important_symbols = unordered_set('(', ')', '*', '|');

    std::unique_ptr<ASTNode> parse_expr() {

        while (pos < input.size()) {
            std::unique_ptr<ASTNode> left_node;

            if (input[pos] == '(') {
                ++pos;
                left_node = parse_expr();
            } else if (important_symbols.find(input[pos]) == important_symbols.end()) {
                left_node = std::make_unique<ASTNode>(Type::Char, input[pos]);
                ++pos;
            }

            std::unique_ptr<ASTNode> right_node;

            if (input[pos] == '*') {
                ++pos;
                right_node = std::make_unique<ASTNode>(Type::Star, left_node);
            }

            if (input[pos] == '|') {
                ++pos;
                
                if (input[pos] == '(') {
                    ++pos;
                    right_node = parse_expr();
                } else if (isalpha(input[pos])) {
                    right_node = std::make_unique<ASTNode>(Type::Char, input[pos]);
                }

                std::make_unique<ASTNode>(Type::Or, left_node, right_node);
            } else {
                if (input[pos] == '\\') {
                    ++pos;

                    right_node = std::make_unique<ASTNode>(Type::Char, input[pos]);
                } else if (input[pos] == '(') {
                    ++pos;

                    right_node = parse_expr();
                } else {
                    right_node = std::make_unique<ASTNode>(Type::Char, input[pos]);
                }   
            }
                
            right_node = std::move(left_node);
            }


            if (input[pos] == ')') {
                ++pos;
                return right_node;
            }

            ++pos;
        }

        return;
    }


public:


    std::unique_ptr<AST> Parse(std::string& s) {
        input = s;

        result = 
    }
}