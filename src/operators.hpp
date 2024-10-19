//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_OPERATORS_HPP
#define WFF2CNF_OPERATORS_HPP

#include "Token.hpp"
#include <array>
#include <string>

// TODO: Consolidate operators into class (?)

constexpr int NUM_UNARY_OPERATORS = 1;
constexpr int NUM_BINARY_OPERATORS = 4;

// Operator dictionaries (in order of operator precedence)
std::array<std::string, NUM_UNARY_OPERATORS> unary_operators = {"!"};
std::array<std::string, NUM_BINARY_OPERATORS> binary_operators = {"*", "+", "=>"};

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

int calc_num_children(const Token& token)
{
    if (token.type == IDENTIFIER)
    {
        return 0;
    }
    else if (token.type == UNARY_OPERATOR)
    {
        return 1;
    }
    else if (token.type == BINARY_OPERATOR)
    {
        return 2;
    }
    else // default
    {
        return 0;
    }
}

bool is_operator(Token& token)
{
    return (find(unary_operators, token.lexeme) != -1) || (find(binary_operators, token.lexeme) != -1);
}

// if operator A has a higher precedence than operator B, then it will return 1,
// if they are the same precedence it will return 0,
// if operator B has a higher precedence than operator A, then it will return -1
int compare_precedence(const Token& a, const Token& b)
{
    if (find(unary_operators, a.lexeme) != -1) // operator A is unary
    {
        if (find(unary_operators, b.lexeme) != -1) // operator A is unary and operator B is unary
        {
            if (find(unary_operators, a.lexeme) < find(unary_operators, b.lexeme))
            {
                return 1;
            }
            else if (find(unary_operators, a.lexeme) == find(unary_operators, b.lexeme))
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
        else // operator A is unary and operator B is binary
        {
            return 1;
        }
    }
    else // operator A is binary
    {
        if (find(unary_operators, b.lexeme) != -1) // operator A is binary and operator B is unary
        {
            return -1;
        }
        else // operator A is binary and operator B is binary
        {
            if (find(binary_operators, a.lexeme) < find(unary_operators, b.lexeme))
            {
                return 1;
            }
            else if (find(binary_operators, a.lexeme) == find(unary_operators, b.lexeme))
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
    }
}

#endif //WFF2CNF_OPERATORS_HPP
