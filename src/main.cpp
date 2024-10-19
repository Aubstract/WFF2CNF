//
// Created by Aubrey on 10/18/2024.
//

#include "AST.hpp"
#include "Transformer.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

void translate_wff_to_cnf(std::string& expression, const Transformer& zeta)
{
    // Build and AST to represent the WFF
    AST expr = expression;

    // Apply transformations to WFF AST
    for (size_t i=0; i<zeta.size(); i++)
    {

    }
}

int main() {
    // Test expressions:
    // !(a+b)=>c
    // a+b=>c
    // a * (b+!c) => !d
    // !a * (b+!c) => !d
    // (a+(b*c))+d

    // The table of transformations has to include all re-orderings of each rule in order to simplify the applications
    // of the transformations. Eventually maybe I'll make it smarter, so I don't have to do this
    Transformer transformer = std::vector<std::tuple<AST, AST>> {
                {AST("a=>b"), AST("!a+b")}, // Implication
                {AST("a*1"), AST("a")}, // Identities of Operators
                {AST("a+0"), AST("a")},
                {AST("a*0"), AST("0")},
                {AST("a+1"), AST("1")},
                {AST("a+!a"), AST("1")},    // Complement
                {AST("a*!a"), AST("0")},
                {AST("!(a+b)"), AST("!a*!b")}, // De Morgan's Law
                {AST("!(a*b)"), AST("!a+!b")},
                {AST("a*a"), AST("a")}, // Identity
                {AST("a+a"), AST("a")},
                {AST("a+(a*b)"), AST("a")}, // Absorption (8 scenarios)
                {AST("a+(b*a)"), AST("a")},
                {AST("(a*b)+a"), AST("a")},
                {AST("(b*a)+a"), AST("a")},
                {AST("a*(a+b)"), AST("a")},
                {AST("a*(b+a)"), AST("a")},
                {AST("(a+b)*a"), AST("a")},
                {AST("(b+a)*a"), AST("a")},
                {AST("(a+b)*(!b+c)"), AST("a+c")}, // Resolution (8 scenarios)
                {AST("(a+b)*(c+!b)"), AST("a+c")},
                {AST("(b+a)*(!b+c)"), AST("a+c")},
                {AST("(b+a)*(c+!b)"), AST("a+c")},
                {AST("(!b+c)*(a+b)"), AST("a+c")},
                {AST("(c+!b)*(a+b)"), AST("a+c")},
                {AST("(!b+c)*(b+a)"), AST("a+c")},
                {AST("(c+!b)*(b+a)"), AST("a+c")},
    };

    // std::string formula;
    //std::cout << "Enter a WFF: ";
    //std::getline(std::cin, formula);

    AST wff("((!p+s)+r)=>s");
    AST pattern("(a+b)=>!c");
    std::map<std::string, AST_node*> bindings;

    std::cout << transformer.match(wff.root, pattern.root, bindings);

    //translate_wff_to_cnf(formula, zeta);

    // std::cout << "\nCNF: " << formula << std::endl;

    return 0;
}
