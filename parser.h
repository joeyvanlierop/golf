#include <string>
#include "lexer.h"

class Node {
};

struct NumericLiteralNode : public Node  {
    long value;
};

class Parser {
private:
    Lexer* lexer;
    std::string input;
    std::optional<Token> lookahead;
    NumericLiteralNode Program();
    NumericLiteralNode NumericLiteral();
public:
    Parser();
    NumericLiteralNode Parse(std::string input);
    Token Eat(std::string tokenType);
};
