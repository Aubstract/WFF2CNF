//
// Created by Aubrey on 10/19/2024.
//

#include "Operators.hpp"

Operators::Operators(std::initializer_list<std::pair<std::string,OperationProperties>> ops)
    : operators([&ops]()
    {
        std::unordered_map<std::string,OperationProperties> temp;
        for (const auto& op : ops)
        {
            temp[op.first] = op.second;
        }
        return temp;
    }())
    {}

MatchLevel Operators::matchesOperator(const std::string& query) const
{
    if (operators.find(query) != operators.end())
    {
        return MATCH_TRUE;
    }
    else if (partialMatch(query))
    {
        return MATCH_PARTIAL;
    }
    else
    {
        return MATCH_FALSE;
    }
}

const OperationProperties& Operators::getProperties(const std::string& op) const
{
    return operators.at(op);
}

int Operators::getNumOperands(const std::string& op) const
{
    if (matchesOperator(op) != MATCH_TRUE)
    {
        return 0;
    }

    switch (getProperties(op).arity)
    {
        case UNARY:
            return 1;
        case BINARY:
            return 2;
        default:
            return 0;
    }
}

bool Operators::hasHigherOrEqualPrecedence(const std::string& op1, const std::string& op2) const
{
    return operators.at(op1).precedence >= operators.at(op2).precedence;
}

bool Operators::partialMatch(const std::string& query) const
{
    for (const auto& pair : operators)
    {
        if (std::mismatch(query.begin(), query.end(), pair.first.begin(), pair.first.end()).first == query.end()) {
            return true; // Partial match found
        }
    }
    return false; // No partial match found
}