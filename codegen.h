#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <string>
#include <vector>
#include <unordered_map>

class CodeGenerator {
private:
    std::vector<std::string> assembly;
    std::unordered_map<std::string, int> variables;
    int labelCounter;
    
    void generateExpr(const Expr& expr);
    void generateStmt(const Stmt& stmt);
    
    void generateNumberExpr(const NumberExpr& expr);
    void generateIdentifierExpr(const IdentifierExpr& expr);
    void generateBinaryExpr(const BinaryExpr& expr);
    
    void generateLetStmt(const LetStmt& stmt);
    void generateExprStmt(const ExprStmt& stmt);
    void generateBlockStmt(const BlockStmt& stmt);
    void generateIfStmt(const IfStmt& stmt);
    void generateWhileStmt(const WhileStmt& stmt);
    
    std::string newLabel();
    
public:
    CodeGenerator();
    std::string generate(const std::vector<std::unique_ptr<Stmt>>& ast);
};

#endif // CODEGEN_H 