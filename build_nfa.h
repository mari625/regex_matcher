#include "ast.h"
#include "nfa.h"

#include <memory>

NFA build(std::unique_ptr<ASTNode> node) {
    if (node->type == Char) {
        return NFA(node->val);
    }

    if (node->type == And) {
        NFA left_node = build(std::move(node->left));
        NFA right_node = build(std::move(node->right));

        left_node.build_and(std::move(right_node));

        return left_node;

    } else if (node->type == Or) {
        NFA left_node = build(std::move(node->left));
        NFA right_node = build(std::move(node->right));

        left_node.build_or(std::move(right_node));

        return left_node;

    } else {
        NFA new_node = build(std::move(node->left));
        new_node.build_star();

        return new_node;
    }

    return NFA();
}