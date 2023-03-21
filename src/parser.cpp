#include <sstream>
#include "parser.h"
#include "logger.h"

/**
 * Parser class constructor
 * @param filereader pointer to a FileReader object
 * @param tokens list of tokens to parse
 */
Parser::Parser(Input *input, std::vector<Token> tokens) : input(input), tokens(tokens) {}

/**
 * Check if parser has reached the final token
 * @return true if parser has reached the final token, false otherwise
 */
bool Parser::is_at_end() {
    return peek().type == Eof;
}

/**
 * Advance the parser to the next token
 * @return the next token
 */
Token Parser::advance() {
    if (!is_at_end())
        current++;
    return peek();
}

/**
 * Returns the next token without advancing the current position
 * @return the next token
 */
Token Parser::peek() {
    return tokens.at(current);
}

/**
 * Returns the previous token without reducing the current position
 * @return the previous token
 */
Token Parser::previous() {
    return tokens.at(current - 1);
}

/**
 * Consumes the current token and advances
 * Throws an error if the current token does not match the expected type
 * @param type expected token type
 * @return the consumed token
 */
Token Parser::consume(TokenType type) {
    auto curr = peek();
    std::stringstream ss;
    ss << "expected " << type << ", got " << peek().type;
    return consume(type, ss.str());
}

/**
 * Consumes the current token and advances
 * Throws an error if the current token does not match the expected type
 * @param type expected token type
 * @param error_message custom error message
 * @return the consumed token
 */
Token Parser::consume(TokenType type, const std::string &error_message) {
    auto curr = peek();
    if (check(type)) {
        advance();
        return curr;
    }

    Logger::error(input, curr.line, curr.column, curr.lexeme.length(), error_message);
    // Should not be reached
    // TODO: Maybe improve this
    throw 0;
}

/**
 * Check if the current token is of the given type
 * @param type token type to match
 * @return true if the current token matches the given type, false otherwise
 */
bool Parser::check(TokenType type) {
    if (is_at_end())
        return false;
    return peek().type == type;
}

/**
 * Advance if the the current token is of the given type
 * @param type token type to match
 * @return true if the current token matches the given type, false otherwise
 */
bool Parser::match(TokenType expected) {
    if (peek().type != expected)
        return false;
    advance();
    return true;
}

/**
 * Program ::= { Declaration } EOF
 *
 * Parses the list of tokens given to the parser
 * @param verbose should the abstract syntax tree be printed
 * @return abstract syntax tree (AST) representing the given list of tokens
 */
AST *Parser::parse(bool verbose) {
    auto ast = new AST("program");
    while (!is_at_end()) {
        auto child = decl();
        ast->add_child(child);
    }
    if (verbose)
        ast->print();
    return ast;
}

/**
 * Declaration ::= VarDecl | FuncDecl ";"
 */
AST *Parser::decl() {
    AST *ast;

    switch (peek().type) {
        case Var:ast = var_decl(true);
            break;
        case Func:ast = func_decl();
            break;
        default:
            Logger::error(input, peek().line, peek().column, peek().lexeme.length(),
                          "declarations must be begin with a \"var\" or \"func\" keyword");
    }

    consume(Semicolon, "declarations must be terminated by a semicolon");
    return ast;
}

/**
 * VarDecl ::= "var" identifier identifier
 */
AST *Parser::var_decl(bool global) {
    auto token = consume(Var);
    auto ast = new AST(global ? "globalvar" : "var", token.line, token.column);

    // Variable name
    auto id = consume(Identifier, "variable identifier must follow the \"var\" keyword");
    ast->add_child(new AST("newid", id.lexeme, id.line, id.column));

    // Variable type
    auto type = consume(Identifier, "variable type must follow the identifier");
    ast->add_child(new AST("typeid", type.lexeme, type.line, type.column));

    return ast;
}

/**
 * FuncDecl ::= "func" identifier Signature Block
 */
AST *Parser::func_decl() {
    auto token = consume(Func);
    auto ast = new AST("func", token.line, token.column);

    // Function name
    auto id = consume(Identifier, "function identifier must follow the \"func\" keyword");
    ast->add_child(new AST("newid", id.lexeme, id.line, id.column));

    // Function signature
    auto signature = func_sig();
    ast->add_child(signature);

    // Function body
    auto body = block();
    ast->add_child(body);

    return ast;
}

/**
 * Signature ::= "(" { identifier identifier "," } ")" [ identifier ]
 */
AST *Parser::func_sig() {
    auto ast = new AST("sig");

    // Opening paren
    consume(LeftParen, "function signature must open with \"(\"");

    // Formals
    auto formals = new AST("formals");
    ast->add_child(formals);
    while (check(Identifier)) {
        auto formal = new AST("formal");
        auto id = consume(Identifier, "signature formal must begin with an identifier");
        formal->add_child(new AST("newid", id.lexeme, id.line, id.column));
        auto type = consume(Identifier, "expected a type to follow the formal identifier");
        formal->add_child(new AST("typeid", type.lexeme, type.line, type.column));
        formals->add_child(formal);

        if (!match(Comma))
            break;
    };

    // Closing paren
    consume(RightParen, "function signature must be closed with \")\"");

    // Optional return type
    auto has_type = check(Identifier);
    if (has_type) {
        auto type = consume(Identifier);
        ast->add_child(new AST("typeid", type.lexeme, type.line, type.column));
    } else {
        ast->add_child(new AST("typeid", "$void"));
    }

    return ast;
}

/**
 * Block ::= "{" { Statement } "}"
 */
AST *Parser::block() {
    auto ast = new AST("block");
    consume(LeftBracket, "block must begin with \"{\"");

    // Block body
    while (!check(RightBracket)) {
        auto child = stmt();
        ast->add_child(child);
    }

    consume(RightBracket, "block must close with \"}\"");
    return ast;
}

/**
 * Statement ::=
 *  | VarDecl
 *  | IfStmt
 *  | ForStmt
 *  | BreakStmt
 *  | ReturnStmt
 *  | Block
 *  | ExpressionStmt
 *  ";"
 */
AST *Parser::stmt() {
    AST *ast;

    switch (peek().type) {
        case Var:ast = var_decl(false);
            break;
        case If:ast = if_stmt();
            break;
        case For:ast = for_stmt();
            break;
        case Break:ast = break_stmt();
            break;
        case Return:ast = return_stmt();
            break;
        case LeftBracket:ast = block();
            break;
        default:ast = expr_stmt();
            break;
    }

    consume(Semicolon, "statement must be terminated by \";\"");
    return ast;
}

/**
 * IfStmt ::= "if" Expression block [ "else" IfStmt | block ]
 */
AST *Parser::if_stmt() {
    auto token = consume(If);
    auto ast = new AST("if", token.line, token.column);

    // Condition
    auto condition = expr();
    ast->add_child(condition);

    // If body
    auto body = block();
    ast->add_child(body);

    // Optional else/else-if
    if (match(Else)) {
        // Else-if
        if (check(If))
            ast->add_child(if_stmt());
            // Else
        else
            ast->add_child((new AST("else"))->add_child(block()));
    }

    return ast;
}

/**
 * ForStmt ::= "for" Expression block
 */
AST *Parser::for_stmt() {
    auto token = consume(For);
    auto ast = new AST("for", token.line, token.column);

    // Optional condition
    AST *condition;
    if (check(LeftBracket)) {
        condition = new AST("id", "$true");
    } else {
        condition = expr();
    }
    ast->add_child(condition);

    // For body
    auto body = block();
    ast->add_child(body);

    return ast;
}

/**
 * BreakStmt ::= "break"
 */
AST *Parser::break_stmt() {
    auto token = consume(Break);
    auto ast = new AST("break", token.line, token.column);
    return ast;
}

/**
 * ReturnStmt ::= "return" [ Expression ]
 */
AST *Parser::return_stmt() {
    auto token = consume(Return);
    auto ast = new AST("return", token.line, token.column);

    // Optional return expression
    if (!check(Semicolon)) {
        auto child = expr();
        ast->add_child(child);
    }

    return ast;
}

/**
 * ExpressionStmt ::= Assignment
 */
AST *Parser::expr_stmt() {
    auto ast = assignment();
    return ast;
}

/**
 * Assignment ::= Expr "=" Expr | Expr
 */
AST *Parser::assignment() {
    auto l = expr();

    if (match(Equal)) {
        auto op = previous();
        auto r = expr();
        l = (new AST(op.lexeme, op.line, op.column))->add_child(l)->add_child(r);
    }

    return l;
}

/**
 * Expression ::= OrExpr
 */
AST *Parser::expr() {
    auto ast = or_expr();
    return ast;
}

/**
 * OrExpr ::= AndExpr { "||" AndExpr }
 */
AST *Parser::or_expr() {
    auto l = and_expr();

    while (match(Or)) {
        auto op = previous();
        auto r = and_expr();
        l = (new AST(op.lexeme, op.line, op.column))->add_child(l)->add_child(r);
    }

    return l;
}

/**
 * AndExpr ::= RelExpr { "&&" RelExpr }
 */
AST *Parser::and_expr() {
    auto l = rel_expr();

    while (match(And)) {
        auto op = previous();
        auto r = rel_expr();
        l = (new AST(op.lexeme, op.line, op.column))->add_child(l)->add_child(r);
    }

    return l;
}

/**
 * RelExpr ::= AddExpr { ("==" | "!=" | "<" | "<=" | ">" | ">=") AddExpr }
 */
AST *Parser::rel_expr() {
    auto l = add_expr();

    while (match(EqualEqual) || match(NotEqual) || match(Less) ||
           match(LessEqual) || match(Greater) || match(GreaterEqual)) {
        auto op = previous();
        auto r = add_expr();
        l = (new AST(op.lexeme, op.line, op.column))->add_child(l)->add_child(r);
    }

    return l;
}

/**
 * AddExpr ::= MulExpr { ("+" | "-") MulExpr }
 */
AST *Parser::add_expr() {
    auto l = mul_expr();

    while (match(Add) || match(Subtract)) {
        auto op = previous();
        auto r = mul_expr();
        l = (new AST(op.lexeme, op.line, op.column))->add_child(l)->add_child(r);
    }

    return l;
}

/**
 * MulExpr ::= UnaryExpr { ("*" | "/" | "%") UnaryExpr }
 */
AST *Parser::mul_expr() {
    auto l = unary_expr();

    while (match(Multiply) || match(Divide) || match(Modulo)) {
        auto op = previous();
        auto r = unary_expr();
        l = (new AST(op.lexeme, op.line, op.column))->add_child(l)->add_child(r);
    }

    return l;
}

/**
 * UnaryExpr ::= ("!" | "-") UnaryExpr | FuncCall
 */
AST *Parser::unary_expr() {
    // TODO: Refactor this to be more concise if you are out of fun things to do in life :)
    if (match(Not)) {
        auto op = previous();
        auto r = unary_expr();
        return (new AST(op.lexeme, op.line, op.column))->add_child(r);
    }
    if (match(Subtract)) {
        // TODO: Hacky solution for negative integers
        if(match(Integer))
            return new AST("int", "-" + previous().lexeme, previous().line, previous().column - 1);
        auto op = previous();
        auto r = unary_expr();
        return (new AST("u" + op.lexeme, op.line, op.column))->add_child(r);
    }

    return func_call();
}

/**
 * FuncCall ::= Operand | Operand "(" [ Expression { "," Expression } ] ")"
 */
AST *Parser::func_call() {
    auto ast = operand();

    // Arguments
    while (match(LeftParen)) {
        ast = (new AST("funccall", previous().line, previous().column))->add_child(ast);
        auto actuals = new AST("actuals");
        ast->add_child(actuals);
        while (!match(RightParen)) {
            actuals->add_child(expr());
            if (!match(Comma)) {
                consume(RightParen, "function call must closed with \")\"");
                break;
            }
        }
    }

    return ast;
}

/**
 * Operand ::= int_lit | string_lit | identifier | ";" | "(" Expression ")"
 */
AST *Parser::operand() {
    if (match(Integer))
        return new AST("int", previous().lexeme, previous().line, previous().column);

    if (match(String))
        return new AST("string", previous().lexeme, previous().line, previous().column);

    if (match(Identifier))
        return new AST("id", previous().lexeme, previous().line, previous().column);

    if (check(Semicolon))
        return new AST("emptystmt");

    if (match(LeftParen)) {
        auto ast = expr();
        consume(RightParen, "parenthesised operands must be closed with \")\"");
        return ast;
    }

    Logger::error(input, peek().line, peek().column + 1, 1, "expected expression");
    // Should not be reached
    // TODO: Maybe improve this
    throw 0;
}
