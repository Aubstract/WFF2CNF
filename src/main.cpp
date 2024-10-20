//
// Created by Aubrey on 10/18/2024.
//

#include "AST.hpp"
#include "Transformer.hpp"
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

int main() {
    // Test expressions:
    // !(a+b)=>c
    // a+b=>c
    // a * (b+!c) => !d
    // !a * (b+!c) => !d
    // (a+(b*c))+d

    // The table of transformations has to include all re-orderings of each rule in order to simplify the applications
    // of the transformations. Eventually maybe I'll make it smarter, so I don't have to do this
    Transformer wff2cnf = std::vector<std::tuple<AST, AST>> {
        {AST("a=>b"), AST("!a+b")},           // Implication
        //{AST("a*1"), AST("a")},               // Identities of Operators
        //{AST("1*a"), AST("a")},
        //{AST("a+0"), AST("a")},
        //{AST("0+a"), AST("a")},
        //{AST("a*0"), AST("0")},
        //{AST("0*a"), AST("0")},
        //{AST("a+1"), AST("1")},
        //{AST("1+a"), AST("1")},
        //{AST("a+!a"), AST("1")},              // Complement
        //{AST("!a+a"), AST("1")},
        //{AST("a*!a"), AST("0")},
        //{AST("!a*a"), AST("0")},
        {AST("!(a+b)"), AST("!a*!b")},        // De Morgan's Law
        {AST("!(a*b)"), AST("!a+!b")},
        {AST("a*a"), AST("a")},               // Identity
        {AST("a+a"), AST("a")},
        {AST("a+(a*b)"), AST("a")},           // Absorption (8 scenarios)
        {AST("a+(b*a)"), AST("a")},
        {AST("(a*b)+a"), AST("a")},
        {AST("(b*a)+a"), AST("a")},
        {AST("a*(a+b)"), AST("a")},
        {AST("a*(b+a)"), AST("a")},
        {AST("(a+b)*a"), AST("a")},
        {AST("(b+a)*a"), AST("a")},
        {AST("(a+b)*(!b+c)"), AST("a+c")},    // Resolution (8 scenarios)
        {AST("(a+b)*(c+!b)"), AST("a+c")},
        {AST("(b+a)*(!b+c)"), AST("a+c")},
        {AST("(b+a)*(c+!b)"), AST("a+c")},
        {AST("(!b+c)*(a+b)"), AST("a+c")},
        {AST("(c+!b)*(a+b)"), AST("a+c")},
        {AST("(!b+c)*(b+a)"), AST("a+c")},
        {AST("(c+!b)*(b+a)"), AST("a+c")},
        {AST("(a*b)+c"), AST("(a+c)*(b+c)")}, // Distribution
        {AST("c+(a*b)"), AST("(a+c)*(b+c)")}
    };

    // std::string formula;
    // std::cout << "Enter a WFF: ";
    // std::getline(std::cin, formula);

    AST wff("!(a+b)");
    wff2cnf.applyTransformations(wff);

    //std::cout << "\nCNF: " << wff.toString() << std::endl;

    return 0;
}
