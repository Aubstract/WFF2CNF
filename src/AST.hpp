//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_AST_HPP
#define WFF2CNF_AST_HPP

#include "operators.hpp"
#include "Token.hpp"
#include <stack>
#include <string>
#include <vector>

struct AST_node
{
    Token token;
    std::vector<AST_node*> children; // Variable number of children in order to deal w/ binary operators,
                                     // unary operators, and identifiers (which have no children).
                                     // Basically a node can have [0-2] children.
    AST_node() = default;
    AST_node(const Token& _token) : token(_token) {}
};

class AST
{
private:
    AST_node* root = nullptr;

    std::vector<Token> tokenize_wff(std::string);
    std::vector<Token> shunting_yard(const std::vector<Token>&);
    void insert_ast_nodes(AST_node*&, const std::vector<Token>&);
    void delete_tree(AST_node*);
    bool containsNode(AST_node*, AST_node*) const;

public:
    AST(const std::string&);
    AST(const AST&);
    ~AST();

    const AST_node* getRoot() const;
    bool replaceNode(AST_node*, const AST_node&);
    std::string toString() const;
};

AST_node* deep_copy(const AST_node*);
bool is_equal(const AST_node* , const AST_node*);

#endif //WFF2CNF_AST_HPP
