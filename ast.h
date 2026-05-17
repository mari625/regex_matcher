#ifndef AST_H
#define AST_H

#include <memory>

enum Type {
        Char,
        And, 
        Or, 
        Star
    };

struct ASTNode {   
    Type type;
    char val = ' ';
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    ASTNode(Type type, char c) : type(type), val(c) {}

    ASTNode(Type type, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) : type(type) {
        left = std::move(l);
        right = std::move(r);
    }

    ASTNode(Type type, std::unique_ptr<ASTNode> l) : type(type), right(nullptr) {
        left = std::move(l);
    }
};

#endif