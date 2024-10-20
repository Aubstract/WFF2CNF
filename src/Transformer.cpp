//
// Created by Aubrey on 10/19/2024.
//

#include "Transformer.hpp"

Transformer::Transformer(const std::vector<std::tuple<AST, AST>>& _transforms) : transforms(_transforms) {}

void Transformer::applyTransformations(AST& wff)
{
    traverseAndApplyTransformations(wff, wff.getRoot());
}

void Transformer::traverseAndApplyTransformations(AST& wff, const AST_node* curr)
{
    for (const std::tuple<AST,AST>& pattern : transforms)
    {
        std::map<std::string,AST_node*> bindings;
        bool match = this->match(curr, std::get<PRE_TRANSFORM_INDEX>(pattern).getRoot(), bindings);
        if (match)
        {
            AST_node* populated_pattern = deep_copy(std::get<POST_TRANSFORM_INDEX>(pattern).getRoot());
            applyBindings(populated_pattern, bindings);
            wff.replaceNode(const_cast<AST_node *>(curr), *populated_pattern);
        }
    }

    // traverse down
    for (const AST_node* child : curr->children)
    {
        traverseAndApplyTransformations(wff, child);
    }
}

size_t Transformer::size() const
{
    return transforms.size();
}

AST Transformer::pre_transform_at(size_t index) const
{
    return std::get<PRE_TRANSFORM_INDEX>(transforms.at(index));
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

    if (is_operator(pattern->token))
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
    for (size_t i=0; i< calc_num_children(pattern->token); i++)
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