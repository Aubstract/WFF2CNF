//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_TOKEN_HPP
#define WFF2CNF_TOKEN_HPP

#include <string>

enum TokenType
{
    IDENTIFIER = 0,
    LITERAL,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
    OPEN_PAREN,
    CLOSE_PAREN
};

struct Token
{
    TokenType type;
    std::string lexeme; // Actual text that the Token represents
    Token()
    {
        type = IDENTIFIER;
        lexeme = "";
    };
    Token(const TokenType _type, const std::string& _lexeme)
    {
        type = _type;
        lexeme = _lexeme;
    }
    Token(const Token& other)
    {
        type = other.type;
        lexeme = other.lexeme;
    }
};

#endif //WFF2CNF_TOKEN_HPP
