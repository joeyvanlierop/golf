#include <set>
#include "lexer.h"
#include "logger.h"

/**
 * Lexer class constructor
 * @param filereader pointer to a FileReader object
 */
Lexer::Lexer(Input *input) : input(input) { }

/**
 * Check if lexer is at the end of the input
 * @return true if lexer is at the end of the input, false otherwise
 */
bool Lexer::is_at_end() {
    return current >= input->data.length();
}

/**
 * Advance the lexer to the next character in the input
 * @return the next character in the input
 */
char Lexer::advance() {
    column++;
    return input->data[current++];
}

/**
 * Creates a token of the given type with the given range
 * @param token_type the type of token to create
 * @param start the start index of the substring
 * @param end the end index of the substring
 * @return the created token
 */
Token Lexer::create_token(TokenType token_type, int start, int end) {
    return Token(token_type, input->data.substr(start, (end - start)), line, column - current + start);
}

/**
 * Creates a token of the given type with the given lexeme
 * @param token_type the type of token to create
 * @param lexeme the lexeme of the token
 * @return the created token
 */
Token Lexer::create_token(TokenType token_type, std::string lexeme) {
    return Token(token_type, lexeme, line, column - current + start);
}

/**
 * Check if the next character in the input matches the expected character
 * @param expected character to match
 * @return true if the next character matches the expected character, false otherwise
 */
bool Lexer::match(char expected) {
    if (peek() != expected)
        return false;
    advance();
    return true;
}

/**
 * Checks if the current character in the input matches the expected character
 * If it does, it returns the matched token type, otherwise it returns the unmatched token type
 * @param expected the character that is expected to be matched
 * @param matched the token type to be returned if the character is matched
 * @param unmatched the token type to be returned if the character is not matched
 * @return the matched or unmatched token type
 */
TokenType Lexer::either(char expected, TokenType matched, TokenType unmatched) {
    return match(expected) ? matched : unmatched;
}

/**
 * Returns the next character in the input without advancing the current position
 * @return the next character in the input
 */
char Lexer::peek() {
    return input->data.at(current);
}

/**
 * Infers if a semicolon token should be inserted
 * @return the semicolon token if the last token is valid, otherwise nullopt
 */
std::optional<Token> Lexer::infer_semicolon() {
    const std::set<TokenType> valid = {
            Identifier, Integer, String, Break, Return, RightParen, RightBracket
    };
    if (tokens.size() > 0)
        if (valid.count(tokens.back().type))
            return create_token(Semicolon, "");
    return std::nullopt;
}

/**
 * Creates a semicolon token if the last token is a valid token, and increments the line and column count
 * @return the semicolon token if the last token is valid, otherwise nullopt
 */
std::optional<Token> Lexer::newline() {
    auto token = infer_semicolon();
    line++;
    column = 1;
    return token;
}

/**
 * Scans the input for a number and creates a token of type Integer.
 * @return a token of type Integer
 */
Token Lexer::number() {
    while (!is_at_end() && is_digit(peek()))
        advance();
    return create_token(Integer);
}

/**
 * Scans the input for an identifier and creates a token of type Identifier or Keyword.
 * @return a token of type Identifier or Keyword
 */
Token Lexer::identifier() {
    while (!is_at_end() && is_alphanumeric(peek()))
        advance();

    auto lexeme = input->data.substr(start, (current - start));
    if (Keywords.count(lexeme))
        return create_token(Keywords[lexeme]);
    return create_token(Identifier);
}

/**
 * Creates a token of the given type
 * @param token_type the type of token to create
 * @return the created token
 */
Token Lexer::create_token(TokenType token_type) {
    return create_token(token_type, start, current);
}

/**
 * Returns true if the given character is a digit
 * @param c the character to check
 * @return true if the character is a digit, false otherwise
 */
bool Lexer::is_digit(char c) {
    return c >= '0' && c <= '9';
}

/**
 * Returns true if the given character is alpha
 * @param c the character to check
 * @return true if the character is alpha, false otherwise
 */
bool Lexer::is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

/**
 * Returns true if the given character is alphanumeric
 * @param c the character to check
 * @return true if the character is alphanumeric, false otherwise
 */
bool Lexer::is_alphanumeric(char c) {
    return is_alpha(c) || is_digit(c);
}

/**
 * Iterates through the input string, matching a token at each iteration
 * When the end of the input string is reached, the function adds a Eof token to the tokens vector and returns it
 * @param verbose should the token be printed
 * @return a vector of all the matched tokens
*/
std::vector<Token> Lexer::match_tokens(bool verbose) {
    while (!is_at_end()) {
        start = current;
        auto token = match_token();
        if (token.has_value()) {
            tokens.push_back(token.value());
            if (verbose)
                std::cout << token.value() << std::endl;
        }
    }
    auto closing_semicolon = infer_semicolon();
    if(closing_semicolon.has_value())
        tokens.push_back(closing_semicolon.value());
    tokens.push_back(create_token(Eof, current, current));

    return tokens;
}

/**
 * Matches a single token from the input and returns it
 * @return the matched token, or nullopt if a token was not matched
 */
std::optional<Token> Lexer::match_token() {
    char c = advance();

    // Ugly approach to allow complex statements to occupy a single line
    // TODO: Refactor this lmao
    if(c == '}' && tokens.back().type != Semicolon) {
        current--;
        return create_token(Semicolon);
    }

    switch (c) {
        // Whitespace
        case ' ':
        case '\r':
        case '\t': return std::nullopt;

        // Newline (with semicolon inference)
        case '\n': return newline();

        // Single character
        case '{': return create_token(LeftBracket);
        case '}': return create_token(RightBracket);
        case '(': return create_token(LeftParen);
        case ')': return create_token(RightParen);
        case ';': return create_token(Semicolon);
        case ':': return create_token(Colon);
        case ',': return create_token(Comma);
        case '+': return create_token(Add);
        case '-': return create_token(Subtract);
        case '*': return create_token(Multiply);
        case '%': return create_token(Modulo);

        // Either
        case '!': return create_token(either('=', NotEqual, Not));
        case '=': return create_token(either('=', EqualEqual, Equal));
        case '>': return create_token(either('=', GreaterEqual, Greater));
        case '<': return create_token(either('=', LessEqual, Less));

        // Binary
        case '&':
            if (match('&'))
                return create_token(And);
            else
                Logger::error(input, line, column, 1, "bitwise AND not supported");
        case '|':
            if (match('|'))
                return create_token(Or);
            else
                Logger::error(input, line, column, 1, "bitwise OR not supported");

        // Comment
        case '/':
            if (match('/')) {
                while (!is_at_end() && peek() != '\n')
                    advance();
                return std::nullopt;
            } else
                return create_token(Divide);

        // String literal
        case '"':
            while (!is_at_end() && peek() != '"') {
                if (match('\\'))
                    if (match('b') || match('f') || match('n') || match('r') ||
                        match('t') || match('\\') || match('\"'))
                        continue;
                    else
                        Logger::error(input, line, column, 2,
                              "bad string escape '\\" + std::string(1, peek()) + "'");
                if (peek() == '\n')
                    Logger::error(input, line, column - current + start + 1, current - start + 1,
                          "string contains newline");
                advance();
            }

            if (is_at_end())
                Logger::error(input, line, column - current + start + 1, current - start + 1, "unterminated string");

            advance();
            return create_token(String, start + 1, current - 1);

        // Non-trivial
        default:
            // Integer literal
            if (is_digit(c))
                return number();

            // Identifier
            else if (is_alpha(c))
                return identifier();

            // Non-ascii character
            else if (!isascii(c))
                Logger::warning(input, line, column, 1, "skipping non-ascii character");

            // Unknown
            else
                Logger::warning(input, line, column, 1, "skipping unknown character '" + std::string(1, c) + "'");
    }

    // Couldn't match any character, ignore
    return std::nullopt;
}
