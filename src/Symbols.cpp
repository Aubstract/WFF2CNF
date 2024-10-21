//
// Created by Aubrey on 10/20/2024.
//

#include "Symbols.hpp"

Symbols::Symbols(const std::initializer_list<std::pair<std::string, ConstantValue>> _constants,
                 const std::initializer_list<std::string> _variables)
    : constants([&_constants]()
                {
                    std::vector<Constant> temp;
                    for (const auto& pair : _constants)
                    {
                        temp.emplace_back(pair.first, pair.second);  // Construct constant objects
                    }
                    return temp;
                }()),
      variables([&_variables]()
                {
                    std::vector<Variable> temp;
                    for (const auto& var : _variables)
                    {
                        temp.emplace_back(var);  // Construct variable objects
                    }
                    return temp;
                }())
{}

bool Symbols::isSymbol(const std::string& query) const
{
    if (isConstant(query) || isVariable(query))
    {
        return true;
    }

    return false;
}

bool Symbols::isConstant(const std::string& query) const
{
    for (const Constant& constant : constants)
    {
        if (constant.lexeme == query)
        {
            return true;
        }
    }

    return false;
}

bool Symbols::isVariable(const std::string& query) const
{
    for (const Variable& variable : variables)
    {
        if (variable.lexeme == query)
        {
            return true;
        }
    }

    return false;
}

ConstantValue Symbols::getConstValue(const std::string& query) const
{
    for (const Constant& constant : constants)
    {
        if (constant.lexeme == query)
        {
            return constant.value;
        }
    }

    return CONST_NOT_FOUND;
}