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

// TODO: Move the function defs into a .cpp

struct AST_node
{
    Token token;
    std::vector<AST_node*> children; // Variable number of children in order to deal w/ binary operators,
    // unary operators, and identifiers (which have no children).
    // Basically a node can have [0-2] children.
};

std::vector<Token> tokenize_wff(std::string formula)
{
    std::vector<Token> tokens;
    std::string curr;
    for (size_t i=0; i<formula.size(); i++)
    {
        curr = formula[i];

        if (curr == " ")
        {
            continue;
        }

        // Handle the two-char operators (e.g. the implies operator which is =>)
        if (curr == "=" && (i < (formula.size() - 1)))
        {
            if (formula[i+1] == '>')
            {
                curr += formula[i+1];
                i++; // Skip the next char
            }
        }

        if (find(unary_operators, curr) != -1) // If curr is a unary operator
        {
            tokens.push_back(Token{UNARY_OPERATOR, curr});
        }
        else if (find(binary_operators, curr) != -1) // If curr is a binary operator
        {
            tokens.push_back(Token{BINARY_OPERATOR, curr});
        }
        else if (curr == "(")
        {
            tokens.push_back(Token{OPEN_PAREN, curr});
        }
        else if (curr == ")")
        {
            tokens.push_back(Token{CLOSE_PAREN, curr});
        }
        else // curr is an identifier
        {
            tokens.push_back(Token{IDENTIFIER, curr});
        }
    }
    return tokens;
}

std::vector<Token> shunting_yard(const std::vector<Token>& tokens)
{
    std::vector<Token> postfix;
    postfix.reserve(tokens.size());
    std::stack<Token> token_stack;
    Token temp;

    for (size_t i=0; i<tokens.size(); i++)
    {
        if (tokens[i].type == IDENTIFIER)
        {
            postfix.emplace_back(tokens[i]); // Put operands right into output vec
            if (!token_stack.empty() && find(unary_operators, token_stack.top().lexeme) != -1)
            {
                postfix.emplace_back(token_stack.top());
                token_stack.pop();
            }
        }
        else if (tokens[i].type == BINARY_OPERATOR || tokens[i].type == UNARY_OPERATOR)
        {
            // While there is an operator on the stack AND that operator has higher-or-equal precedence than the current one
            while (!token_stack.empty()
                   && find(unary_operators, tokens[i].lexeme) == -1
                   && is_operator(token_stack.top())
                   && (compare_precedence(token_stack.top(), tokens[i]) <= 0))
            {
                postfix.emplace_back(token_stack.top());
                token_stack.pop();
            }

            token_stack.push(tokens[i]); // Finally, push the current operator onto the stack
        }
        else if (tokens[i].type == OPEN_PAREN)
        {
            token_stack.push(tokens[i]);
        }
        else // tokens[i].type == CLOSE_PAREN
        {
            while (!token_stack.empty() && token_stack.top().type != OPEN_PAREN)
            {
                postfix.emplace_back(token_stack.top());
                token_stack.pop();
            }
            token_stack.pop(); // Pop the OPEN_PAREN

            if (!token_stack.empty() && find(unary_operators, token_stack.top().lexeme) != -1)
            {
                postfix.emplace_back(token_stack.top());
                token_stack.pop();
            }
        }
    }

    // Pop any remaining operators on the stack
    while (!token_stack.empty())
    {
        postfix.emplace_back(token_stack.top());
        token_stack.pop();
    }

    return postfix;
}

void insert_ast_nodes(AST_node*& root, const std::vector<Token>& tokens, int i)
{
    if (!root)
    {
        root = new AST_node {tokens[i], {nullptr}};
        root->children.resize(calc_num_children(tokens[i]), nullptr);
    }

    for (int j=1; j<=calc_num_children(tokens[i]); j++)
    {
        insert_ast_nodes(root->children[j-1], tokens, i-j);
    }
}

bool equal(AST_node*& a, AST_node*&b)
{
    if (a->token.lexeme != b->token.lexeme)
    {
        return false;
    }

    // if they are the same, then they must have the same number of children
    for (int i=0; i<a->children.size(); i++)
    {
        bool equal_children = equal(a->children[i],b->children[i]);
        if (!equal_children)
        {
            return false;
        }
    }

    return true;
}

struct AST
{
    AST_node* root = nullptr;

    AST(const std::string& expression)
    {
        // Tokenize
        std::vector<Token> tokens = tokenize_wff(expression);

        // Translate from infix to postfix
        tokens = shunting_yard(tokens);

        // Generate an AST from the tokens
        insert_ast_nodes(root, tokens, tokens.size() - 1);
    }
};

bool equal(AST a, AST b)
{
    return equal(a.root, b.root);
}

#endif //WFF2CNF_AST_HPP
