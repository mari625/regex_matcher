#include AST.h

#include <string>
#include <memory>
#include <unordered_set>
#include <cctype>


class RegexParser {
private:
    std::string_view input;
    size_t pos = 0;
    unordered_set<char> important_symbols = {'(', ')', '*', '|'};

    std::unique_ptr<ASTNode> parse_or() {
        std::unique_ptr<ASTNode> left = parse_and();

        while (pos < input.size() && input[pos] == '|') {
            ++pos;

            std::unique_ptr<ASTNode> right = parse_and();

            left = std::make_unique<ASTNode>(Type::Or, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<ASTNode> parse_and() {
        std::unique_ptr<ASTNode> left;

        while (pos < input.size() && input[pos] != '|' && input[pos] != ')') {
            std::unique_ptr<ASTNode> right = parse_star();

            if (!left) {
                left = std::move(right);
            }  else {
                left = std::make_unique<ASTNode>(Type::And, std::move(left), std::move(right));
            }    
        }

        return left;
    }

    std::unique_ptr<ASTNode> parse_star() {
        std::unique_ptr<ASTNode> left = parse_element();

        if (pos < input.size() && input[pos] == '*') {
            ++pos;

            return std::make_unique<ASTNode>(Type::Star, std::move(left));
        }

        return left;
    }

    std::unique_ptr<ASTNode> parse_element() {
        char symb = input[pos];

        if (symb == '(') {
            ++pos;
            std::unique_ptr<ASTNode> left = parse_or();

            if (pos < input.size() && input[pos] == ')') {
                ++pos;
                return left;
            }
        }

        if (symb == '\\') {
            ++pos;
            symb = input[pos];
            ++pos;
            return std::make_unique<ASTNode>(Type::Char, symb);
        }

        if (important_symbols.find(symb) == important_symbols.end()) {
            ++pos;
            return std::make_unique<ASTNode>(Type::Char, symb);
        }

        return nullptr;
    }

public:

    std::unique_ptr<ASTNode> Parse(std::string& s) {
        input = s;

        std::unique_ptr<ASTNode> result = parse_or();

        return result;
    }
}