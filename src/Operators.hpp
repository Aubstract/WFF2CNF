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
    FALSE,
    PARTIAL_MATCH,
    TRUE
};

class Operators
{
private:
    const std::unordered_map<std::string,OperationProperties> operators;

    bool partialMatch(const std::string& query) const {
        for (const auto& pair : operators) {
            if (std::mismatch(query.begin(), query.end(), pair.first.begin(), pair.first.end()).first == query.end()) {
                return true; // Partial match found
            }
        }
        return false; // No partial match found
    }

public:
    Operators(std::initializer_list<std::pair<std::string,OperationProperties>> ops)
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

    MatchLevel matchesOperator(const std::string& query) const
    {
        if (operators.find(query) != operators.end())
        {
            return TRUE;
        }
        else if (partialMatch(query))
        {
            return PARTIAL_MATCH;
        }
        else
        {
            return FALSE;
        }
    }

    const OperationProperties& getProperties(const std::string& op) const
    {
        if (!this->matchesOperator(op))
        {
            int i = 0;
        }

        return operators.at(op);
    }

    int getNumOperands(const std::string& op) const
    {
        if (matchesOperator(op) != TRUE)
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

    bool hasHigherOrEqualPrecedence(const std::string& op1, const std::string& op2) const
    {
        return operators.at(op1).precedence >= operators.at(op2).precedence;
    }
};

/*
constexpr int NUM_UNARY_OPERATORS = 1;
constexpr int NUM_BINARY_OPERATORS = 4;

// Operator dictionaries (in order of operator precedence)
static std::array<std::string, NUM_UNARY_OPERATORS> unary_operators = {"!"};
static std::array<std::string, NUM_BINARY_OPERATORS> binary_operators = {"*", "+", "=>"};

template<typename T, size_t N>
int find(std::array<T,N> arr, T val)
{
    int index = -1;

    for (int i=0; i<N; i++)
    {
        if (arr[i] == val)
        {
            index = i;
            break;
        }
    }

    return index;
}

int calc_num_children(const Token&);
bool is_operator(const Token&);
int compare_precedence(const Token&, const Token&);
*/

#endif //WFF2CNF_OPERATORS_HPP
