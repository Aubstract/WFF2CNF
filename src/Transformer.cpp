//
// Created by Aubrey on 10/19/2024.
//

#include "Transformer.hpp"

Transformer::Transformer(const Operators& _ops, const std::initializer_list<std::pair<std::string,std::string>>& _transforms)
    : ops(_ops),
      transforms([&_ops, &_transforms]()
      {
            std::vector<std::pair<AST,AST>> temp;
            for (const auto& pair : _transforms)
            {
                temp.emplace_back(AST(_ops, pair.first), AST(_ops, pair.second));
            }
            return temp;
      }()) // lambda function to construct ASTs using the initializer list of string pairs, and use the ASTs to
           // initialize the vector
    {}

void Transformer::applyTransformations(AST& wff)
{
    bool applied_transform = false;
    do
    {
        applied_transform = traverseAndApplyTransformations(wff, wff.getRoot());
    } while (applied_transform);
}

bool Transformer::traverseAndApplyTransformations(AST& wff, const AST_node* curr)
{
    bool applied_transform = false;

    for (const std::pair<AST,AST>& pattern : transforms)
    {
        std::map<std::string,AST_node*> bindings;
        bool match = this->match(curr, pattern.first.getRoot(), bindings);
        if (match)
        {
            AST_node* populated_pattern = deep_copy(pattern.second.getRoot());
            applyBindings(populated_pattern, bindings);
            wff.replaceNode(curr, populated_pattern);
            applied_transform = true;
        }
    }

    // traverse down
    for (const AST_node* child : curr->children)
    {
        if (traverseAndApplyTransformations(wff, child))
        {
            applied_transform = true;
        }
    }

    return applied_transform;
}

bool Transformer::match(const AST_node* wff,
                        const AST_node* pattern,
                        std::map<std::string,AST_node*>& bindings) const
{
    // Match is based on a traversal function, so start at the root and traverse down to the next node:
    //   - If the current node in the pattern is an operator then check if it's the same operator
    //     in the wff. If not, return false.
    //   - Else if the current node in the pattern is an identifier, then check if that identifier is bound to an
    //     identical WFF to the current node. If it is bound to something else, return false. Else if it is not
    //     bound to anything, bind the current wff node to the pattern's identifier.

    if (ops.matchesOperator(pattern->token.lexeme) == TRUE)
    {
        if (wff->token.lexeme != pattern->token.lexeme)
        {
            return false;
        }
    }
    else // pattern node is an identifier
    {
        if (bindings.find(pattern->token.lexeme) != bindings.end()) // if already bound
        {
            if (!is_equal(wff, bindings.at(pattern->token.lexeme))) // if the stored binding is not identical
                // to the current wff
            {
                return false;
            }
        }
        else // unbound
        {
            bindings.insert({pattern->token.lexeme, deep_copy(wff)});
        }
    }

    // traverse down
    for (size_t i=0; i<ops.getNumOperands(pattern->token.lexeme); i++)
    {
        bool children_equal = match(wff->children[i], pattern->children[i], bindings);
        if (!children_equal)
        {
            return false;
        }
    }

    return true;
}

void Transformer::applyBindings(AST_node*& root, const std::map<std::string,AST_node*>& bindings)
{
    if (root->token.type == IDENTIFIER)
    {
        root = deep_copy(bindings.at(root->token.lexeme));
        return;
    }

    for (auto& child : root->children)
    {
        applyBindings(child, bindings);
    }
}