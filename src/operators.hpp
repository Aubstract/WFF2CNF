//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_OPERATORS_HPP
#define WFF2CNF_OPERATORS_HPP

#include "Token.hpp"
#include <array>
#include <string>

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

#endif //WFF2CNF_OPERATORS_HPP
