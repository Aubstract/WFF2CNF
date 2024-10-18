//
// Created by Aubrey on 10/18/2024.
//
#include <array>
#include <iostream>
#include <cctype>
#include <map>
#include <string>
#include <stack>
#include <vector>

constexpr int NUM_UNARY_OPERATORS = 1;
constexpr int NUM_BINARY_OPERATORS = 4;

// Operator dictionaries (in order of operator precedence)
std::array<std::string, NUM_UNARY_OPERATORS> unary_operators = {"!"};
std::array<std::string, NUM_BINARY_OPERATORS> binary_operators = {"*", "+", "=>"};
/*
std::map<std::string, std::string> str_transforms = {
        {"a=>b","!a+b"}, // implies
        {"a+!a","1"},    // complement
        {"a*!a","0"},
        {"!(a+b)","!a*!b"},
        {"!(a*b)","!a+!b"},
        {"a*a","a"},
        {"a+a","a"},
        {"a+(a*b)","a"},
        {"a*(a+b)","a"},
        {"(a+b)*(!b+c)","a+c"}
};
 */

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

enum TokenType
{
    IDENTIFIER = 0,
    LITERAL,
    UNARY_OPERATOR,
    BINARY_OPERATOR,
    OPEN_PAREN,
    CLOSE_PAREN
};

struct token
{
    TokenType type;
    std::string lexeme; // Actual text that the token represents
    token()
    {
        type = IDENTIFIER;
        lexeme = "";
    };
    token(const TokenType _type, const std::string& _lexeme)
    {
        type = _type;
        lexeme = _lexeme;
    }
    token(const token& other)
    {
        type = other.type;
        lexeme = other.lexeme;
    }
};

struct AST_node
{
    token token;
    std::vector<AST_node*> children; // Variable number of children in order to deal w/ binary operators,
    // unary operators, and identifiers (which have no children).
    // Basically a node can have [0-2] children.
};

int calc_num_children(const token& token)
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

bool is_operator(token& token)
{
    return (find(unary_operators, token.lexeme) != -1) || (find(binary_operators, token.lexeme) != -1);
}

// if operator A has a higher precedence than operator B, then it will return 1,
// if they are the same precedence it will return 0,
// if operator B has a higher precedence than operator A, then it will return -1
int compare_precedence(const token& a, const token& b)
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

std::vector<token> tokenize_wff(std::string formula)
{
    std::vector<token> tokens;
    std::string curr;
    for (size_t i=0; i<formula.size(); i++)
    {
        curr = formula[i];

        if (curr == " ")
        {
            continue;
        }

        // Handle the two-char operators (e.g. the implies operator which is =>)
        if (curr == "=" && (i < (formula.size() - 1)))
        {
            if (formula[i+1] == '>')
            {
                curr += formula[i+1];
                i++; // Skip the next char
            }
        }

        if (find(unary_operators, curr) != -1) // If curr is a unary operator
        {
            tokens.push_back(token{UNARY_OPERATOR, curr});
        }
        else if (find(binary_operators, curr) != -1) // If curr is a binary operator
        {
            tokens.push_back(token{BINARY_OPERATOR, curr});
        }
        else if (curr == "(")
        {
            tokens.push_back(token{OPEN_PAREN, curr});
        }
        else if (curr == ")")
        {
            tokens.push_back(token{CLOSE_PAREN, curr});
        }
        else // curr is an identifier
        {
            tokens.push_back(token{IDENTIFIER, curr});
        }
    }
    return tokens;
}

std::vector<token> shunting_yard(const std::vector<token>& tokens)
{
    std::vector<token> postfix;
    postfix.reserve(tokens.size());
    std::stack<token> token_stack;
    token temp;

    for (size_t i=0; i<tokens.size(); i++)
    {
        if (tokens[i].type == IDENTIFIER)
        {
            postfix.emplace_back(tokens[i]); // Put operands right into output vec
            if (!token_stack.empty() && find(unary_operators, token_stack.top().lexeme) != -1)
            {
                temp.type = token_stack.top().type;
                temp.lexeme = token_stack.top().lexeme;
                token_stack.pop();
                postfix.emplace_back(temp);
            }
        }
        else if (tokens[i].type == BINARY_OPERATOR || tokens[i].type == UNARY_OPERATOR)
        {
            // While there is an operator on the stack AND that operator has higher-or-equal precedence than the current one
            while (!token_stack.empty()
                   && find(unary_operators, tokens[i].lexeme) == -1
                   && is_operator(token_stack.top())
                   && (compare_precedence(token_stack.top(), tokens[i]) <= 0))
            {
                temp.type = token_stack.top().type;
                temp.lexeme = token_stack.top().lexeme;
                token_stack.pop();
                postfix.emplace_back(temp);
            }

            token_stack.push(tokens[i]); // Finally, push the current operator onto the stack
        }
        else if (tokens[i].type == OPEN_PAREN)
        {
            token_stack.push(tokens[i]);
        }
        else // tokens[i].type == CLOSE_PAREN
        {
            while (!token_stack.empty() && token_stack.top().type != OPEN_PAREN)
            {
                temp.type = token_stack.top().type;
                temp.lexeme = token_stack.top().lexeme;
                token_stack.pop();
                postfix.emplace_back(temp);
            }
            token_stack.pop(); // Pop the OPEN_PAREN

            if (!token_stack.empty() && find(unary_operators, token_stack.top().lexeme) != -1)
            {
                temp.type = token_stack.top().type;
                temp.lexeme = token_stack.top().lexeme;
                token_stack.pop();
                postfix.emplace_back(temp);
            }
        }
    }

    // Pop any remaining operators on the stack
    while (!token_stack.empty())
    {
        temp.type = token_stack.top().type;
        temp.lexeme = token_stack.top().lexeme;
        token_stack.pop();
        postfix.emplace_back(temp);
    }

    return postfix;
}

void insert_ast_nodes(AST_node*& root, const std::vector<token>& tokens, int i)
{
    if (!root)
    {
        root = new AST_node {tokens[i], {nullptr}};
        root->children.resize(calc_num_children(tokens[i]), nullptr);
    }

    for (int j=1; j<=calc_num_children(tokens[i]); j++)
    {
        insert_ast_nodes(root->children[j-1], tokens, i-j);
    }
}

AST_node* ast(const std::string expression)
{
    std::vector<token> tokens = tokenize_wff(expression);

    tokens = shunting_yard(tokens);

    // Just for debug purposes
    std::string acc;
    for (auto & token : tokens)
    {
        acc += token.lexeme;
    }
    std::cout << acc << std::endl;

    AST_node* root = nullptr;
    int i = tokens.size() - 1;
    insert_ast_nodes(root, tokens, i);

    return root;
}

void translate_wff_to_cnf(std::string& expression)
{
    AST_node* root = ast(expression);

    // Build "library" of transformations
    std::map<AST_node*, AST_node*> ast_transforms;

    // Translate implications (a=>b == !a+b)

}

int main() {
    // !(a+b)=>c
    // a+b=>c
    // a * (b+!c) => !d
    // !a * (b+!c) => !d
    // (a+(b*c))+d

    std::map<AST_node*, AST_node*> ast_transforms = {
            {ast("a=>b"), ast("!a+b")}, // implies
            {ast("a+!a"), ast("1")},    // complement
            {ast("a*!a"), ast("0")},
            {ast("!(a+b)"), ast("!a*!b")},
            {ast("!(a*b)"), ast("!a+!b")},
            {ast("a*a"), ast("a")},
            {ast("a+a"), ast("a")},
            {ast("a+(a*b)"), ast("a")},
            {ast("a*(a+b)"), ast("a")},
            {ast("(a+b)*(!b+c)"), ast("a+c")}
    };
    std::string formula;

    // write a header that explains the necessary syntax (e.g. only use +, *, !, and =>)
    std::cout << "Enter a WFF: " << std::flush;
    std::getline(std::cin, formula);

    translate_wff_to_cnf(formula);

    // std::cout << "\nCNF: " << formula << std::endl;

    return 0;
}
