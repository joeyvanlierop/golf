#include "golf.h"
#include "parser.h"

int main() {
    Parser *parser = new Parser();

    std::string input = R"(
42
4 213
"abc"
var cole
if {
    var test = 0;
})";
    parser->Parse(input);
}
