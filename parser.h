#include <string>
#include "lexer.h"

enum NodeType {
    NumericLiteral
};

struct Node {
    NodeType type;
};

struct NumericLiteralNode : public Node  {
    NodeType type = NumericLiteral;
    long value;
};

class Parser {
private:
    Lexer* lexer;
    std::string input;
    std::optional<Token> lookahead;
    Node Program();
    NumericLiteralNode NumericLiteral();
public:
    Parser();
    Node Parse(std::string input);
    Token Eat(std::string tokenType);
};
