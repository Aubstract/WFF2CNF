//
// Created by Aubrey on 10/18/2024.
//

#include "AST.hpp"
#include "Operators.hpp"
#include "Symbols.hpp"
#include "Transformer.hpp"
#include <chrono>
#include <iostream>
#include <string>

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    const Symbols symbols =
    {
        {
            {"1", CONST_TRUE}, // Constants
            {"0", CONST_FALSE}
        },
        {
            {"a"}, // Variables
            {"b"},
            {"c"},
            {"p"},
            {"q"},
            {"r"},
            {"s"},
            {"t"}
        }
    };

    const Operators ops = {
            {"!", {3, NOT_ASSOCIATIVE, UNARY}},
            {"*", {2, ASSOCIATIVE, BINARY}},
            {"+", {1, ASSOCIATIVE, BINARY}},
            {"=>", {0 , NOT_ASSOCIATIVE, BINARY}}
    };

    // There are a lot of implicit definitions in these transformations. I could either:
    // 1. Try to restrict the Transformer class to remove the implicit definitions.
    // 2. Lean into the implicit definitions, and create a sort of factory class that sets up
    //    the symbols, operators, transformers for you, and provides an interface like "specify
    //    a set of symbols (alpha), a set of operators (omega), a set of transformation rules (zeta),
    //    and a set of WFFs that are known to be true (iota). Then this factory class would have to be
    //    able to parse through those sets and define the three classes (symbols, operators, transformer).
    //    Probably actually multiple transformers. That's going to be the trickiest part: determining how to
    //    split up the set of transformations into useful transformers.
    //
    // I should really try other transformers. I should try to prove a theorem and see what happens.
    Transformer wff2cnf = {&symbols, &ops,
            {
                {"a=>b", "!a+b"},           // Implication
                {"!(a+b)", "!a*!b"},        // De Morgan's Law
                {"!(a*b)", "!a+!b"},
                {"a*a", "a"},               // Identity
                {"a+a", "a"},
                {"a*1", "a"},               // Identities of Operators
                {"1*a", "a"},
                {"a+0", "a"},
                {"0+a", "a"},
                {"a*0", "0"},
                {"0*a", "0"},
                {"a+1", "1"},
                {"1+a", "1"},
                {"a+!a", "1"},              // Complement
                {"!a+a", "1"},
                {"a*!a", "0"},
                {"!a*a", "0"},
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
            }
        };

    Transformer wff2dnf = {&symbols, &ops,
            {
                    {"a=>b", "!a+b"},           // Implication
                    {"!(a+b)", "!a*!b"},        // De Morgan's Law
                    {"!(a*b)", "!a+!b"},
                    {"a*a", "a"},               // Identity
                    {"a+a", "a"},
                    {"a*1", "a"},               // Identities of Operators
                    {"1*a", "a"},
                    {"a+0", "a"},
                    {"0+a", "a"},
                    {"a*0", "0"},
                    {"0*a", "0"},
                    {"a+1", "1"},
                    {"1+a", "1"},
                    {"a+!a", "1"},              // Complement
                    {"!a+a", "1"},
                    {"a*!a", "0"},
                    {"!a*a", "0"},
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
                    {"(a+b)*c", "(a*c)+(b*c)"}, // Distribution
                    {"c*(a+b)", "(a*c)+(b*c)"},
                    {"!!a", "a"}                // Remove double negation
            }
    };

    //std::string formula;
    //std::cout << "Enter a WFF: ";
    //std::getline(std::cin, formula);
    //AST wff(symbols, ops, formula);

    //AST wff(symbols, ops, "!q+p+q");
    AST wff(&symbols, &ops, "(p+!(q*r))=>((p+s)*t)");
    //AST wff(symbols, ops, "((p+!(q*r))=>((p+s)*t))=>(p*q)");
    //AST wff(symbols, ops, "(((p+!(q*r))=>((p+s)*t))=>(p*q))*(!q+p+q)+((p+!(q*r))=>((p+s)*t))");

    wff2cnf.applyTransformations(wff);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "\nCNF: " << wff.toString() << std::endl;
    std::cout << "Completed in: " << elapsed_time.count() << " microseconds" << std::endl;

    return 0;
}

// Test expressions:
// !(a+b)=>c
// a+b=>c
// a * (b+!c) => !d
// !a * (b+!c) => !d
// (a+(b*c))+d
