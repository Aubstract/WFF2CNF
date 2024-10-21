//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_TRANSFORMER_HPP
#define WFF2CNF_TRANSFORMER_HPP

#include "AST.hpp"
#include "Operators.hpp"
#include "Symbols.hpp"
#include <map>
#include <string>
#include <utility>
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
    const Symbols symbols;
    const std::vector<std::pair<AST,AST>> transforms;
    const Operators ops;

    bool match(const AST_node*, const AST_node*, std::map<std::string,AST_node*>&) const;
    static void applyBindings(AST_node*&, const std::map<std::string,AST_node*>&);
    bool traverseAndApplyTransformations(AST&, const AST_node*);

public:
    Transformer(const Symbols&, const Operators&, const std::initializer_list<std::pair<std::string,std::string>>&);

    void applyTransformations(AST&);
};

#endif //WFF2CNF_TRANSFORMER_HPP
