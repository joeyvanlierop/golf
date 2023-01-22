#include "golf.h"
#include "parser.h"
#include <iostream>

int main() {
    Parser *parser = new Parser();
    Node node = parser->Parse("32");

    switch (node.type) {
        case NumericLiteral: {
            auto numNode = static_cast<const NumericLiteralNode*>(&node);
            std::cout << numNode->type << std::endl;
            std::cout << numNode->value << std::endl;
            break;
        }
        default: {
            std::cout << "Invalid type" << std::endl;
            break;
        }
    }
}
