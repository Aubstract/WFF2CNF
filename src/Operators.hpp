//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_OPERATORS_HPP
#define WFF2CNF_OPERATORS_HPP

#include "Token.hpp"
#include <array>
#include <unordered_map>
#include <string>

enum Associativity
{
    ASSOCIATIVE,
    NOT_ASSOCIATIVE
};

enum Arity
{
    UNARY,
    BINARY
};

struct OperationProperties
{
    int precedence;
    Associativity associativity;
    Arity arity;
};

enum MatchLevel
{
    MATCH_FALSE,
    MATCH_PARTIAL,
    MATCH_TRUE
};

class Operators
{
private:
    const std::unordered_map<std::string,OperationProperties> operators;
    bool partialMatch(const std::string&) const;

public:
    Operators(std::initializer_list<std::pair<std::string,OperationProperties>>);

    MatchLevel matchesOperator(const std::string&) const;
    const OperationProperties& getProperties(const std::string&) const;
    int getNumOperands(const std::string&) const;
    bool hasHigherOrEqualPrecedence(const std::string&, const std::string&) const;
};

#endif //WFF2CNF_OPERATORS_HPP
