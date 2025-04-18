#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <memory>
#include <vector>

// Forward declarations
class Expr;
class Stmt;

// Expression types
class Expr {
public:
    virtual ~Expr() = default;
};

class NumberExpr : public Expr {
public:
    int value;
    NumberExpr(int value) : value(value) {}
};

class IdentifierExpr : public Expr {
public:
    std::string name;
    IdentifierExpr(const std::string& name) : name(name) {}
};

class BinaryExpr : public Expr {
public:
    TokenType op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    
    BinaryExpr(TokenType op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

// Statement types
class Stmt {
public:
    virtual ~Stmt() = default;
};

class LetStmt : public Stmt {
public:
    std::string name;
    std::unique_ptr<Expr> value;
    
    LetStmt(const std::string& name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}
};

class ExprStmt : public Stmt {
public:
    std::unique_ptr<Expr> expr;
    
    ExprStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
};

class BlockStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
    
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
           std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}
};

class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
    
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token& peek();
    Token& previous();
    bool isAtEnd();
    Token& advance();
    bool check(TokenType type);
    bool match(TokenType type);
    void error(const std::string& message);
    
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> primary();
    
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> letStatement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> blockStatement();
    
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();
};

#endif // PARSER_H 