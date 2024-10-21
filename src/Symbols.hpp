//
// Created by Aubrey on 10/20/2024.
//

#ifndef WFF2CNF_SYMBOLS_HPP
#define WFF2CNF_SYMBOLS_HPP

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

enum ConstantValue
{
    CONST_FALSE,
    CONST_TRUE,
    CONST_NOT_FOUND
};

struct Constant
{
    ConstantValue value;
    std::string lexeme;

    Constant(std::string lex, const ConstantValue val) : lexeme(std::move(lex)), value(val) {}
};

struct Variable
{
    std::string lexeme;

    Variable(std::string lex) : lexeme(std::move(lex)) {}
};

class Symbols
{
private:
    const std::vector<Constant> constants;
    const std::vector<Variable> variables;

public:
    Symbols(std::initializer_list<std::pair<std::string,ConstantValue>>,
            std::initializer_list<std::string>);

    bool isSymbol(const std::string&) const;
    bool isConstant(const std::string&) const;
    bool isVariable(const std::string&) const;
    ConstantValue getConstValue(const std::string&) const;
};


#endif //WFF2CNF_SYMBOLS_HPP
