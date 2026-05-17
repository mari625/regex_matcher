#include "ast.h"

#include <string>
#include <memory>
#include <unordered_set>
#include <cctype>
#include <iostream>


class RegexParser {
private:
    std::string_view input;
    size_t pos = 0;
    std::unordered_set<char> important_symbols = {'(', ')', '*', '|'};

    std::unique_ptr<ASTNode> parse_or() {
        std::unique_ptr<ASTNode> left = parse_and();

        while (pos < input.size() && input[pos] == '|') {
            ++pos;

            std::unique_ptr<ASTNode> right;

            size_t right_start = pos;

            try{
                right = parse_and();
            }  catch (const std::exception& e) {
                throw e;
            }

            if (!right || (pos == right_start && !right)) {
                throw std::runtime_error("Error parsing");
            }

            left = std::make_unique<ASTNode>(Type::Or, std::move(left), std::move(right));
        }

        return left;
    }

    std::unique_ptr<ASTNode> parse_and() {
        std::unique_ptr<ASTNode> left;

        while (pos < input.size() && input[pos] != '|' && input[pos] != ')') {
            std::unique_ptr<ASTNode> right;

            try {
                right = parse_star();
            } catch (const std::exception& e) {
                throw e;
            }

            if (!left) {
                left = std::move(right);
            }  else {
                left = std::make_unique<ASTNode>(Type::And, std::move(left), std::move(right));
            }    
        }

        return left;
    }

    std::unique_ptr<ASTNode> parse_star() {
        std::unique_ptr<ASTNode> left;

        try {
            left = parse_element();
        } catch (const std::exception& e) {
            throw e;
        }

        if (pos < input.size() && input[pos] == '*') {
            ++pos;

            return std::make_unique<ASTNode>(Type::Star, std::move(left));
        }

        return left;
    }

    std::unique_ptr<ASTNode> parse_element() {
        if (pos >= input.size()) {
            throw std::runtime_error("Error parsing");
        }

        char symb = input[pos];

        if (symb == '(') {
            ++pos;
            std::unique_ptr<ASTNode> left;

            try {
                left = parse_or();
            } catch (const std::exception& e) {
                throw e;
            }

            if (pos < input.size() && input[pos] == ')') {
                ++pos;
                return left;
            } else {
                throw std::runtime_error("Error parsing");
            }
        }

        if (symb == '\\') {
            ++pos;
            if (pos >= input.size()) {
                throw std::runtime_error("Error parsing");
            }

            symb = input[pos];
            ++pos;
            return std::make_unique<ASTNode>(Type::Char, symb);
        }

        if (important_symbols.find(symb) == important_symbols.end()) {
            ++pos;
            return std::make_unique<ASTNode>(Type::Char, symb);
        }

        throw std::runtime_error("Error parsing");
    }

public:

    RegexParser() = default;

    std::unique_ptr<ASTNode> Parse(std::string& s) {
        input = s;
        pos = 0;

        try {
            std::unique_ptr<ASTNode> result = parse_or();

            if (pos != input.size()) {
                std::cout << "Parsing error\n";
                return nullptr;
            }

            return result;
        } catch (const std::exception& e) {
            return nullptr;
        }
    }
};
