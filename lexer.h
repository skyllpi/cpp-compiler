#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <iostream>

// Token types for our minimal language
enum class TokenType {
    // Keywords
    LET,
    IF,
    ELSE,
    WHILE,
    RETURN,
    
    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    EQUAL,
    LESS,
    GREATER,
    
    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    COMMA,
    
    // Literals
    IDENTIFIER,
    NUMBER,
    
    // Special
    END_OF_FILE,
    INVALID
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    char current() const;
    char next();
    void skipWhitespace();
    Token readNumber();
    Token readIdentifier();
    Token readOperator();
    
public:
    Lexer(const std::string& source);
    Token getNextToken();
    std::vector<Token> tokenize();
};

#endif // LEXER_H
