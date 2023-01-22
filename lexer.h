#include <string>
#include <optional>
#include <regex>

struct Token {
    std::string type;
    std::string value;
};

static std::pair<std::string, std::string> Spec[] =  {
        {"^\\d+", "NUMBER"},
};

class Lexer {
private:
    std::string input;
    int cursor;
    bool HasMoreTokens();
    std::optional<std::string> Match(std::string regexp, std::string input);
public:
    void Init(std::string input);
    std::optional<Token> GetNextToken();
};
