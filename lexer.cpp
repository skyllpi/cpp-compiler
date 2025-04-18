#include <stdio.h>
#include <iostream>
#include "lexer.h"
#include <cctype>
#include <unordered_map>

using namespace std;

// Keyword mapping
const std::unordered_map<std::string, TokenType> keywords = {
    {"let", TokenType::LET},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN}
};

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

char Lexer::current() const {
    return position < source.length() ? source[position] : '\0';
}

char Lexer::next() {
    if (position < source.length()) {
        char c = source[position++];
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return c;
    }
    return '\0';
}

void Lexer::skipWhitespace() {
    while (isspace(current())) {
        next();
    }
}

Token Lexer::readNumber() {
    std::string value;
    while (isdigit(current())) {
        value += current();
        next();
    }
    return Token(TokenType::NUMBER, value, line, column);
}

Token Lexer::readIdentifier() {
    std::string value;
    while (isalnum(current()) || current() == '_') {
        value += current();
        next();
    }
    
    // Check if it's a keyword
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return Token(it->second, value, line, column);
    }
    
    return Token(TokenType::IDENTIFIER, value, line, column);
}

Token Lexer::readOperator() {
    char c = current();
    next();
    
    switch (c) {
        case '+': return Token(TokenType::PLUS, "+", line, column);
        case '-': return Token(TokenType::MINUS, "-", line, column);
        case '*': return Token(TokenType::MULTIPLY, "*", line, column);
        case '/': return Token(TokenType::DIVIDE, "/", line, column);
        case '=': 
            if (current() == '=') {
                next();
                return Token(TokenType::EQUAL, "==", line, column);
            }
            return Token(TokenType::ASSIGN, "=", line, column);
        case '<': return Token(TokenType::LESS, "<", line, column);
        case '>': return Token(TokenType::GREATER, ">", line, column);
        case '(': return Token(TokenType::LPAREN, "(", line, column);
        case ')': return Token(TokenType::RPAREN, ")", line, column);
        case '{': return Token(TokenType::LBRACE, "{", line, column);
        case '}': return Token(TokenType::RBRACE, "}", line, column);
        case ';': return Token(TokenType::SEMICOLON, ";", line, column);
        case ',': return Token(TokenType::COMMA, ",", line, column);
        default: return Token(TokenType::INVALID, std::string(1, c), line, column);
    }
}

Token Lexer::getNextToken() {
    skipWhitespace();
    
    if (current() == '\0') {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }
    
    if (isdigit(current())) {
        return readNumber();
    }
    
    if (isalpha(current()) || current() == '_') {
        return readIdentifier();
    }
    
    return readOperator();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token = getNextToken();
    
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = getNextToken();
    }
    
    tokens.push_back(token); // Add EOF token
    return tokens;
}

int main(){
    cout<<"Hello world";
}