#include "golf.h"
#include "parser.h"
#include <iostream>

int main() {
    Parser* parser = new Parser();
    NumericLiteralNode n = parser->Parse("32");

	std::cout << n.value << std::endl;
}
