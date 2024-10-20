//
// Created by Aubrey on 10/19/2024.
//

#include "AST.hpp"

AST::AST(const std::string& expression)
{
    // Tokenize
    std::vector<Token> tokens = tokenize_wff(expression);

    // Translate from infix to postfix
    tokens = shunting_yard(tokens);

    // Generate an AST from the tokens
    insert_ast_nodes(root, tokens);
}

AST::AST(const AST& other)
{
    root = deep_copy(other.getRoot());
}

AST::~AST()
{
    delete_tree(root);
}

const AST_node* AST::getRoot() const
{
    return root;
}

bool AST::replaceNode(AST_node* node, const AST_node& new_node)
{
    if (containsNode(this->root, node))
    {
        *node = new_node;
        return true;
    }
    return false;
}

std::vector<Token> AST::tokenize_wff(std::string formula)
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

std::vector<Token> AST::shunting_yard(const std::vector<Token>& tokens)
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
        else if (is_operator(tokens[i]))
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

void AST::insert_ast_nodes(AST_node*& root, const std::vector<Token>& tokens)
{
    std::stack<AST_node*> node_stack;

    for (const Token& token : tokens)
    {
        AST_node* new_node = new AST_node(token);

        if (is_operator(token))
        {
            int num_children = calc_num_children(token);
            new_node->children.resize(num_children, nullptr);

            for (int i=num_children-1; i>=0; i--)
            {
                new_node->children[i] = node_stack.top();
                node_stack.pop();
            }
        }

        node_stack.push(new_node);
    }

    root = node_stack.top();
    node_stack.pop();
}

void AST::delete_tree(AST_node* node)
{
    if (node)
    {
        for (AST_node* child : node->children) {
            delete_tree(child);
        }
        delete node;
    }
}

bool AST::containsNode(AST_node* start_node, AST_node* target) const {
    if (!start_node) {
        return false;
    }

    if (start_node == target) {
        return true;
    }

    for (AST_node *child: start_node->children)
    {
        if (containsNode(child, target))
        {
            return true;
        }
    }

    return false;
}

AST_node* deep_copy(const AST_node* root)
{
    if (!root)
    {
        return nullptr;
    }

    AST_node* new_root = new AST_node(root->token);

    for (const AST_node* child : root->children)
    {
        new_root->children.push_back(deep_copy(child));
    }

    return new_root;
}

bool is_equal(const AST_node* a, const AST_node* b)
{
    if (a->token.lexeme != b->token.lexeme)
    {
        return false;
    }

    // if they are the same, then they must have the same number of children
    for (int i=0; i<a->children.size(); i++)
    {
        bool equal_children = is_equal(a->children[i],b->children[i]);
        if (!equal_children)
        {
            return false;
        }
    }

    return true;
}