#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), current(0) {}

Token& Parser::peek() {
    return tokens[current];
}

Token& Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const std::string& message) {
    throw std::runtime_error(message + " at line " + std::to_string(peek().line));
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(statement());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::LET)) return letStatement();
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::LBRACE)) return blockStatement();
    return std::make_unique<ExprStmt>(expression());
}

std::unique_ptr<Stmt> Parser::letStatement() {
    Token name = advance();
    if (name.type != TokenType::IDENTIFIER) {
        error("Expected identifier after 'let'");
    }
    
    if (!match(TokenType::ASSIGN)) {
        error("Expected '=' after identifier");
    }
    
    auto value = expression();
    if (!match(TokenType::SEMICOLON)) {
        error("Expected ';' after value");
    }
    
    return std::make_unique<LetStmt>(name.value, std::move(value));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    if (!match(TokenType::LPAREN)) {
        error("Expected '(' after 'if'");
    }
    
    auto condition = expression();
    
    if (!match(TokenType::RPAREN)) {
        error("Expected ')' after condition");
    }
    
    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    if (!match(TokenType::LPAREN)) {
        error("Expected '(' after 'while'");
    }
    
    auto condition = expression();
    
    if (!match(TokenType::RPAREN)) {
        error("Expected ')' after condition");
    }
    
    auto body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::blockStatement() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        statements.push_back(statement());
    }
    
    if (!match(TokenType::RBRACE)) {
        error("Expected '}' after block");
    }
    
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    
    while (match(TokenType::EQUAL)) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    
    while (match(TokenType::LESS) || match(TokenType::GREATER)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = primary();
    
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE)) {
        Token op = previous();
        auto right = primary();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match(TokenType::NUMBER)) {
        return std::make_unique<NumberExpr>(std::stoi(previous().value));
    }
    
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<IdentifierExpr>(previous().value);
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = expression();
        if (!match(TokenType::RPAREN)) {
            error("Expected ')' after expression");
        }
        return expr;
    }
    
    error("Expected expression");
    return nullptr;
} 