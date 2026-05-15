#include <memory>


struct ASTNode {
    enum Type {
        Char,
        And, 
        Or, 
        Star
    };
    
    Type type;
    char val = ' ';
    std::unique_ptr<AST> left;
    std::unique_ptr<AST> right;

    ASTNode(Type type, char c) : type(type), val(c) {}

    ASTNode(Type type, std::unique_ptr<AST> l, std::unique_ptr<AST> r) : type(type) {
        left = std::move(l);
        right = std::move(r);
    }

    ASTNode(Type type, std::unique_ptr<AST> l) : type(type), right(nullptr) {
        left = std::move(l);
    }
};