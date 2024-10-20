//
// Created by Aubrey on 10/19/2024.
//

#include "AST.hpp"

#include <utility>

AST::AST(Operators _ops, const std::string& expression) : ops(std::move(_ops))
{
    // Tokenize
    std::vector<Token> tokens = tokenize_wff(ops, expression);
    // Translate from infix to postfix
    tokens = shunting_yard(ops, tokens);
    // Generate an AST from the tokens
    insert_ast_nodes(ops, root, tokens);
}

AST::AST(const AST& other)
    : ops(other.ops),
      root(deep_copy(other.root))
    {}

AST::~AST()
{
    delete_tree(root);
}

const AST_node* AST::getRoot() const
{
    return root;
}

AST_node* AST::findMutableNode(AST_node* curr, const AST_node* target)
{
    if (curr == target)
    {
        return curr;
    }
    for (AST_node* child : curr->children)
    {
        AST_node* result = findMutableNode(child, target);
        if (result)
        {
            return result;
        }
    }
    return nullptr;
}

bool AST::replaceNode(const AST_node* node, const AST_node* new_node)
{
    AST_node* mut_node = findMutableNode(root, node);
    if (mut_node && new_node)
    {
        for (AST_node* child : mut_node->children)
        {
            delete_tree(child);
        }
        *mut_node = *deep_copy(new_node);
        return true;
    }
    return false;
}

void AST::traverseAndPrint(std::ostream& os, const AST_node* curr) const
{
    // Print unary operators before their operands
    if (curr->token.type == OPERATOR && ops.getProperties(curr->token.lexeme).arity == UNARY)
    {
        os << curr->token.lexeme;
    }

    // Print identifiers (identifiers have no children so this is the end of a tree)
    if (curr->token.type == IDENTIFIER)
    {
        os << curr->token.lexeme;
    }

    // traverse down and print
    bool already_printed = false;
    for (const AST_node* child : curr->children)
    {
        bool opened_paren = false;
        if (curr->token.type == OPERATOR
            && child->token.type == OPERATOR
            && ops.getProperties(child->token.lexeme).arity != UNARY
            && (curr->token.lexeme != child->token.lexeme || ops.getProperties(child->token.lexeme).associativity == NOT_ASSOCIATIVE))
        {
            opened_paren = true;
            os << "(";
        }
        traverseAndPrint(os, child);
        if (opened_paren)
        {
            os << ")";
        }
        if (!already_printed && curr->token.type == OPERATOR && ops.getProperties(curr->token.lexeme).arity == BINARY)
        {
            already_printed = true;
            os << curr->token.lexeme;
        }

    }
}

std::string AST::toString() const
{
    std::stringstream ss;
    traverseAndPrint(ss, root);
    return ss.str();
}

std::vector<Token> AST::tokenize_wff(const Operators& ops, std::string formula)
{
    std::vector<Token> tokens;
    std::string curr;
    for (size_t i=0; i<formula.size(); i++)
    {
        curr = formula[i];

        // Ignore spaces
        if (curr == " ")
        {
            continue;
        }

        // Handle the multi-char operators (e.g. the implies operator which is =>)
        if (ops.matchesOperator(curr) == PARTIAL_MATCH)
        {
            while (i<formula.size()-1 && ops.matchesOperator(curr) == PARTIAL_MATCH)
            {
                curr += formula[i+1];
                i++;
            }
        }

        /*
        if (curr == "=" && (i < (formula.size() - 1)))
        {
            if (formula[i+1] == '>')
            {
                curr += formula[i+1];
                i++; // Skip the next char
            }
        }
        */

        /*
        if (find(unary_operators, curr) != -1) // If curr is a unary operator
        {
            tokens.push_back(Token{UNARY_OPERATOR, curr});
        }
        else if (find(binary_operators, curr) != -1) // If curr is a binary operator
        {
            tokens.push_back(Token{BINARY_OPERATOR, curr});
        }
         */
        if (ops.matchesOperator(curr) == TRUE)
        {
            tokens.push_back({OPERATOR, curr});
        }
        else if (curr == "(")
        {
            tokens.push_back({OPEN_PAREN, curr});
        }
        else if (curr == ")")
        {
            tokens.push_back({CLOSE_PAREN, curr});
        }
        else // curr is an identifier
        {
            tokens.push_back({IDENTIFIER, curr});
        }
    }
    return tokens;
}

std::vector<Token> AST::shunting_yard(const Operators& ops, const std::vector<Token>& tokens)
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

            if (!token_stack.empty()
                && ops.matchesOperator(token_stack.top().lexeme) == TRUE
                && ops.getProperties(token_stack.top().lexeme).arity == UNARY)
            {
                postfix.emplace_back(token_stack.top());
                token_stack.pop();
            }
        }
        else if (ops.matchesOperator(tokens[i].lexeme) == TRUE)
        {
            // While there is an operator on the stack AND that operator has higher-or-equal precedence than the current one
            while (!token_stack.empty()
                   && ops.matchesOperator(token_stack.top().lexeme) == TRUE
                   && ops.getProperties(token_stack.top().lexeme).arity != UNARY
                   && ops.hasHigherOrEqualPrecedence(token_stack.top().lexeme, tokens[i].lexeme))
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

            if (!token_stack.empty()
                && ops.matchesOperator(token_stack.top().lexeme) == TRUE
                && ops.getProperties(token_stack.top().lexeme).arity == UNARY)
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

void AST::insert_ast_nodes(const Operators& ops, AST_node*& root, const std::vector<Token>& tokens)
{
    std::stack<AST_node*> node_stack;

    for (const Token& token : tokens)
    {
        AST_node* new_node = new AST_node(token);

        if (ops.matchesOperator(token.lexeme) == TRUE)
        {
            int num_children = ops.getNumOperands(token.lexeme);
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

/*
bool AST::containsNode(const AST_node* start_node, const AST_node* target) const {
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
 */

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

