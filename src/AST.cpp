//
// Created by Aubrey on 10/19/2024.
//

#include "AST.hpp"

#include <utility>

AST::AST(Symbols _symbols, Operators _ops, const std::string& expression) 
    : ops(std::move(_ops)),
      symbols(std::move(_symbols))
{
    std::vector<Token> tokens = tokenizeWff(expression); // Tokenize
    tokens = shuntingYard(tokens); // Translate from infix to postfix
    insertNodes(root, tokens); // Generate an AST from the tokens
}

AST::AST(const AST& other)
    : root(deep_copy(other.root)),
      symbols(other.symbols),
      ops(other.ops)
    {}

AST::~AST()
{
    deleteTree(root);
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
            deleteTree(child);
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
    if (curr->token.type == VARIABLE || curr->token.type == CONSTANT)
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
            && (curr->token.lexeme != child->token.lexeme
                || ops.getProperties(child->token.lexeme).associativity == NOT_ASSOCIATIVE))
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

std::vector<Token> AST::tokenizeWff(const std::string& formula) const
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
        if (ops.matchesOperator(curr) == MATCH_PARTIAL)
        {
            while (i<formula.size()-1 && ops.matchesOperator(curr) == MATCH_PARTIAL)
            {
                curr += formula[i+1];
                i++;
            }
        }

        if (ops.matchesOperator(curr) == MATCH_TRUE)
        {
            tokens.emplace_back(OPERATOR, curr);
        }
        else if (curr == "(")
        {
            tokens.emplace_back(OPEN_PAREN, curr);
        }
        else if (curr == ")")
        {
            tokens.emplace_back(CLOSE_PAREN, curr);
        }
        else if (symbols.isVariable(curr))
        {
            tokens.emplace_back(VARIABLE, curr);
        }
        else if (symbols.isConstant(curr))
        {
            tokens.emplace_back(CONSTANT, curr);
        }
        else
        {
            throw std::runtime_error("Unknown token '" + curr + "' found in AST.tokenize()");
        }
    }
    return tokens;
}

std::vector<Token> AST::shuntingYard(const std::vector<Token>& tokens) const
{
    std::vector<Token> postfix;
    postfix.reserve(tokens.size());
    std::stack<Token> token_stack;

    for (size_t i=0; i<tokens.size(); i++)
    {
        if (tokens[i].type == VARIABLE || tokens[i].type == CONSTANT)
        {
            postfix.emplace_back(tokens[i]); // Put operands right into output vec

            if (!token_stack.empty()
                && ops.matchesOperator(token_stack.top().lexeme) == MATCH_TRUE
                && ops.getProperties(token_stack.top().lexeme).arity == UNARY)
            {
                postfix.emplace_back(token_stack.top());
                token_stack.pop();
            }
        }
        else if (ops.matchesOperator(tokens[i].lexeme) == MATCH_TRUE)
        {
            // While there is an operator on the stack AND that operator has higher-or-equal precedence than the current one
            while (!token_stack.empty()
                   && ops.matchesOperator(token_stack.top().lexeme) == MATCH_TRUE
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
                && ops.matchesOperator(token_stack.top().lexeme) == MATCH_TRUE
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

void AST::insertNodes(AST_node*& curr, const std::vector<Token>& tokens) const
{
    std::stack<AST_node*> node_stack;

    for (const Token& token : tokens)
    {
        AST_node* new_node = new AST_node(token);

        if (ops.matchesOperator(token.lexeme) == MATCH_TRUE)
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

    curr = node_stack.top();
    node_stack.pop();
}

void AST::deleteTree(AST_node* curr)
{
    if (curr)
    {
        for (AST_node* child : curr->children) {
            deleteTree(child);
        }
        delete curr;
    }
}

AST_node* deep_copy(const AST_node* curr)
{
    if (!curr)
    {
        return nullptr;
    }

    AST_node* new_root = new AST_node(curr->token);

    for (const AST_node* child : curr->children)
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

