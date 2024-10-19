//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_TRANSFORMER_HPP
#define WFF2CNF_TRANSFORMER_HPP

#include "AST.hpp"
#include <map>
#include <string>
#include <tuple>
#include <vector>

// Transformations is a container class that allows you to store key-value pairs, but also allows you to read a key at
// a particular index. This is accomplished through the usage of a vector of tuples.
//
// It is constructed using tuples where the left side of the tuple is the key (pre-transform pattern), and the right
// side is the value (post-transform pattern).
//
// Every tuple represents a WFF AST and another AST that it can transform into.
// For example a=>b can transform into !a+b, so there should be a tuple that looks like this (a=>b, !a+b).
class Transformer
{
private:
    std::vector<std::tuple<AST,AST>> transforms;

    static constexpr size_t PRE_TRANSFORM_INDEX = 0;
    static constexpr size_t POST_TRANSFORM_INDEX = 1;

public:
    Transformer(const std::vector<std::tuple<AST, AST>>& _transforms)
    {
        transforms = _transforms;
    }

    size_t size() const
    {
        return transforms.size();
    }

    AST pre_transform_at(size_t index) const
    {
        return std::get<PRE_TRANSFORM_INDEX>(transforms.at(index));
    }

    bool match(AST_node*& wff, AST_node*& pattern, std::map<std::string, AST_node*>& bindings)
    {
        // Match is based on a traversal function, so start at the root and traverse down to the next node:
        //   - If the current node in the pattern is an operator then check if it's the same operator
        //     in the wff. If not, return false.
        //   - Else if the current node in the pattern is an identifier, then check if that identifier is bound to an
        //     identical WFF to the current node. If it is bound to something else, return false. Else if it is not
        //     bound to anything, bind the current wff node to the pattern's identifier.

        if (pattern->token.type == UNARY_OPERATOR || pattern->token.type == BINARY_OPERATOR)
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
                if (!equal(wff, bindings.at(pattern->token.lexeme))) // if the stored binding is not identical
                                                                             // to the current wff
                {
                    return false;
                }
            }
            else // unbound
            {
                bindings.insert({pattern->token.lexeme, wff});
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

    void apply_transform(AST_node*& wff, AST_node*& pattern, std::map<std::string, AST_node*>& bindings)
    {

    }
};

#endif //WFF2CNF_TRANSFORMER_HPP
