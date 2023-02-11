#include <vector>
#include "expr.h"
#include "token.h"

class Parser {
public:
    Parser();
    std::vector<Expr> parse(std::vector<Token>);

private:
    
};
