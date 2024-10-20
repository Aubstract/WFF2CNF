//
// Created by Aubrey on 10/18/2024.
//

#include "AST.hpp"
#include "Operators.hpp"
#include "Transformer.hpp"
#include <chrono>
#include <iostream>
#include <string>

int main() {
    Operators ops = {
            {"!", {3, NOT_ASSOCIATIVE, UNARY}},
            {"*", {2, ASSOCIATIVE, BINARY}},
            {"+", {1, ASSOCIATIVE, BINARY}},
            {"=>", {0 , NOT_ASSOCIATIVE, BINARY}}
    };

    Transformer wff2cnf = {ops, {{"a=>b", "!a+b"},           // Implication
                                {"!(a+b)", "!a*!b"},        // De Morgan's Law
                                {"!(a*b)", "!a+!b"},
                                {"a*a", "a"},               // Identity
                                {"a+a", "a"},
                                {"a+(a*b)", "a"},           // Absorption (8 scenarios)
                                {"a+(b*a)", "a"},
                                {"(a*b)+a", "a"},
                                {"(b*a)+a", "a"},
                                {"a*(a+b)", "a"},
                                {"a*(b+a)", "a"},
                                {"(a+b)*a", "a"},
                                {"(b+a)*a", "a"},
                                {"(a+b)*(!b+c)", "a+c"},    // Resolution (8 scenarios)
                                {"(a+b)*(c+!b)", "a+c"},
                                {"(b+a)*(!b+c)", "a+c"},
                                {"(b+a)*(c+!b)", "a+c"},
                                {"(!b+c)*(a+b)", "a+c"},
                                {"(c+!b)*(a+b)", "a+c"},
                                {"(!b+c)*(b+a)", "a+c"},
                                {"(c+!b)*(b+a)", "a+c"},
                                {"(a*b)+c", "(a+c)*(b+c)"}, // Distribution
                                {"c+(a*b)", "(a+c)*(b+c)"},
                                {"!!a", "a"}                // Remove double negation
    }};

    std::string formula;
    std::cout << "Enter a WFF: ";
    std::getline(std::cin, formula);

    auto start_time = std::chrono::high_resolution_clock::now();

    AST wff(ops, formula);
    //AST wff(ops, "(p+!(q*r))=>((p+s)*t)");

    wff2cnf.applyTransformations(wff);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "\nCompleted in: " << elapsed_time.count() << " microseconds";

    std::cout << "\nCNF: " << wff.toString() << std::endl;

    return 0;
}

// Test expressions:
// !(a+b)=>c
// a+b=>c
// a * (b+!c) => !d
// !a * (b+!c) => !d
// (a+(b*c))+d

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
