#include "lexer.h"
#include <string>

enum NodeType {
    NumericLiteral,
    StringLiteral,
    IdentifierLiteral,
};

struct Node {
    NodeType type;
};

struct NumericLiteralNode : public Node  {
    NodeType type = NumericLiteral;
    int value;
};

struct StringLiteralNode : public Node  {
    NodeType type = StringLiteral;
    std::string value;
};

struct IdentifierLiteralNode : public Node  {
    NodeType type = StringLiteral;
    std::string value;
};

class Parser {
private:
    Lexer* lexer;
    std::string input;
    std::optional<Token> lookahead;
    Node Program();
    Node DummyNode();
    NumericLiteralNode IntegerLiteral();
    StringLiteralNode StringLiteral();
    IdentifierLiteralNode IdentifierLiteral();
public:
    Parser();
    Node Parse(std::string input);
    Token Eat(std::string tokenType);
};
