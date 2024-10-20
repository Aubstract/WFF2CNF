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
    const std::vector<std::tuple<AST,AST>> transforms;

    static constexpr size_t PRE_TRANSFORM_INDEX = 0;
    static constexpr size_t POST_TRANSFORM_INDEX = 1;

    size_t size() const;
    AST pre_transform_at(size_t) const;
    bool match(const AST_node*, const AST_node*, std::map<std::string,AST_node*>&) const;
    void applyBindings(AST_node*&, const std::map<std::string,AST_node*>&);
    void traverseAndApplyTransformations(AST&, const AST_node*);

public:
    Transformer(const std::vector<std::tuple<AST, AST>>&);

    void applyTransformations(AST&);
};

#endif //WFF2CNF_TRANSFORMER_HPP
