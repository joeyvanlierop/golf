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
std::shared_ptr<AST> Parser::parse(bool verbose) {
	// Gather statements
	std::vector<std::shared_ptr<AST>> statements;
    while (!is_at_end()) {
        auto child = decl();
		statements.push_back(child);
    }

	// Construct program
	auto program = std::make_shared<Program>(statements);
    if (verbose)
		program->print();
    return program;
}

/**
 * Declaration ::= VarDecl | FuncDecl ";"
 */
std::shared_ptr<AST> Parser::decl() {
    std::shared_ptr<AST> ast;

    switch (peek().type) {
        case Var: ast = var_decl(true);
            break;
        case Func: ast = func_decl();
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
std::shared_ptr<AST> Parser::var_decl(bool global) {
	// Must start with "var" keyword
    auto token = consume(Var);

	// Variable name
	auto idToken = consume(Identifier, "variable identifier must follow the \"var\" keyword");
	auto id = std::make_shared<Id>(idToken.line, idToken.column, idToken.lexeme);

	// Variable type
	auto typeToken = consume(Identifier, "variable type must follow the identifier");
	auto type = std::make_shared<Type>(typeToken.line, typeToken.column, typeToken.lexeme);


	// Construct variable
	// TODO: Fix this with namespaces
    auto ast = std::make_shared<Variable>(token.line, token.column, id, type);
    return ast;
}

/**
 * FuncDecl ::= "func" identifier Signature Block
 */
std::shared_ptr<AST> Parser::func_decl() {
	// Must start with "func" keyword
	auto token = consume(Func);

	// Function name
	auto idToken = consume(Identifier, "function identifier must follow the \"func\" keyword");
	auto id = std::make_shared<Id>(idToken.line, idToken.column, idToken.lexeme);

	// Function signature
	auto signature = func_sig();

	// Function body
	auto body = block();

	// Construct function
    auto ast = std::make_shared<Function>(token.line, token.column, id, body);
    return ast;
}

/**
 * Signature ::= "(" { identifier identifier "," } ")" [ identifier ]
 */
std::shared_ptr<AST> Parser::func_sig() {
    // Opening paren
    consume(LeftParen, "function signature must open with \"(\"");

    // Formals
    std::vector<std::shared_ptr<AST>> formals;
    while (check(Identifier)) {
        auto idToken = consume(Identifier, "signature formal must begin with an identifier");
        auto id = std::make_shared<Id>(idToken.line, idToken.column, idToken.lexeme);

        auto typeToken = consume(Identifier, "expected a type to follow the formal identifier");
		auto type = std::make_shared<Type>(typeToken.line, typeToken.column, typeToken.lexeme);

		auto formal = std::make_shared<Formal>(id, type);
		formals.push_back(formal);

        if (!match(Comma))
            break;
    };

    // Closing paren
    consume(RightParen, "function signature must be closed with \")\"");

    // Optional return type
	std::shared_ptr<Type> type;
    if (check(Identifier)) {
        auto typeToken = consume(Identifier);
		type = std::make_shared<Type>(typeToken.line, typeToken.column, typeToken.lexeme);
    } else {
		type = std::make_shared<Type>(-1, -1, "$void");
    }

	// Construct function signature
	auto ast = std::make_shared<FunctionSignature>(formals, type);
    return ast;
}

/**
 * Block ::= "{" { Statement } "}"
 */
std::shared_ptr<AST> Parser::block() {
    consume(LeftBracket, "block must begin with \"{\"");

    // Block body
	std::vector<std::shared_ptr<AST>> statements;
    while (!check(RightBracket)) {
        auto child = stmt();
        statements.push_back(child);
    }

    consume(RightBracket, "block must close with \"}\"");

	// Construct block
	auto ast = std::make_shared<Block>(statements);
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
std::shared_ptr<AST> Parser::stmt() {
    std::shared_ptr<AST> ast;

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
std::shared_ptr<AST> Parser::if_stmt() {
	// Must start with "if" keyword
    auto token = consume(If);

    // Condition
    auto condition = expr();

    // If body
    auto body = block();

    // Optional else/else-if
	std::shared_ptr<AST> branch;
    if (match(Else)) {
        // Else-if
        if (check(If)) {
			branch = if_stmt();
		}
		// Else
        else {
			auto else_body = block();
			branch = std::make_shared<class Else>(token.line, token.column, else_body);
		}
	}

	// Construct if
	auto ast = std::make_shared<class If>(token.line, token.column, condition, body, branch);
    return ast;
}

/**
 * ForStmt ::= "for" Expression block
 */
std::shared_ptr<AST> Parser::for_stmt() {
	// Must start with "for" keyword
    auto token = consume(For);

    // Optional condition
    std::shared_ptr<AST> condition;
    if (check(LeftBracket)) {
        condition = std::make_shared<Id>(-1, -1, "$true");
    } else {
        condition = expr();
    }

    // For body
    auto body = block();

	// Construct for
	auto ast = std::make_shared<class For>(token.line, token.column, condition, body);
    return ast;
}

/**
 * BreakStmt ::= "break"
 */
std::shared_ptr<AST> Parser::break_stmt() {
	// Must start with "break" keyword
    auto token = consume(Break);

	// Construct break
	auto ast = std::make_shared<class Break>(token.line, token.column);
	return ast;
}

/**
 * ReturnStmt ::= "return" [ Expression ]
 */
std::shared_ptr<AST> Parser::return_stmt() {
	// Must start with "return" keyword
    auto token = consume(Return);

    // Optional return expression
	std::shared_ptr<AST> value;
    if (!check(Semicolon)) {
        value = expr();
	}

	// Construct return
	auto ast = std::make_shared<class Return>(token.line, token.column, value);
    return ast;
}

/**
 * ExpressionStmt ::= Assignment
 */
std::shared_ptr<AST> Parser::expr_stmt() {
    auto ast = assignment();
    return ast;
}

/**
 * Assignment ::= Expr "=" Expr | Expr
 */
std::shared_ptr<AST> Parser::assignment() {
    auto l = expr();

    if (match(Equal)) {
        auto op = previous();
        auto r = expr();
        l = std::make_shared<Assignment>(op.line, op.column, l, r);
    }

    return l;
}

/**
 * Expression ::= OrExpr
 */
std::shared_ptr<AST> Parser::expr() {
    auto ast = or_expr();
    return ast;
}

/**
 * OrExpr ::= AndExpr { "||" AndExpr }
 */
std::shared_ptr<AST> Parser::or_expr() {
    auto l = and_expr();

    while (match(Or)) {
        auto op = previous();
        auto r = and_expr();
        l = std::make_shared<Binary>(op.line, op.column, op.lexeme, l, r);
    }

    return l;
}

/**
 * AndExpr ::= RelExpr { "&&" RelExpr }
 */
std::shared_ptr<AST> Parser::and_expr() {
    auto l = rel_expr();

    while (match(And)) {
        auto op = previous();
        auto r = rel_expr();
		l = std::make_shared<Binary>(op.line, op.column, op.lexeme, l, r);
    }

    return l;
}

/**
 * RelExpr ::= AddExpr { ("==" | "!=" | "<" | "<=" | ">" | ">=") AddExpr }
 */
std::shared_ptr<AST> Parser::rel_expr() {
    auto l = add_expr();

    while (match(EqualEqual) || match(NotEqual) || match(Less) ||
           match(LessEqual) || match(Greater) || match(GreaterEqual)) {
        auto op = previous();
        auto r = add_expr();
		l = std::make_shared<Binary>(op.line, op.column, op.lexeme, l, r);
    }

    return l;
}

/**
 * AddExpr ::= MulExpr { ("+" | "-") MulExpr }
 */
std::shared_ptr<AST> Parser::add_expr() {
    auto l = mul_expr();

    while (match(Add) || match(Subtract)) {
        auto op = previous();
        auto r = mul_expr();
		l = std::make_shared<Binary>(op.line, op.column, op.lexeme, l, r);
    }

    return l;
}

/**
 * MulExpr ::= UnaryExpr { ("*" | "/" | "%") UnaryExpr }
 */
std::shared_ptr<AST> Parser::mul_expr() {
    auto l = unary_expr();

    while (match(Multiply) || match(Divide) || match(Modulo)) {
        auto op = previous();
        auto r = unary_expr();
		l = std::make_shared<Binary>(op.line, op.column, op.lexeme, l, r);
    }

    return l;
}

/**
 * UnaryExpr ::= ("!" | "-") UnaryExpr | FuncCall
 */
std::shared_ptr<AST> Parser::unary_expr() {
    // TODO: Refactor this to be more concise if you are out of fun things to do in life :)
    if (match(Not)) {
        auto op = previous();
        auto r = unary_expr();
        return std::make_shared<Unary>(op.line, op.column, op.lexeme, r);
    }
    if (match(Subtract)) {
        // TODO: Hacky solution for negative integers
        if(match(Integer))
			return std::make_shared<Literal>(previous().line, previous().column - 1, "-" + previous().lexeme, Literal::Type::Int);
        auto op = previous();
        auto r = unary_expr();
		return std::make_shared<Unary>(op.line, op.column, "u" + op.lexeme, r);
    }

    return func_call();
}

/**
 * FuncCall ::= Operand | Operand "(" [ Expression { "," Expression } ] ")"
 */
std::shared_ptr<AST> Parser::func_call() {
    auto ast = operand();

	// Not a function without the paren
	if(!match(LeftParen)) {
		return ast;
	}

    // Arguments
	std::vector<std::shared_ptr<AST>> actuals;
	while (!match(RightParen)) {
		actuals.push_back(expr());
		if (!match(Comma)) {
			consume(RightParen, "function call must closed with \")\"");
			break;
		}
	}

	ast = std::make_shared<FunctionCall>(ast, actuals);
    return ast;
}

/**
 * Operand ::= int_lit | string_lit | identifier | ";" | "(" Expression ")"
 */
std::shared_ptr<AST> Parser::operand() {
    if (match(Integer))
        return std::make_shared<Literal>(previous().line, previous().column, previous().lexeme, Literal::Type::Int);

    if (match(String))
        return std::make_shared<Literal>(previous().line, previous().column, previous().lexeme, Literal::Type::String);

    if (match(Identifier))
        return std::make_shared<Id>(previous().line, previous().column, previous().lexeme);

    if (check(Semicolon))
        return std::make_shared<Empty>();


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
