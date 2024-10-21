//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_TOKEN_HPP
#define WFF2CNF_TOKEN_HPP

#include <string>
#include <utility>

enum TokenType
{
    VARIABLE,
    CONSTANT,
    OPERATOR,
    OPEN_PAREN,
    CLOSE_PAREN
};

struct Token
{
    TokenType type;
    std::string lexeme; // Actual text that the Token represents

    Token(const TokenType _type, std::string _lexeme) : type(_type), lexeme(std::move(_lexeme)) {}
    Token(const Token& other) = default;
};

#endif //WFF2CNF_TOKEN_HPP
