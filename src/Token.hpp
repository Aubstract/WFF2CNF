//
// Created by Aubrey on 10/18/2024.
//

#ifndef WFF2CNF_TOKEN_HPP
#define WFF2CNF_TOKEN_HPP

#include <string>

enum TokenType
{
    IDENTIFIER = 0,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
    OPEN_PAREN,
    CLOSE_PAREN,
    TRUE,
    FALSE
};

enum Associativity
{
    ASSOCIATIVE,
    NOT_ASSOCIATIVE
};

struct Token
{
    TokenType type;
    //Associativity associativity;
    std::string lexeme; // Actual text that the Token represents


    Token()
    {
        type = IDENTIFIER;
        //associativity = NOT_ASSOCIATIVE;
        lexeme = "";
    };

    Token(const TokenType _type, const std::string& _lexeme)
    {
        type = _type;
        //associativity = NOT_ASSOCIATIVE;
        lexeme = _lexeme;
    }

    Token(const TokenType _type, Associativity _associativity, const std::string& _lexeme)
    {
        type = _type;
        //associativity = _associativity;
        lexeme = _lexeme;
    }
    Token(const Token& other)
    {
        type = other.type;
        //associativity = other.associativity;
        lexeme = other.lexeme;
    }
};

#endif //WFF2CNF_TOKEN_HPP
