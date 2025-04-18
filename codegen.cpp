#include "codegen.h"
#include <sstream>

CodeGenerator::CodeGenerator() : labelCounter(0) {
    // Initialize assembly with prologue
    assembly.push_back("section .text");
    assembly.push_back("global _start");
    assembly.push_back("_start:");
    assembly.push_back("    push rbp");
    assembly.push_back("    mov rbp, rsp");
}

std::string CodeGenerator::newLabel() {
    return "L" + std::to_string(labelCounter++);
}

void CodeGenerator::generateExpr(const Expr& expr) {
    if (auto num = dynamic_cast<const NumberExpr*>(&expr)) {
        generateNumberExpr(*num);
    } else if (auto id = dynamic_cast<const IdentifierExpr*>(&expr)) {
        generateIdentifierExpr(*id);
    } else if (auto bin = dynamic_cast<const BinaryExpr*>(&expr)) {
        generateBinaryExpr(*bin);
    }
}

void CodeGenerator::generateNumberExpr(const NumberExpr& expr) {
    assembly.push_back("    push " + std::to_string(expr.value));
}

void CodeGenerator::generateIdentifierExpr(const IdentifierExpr& expr) {
    auto it = variables.find(expr.name);
    if (it == variables.end()) {
        throw std::runtime_error("Undefined variable: " + expr.name);
    }
    assembly.push_back("    push QWORD [rbp - " + std::to_string(it->second * 8) + "]");
}

void CodeGenerator::generateBinaryExpr(const BinaryExpr& expr) {
    generateExpr(*expr.left);
    generateExpr(*expr.right);
    
    assembly.push_back("    pop rbx");
    assembly.push_back("    pop rax");
    
    switch (expr.op) {
        case TokenType::PLUS:
            assembly.push_back("    add rax, rbx");
            break;
        case TokenType::MINUS:
            assembly.push_back("    sub rax, rbx");
            break;
        case TokenType::MULTIPLY:
            assembly.push_back("    imul rax, rbx");
            break;
        case TokenType::DIVIDE:
            assembly.push_back("    cqo");
            assembly.push_back("    idiv rbx");
            break;
        case TokenType::EQUAL:
            assembly.push_back("    cmp rax, rbx");
            assembly.push_back("    sete al");
            assembly.push_back("    movzx rax, al");
            break;
        case TokenType::LESS:
            assembly.push_back("    cmp rax, rbx");
            assembly.push_back("    setl al");
            assembly.push_back("    movzx rax, al");
            break;
        case TokenType::GREATER:
            assembly.push_back("    cmp rax, rbx");
            assembly.push_back("    setg al");
            assembly.push_back("    movzx rax, al");
            break;
        default:
            throw std::runtime_error("Unsupported binary operator");
    }
    
    assembly.push_back("    push rax");
}

void CodeGenerator::generateStmt(const Stmt& stmt) {
    if (auto let = dynamic_cast<const LetStmt*>(&stmt)) {
        generateLetStmt(*let);
    } else if (auto expr = dynamic_cast<const ExprStmt*>(&stmt)) {
        generateExprStmt(*expr);
    } else if (auto block = dynamic_cast<const BlockStmt*>(&stmt)) {
        generateBlockStmt(*block);
    } else if (auto ifStmt = dynamic_cast<const IfStmt*>(&stmt)) {
        generateIfStmt(*ifStmt);
    } else if (auto whileStmt = dynamic_cast<const WhileStmt*>(&stmt)) {
        generateWhileStmt(*whileStmt);
    }
}

void CodeGenerator::generateLetStmt(const LetStmt& stmt) {
    generateExpr(*stmt.value);
    
    int offset = variables.size() + 1;
    variables[stmt.name] = offset;
    
    assembly.push_back("    pop QWORD [rbp - " + std::to_string(offset * 8) + "]");
}

void CodeGenerator::generateExprStmt(const ExprStmt& stmt) {
    generateExpr(*stmt.expr);
    assembly.push_back("    pop rax"); // Discard result
}

void CodeGenerator::generateBlockStmt(const BlockStmt& stmt) {
    for (const auto& s : stmt.statements) {
        generateStmt(*s);
    }
}

void CodeGenerator::generateIfStmt(const IfStmt& stmt) {
    std::string elseLabel = newLabel();
    std::string endLabel = newLabel();
    
    generateExpr(*stmt.condition);
    assembly.push_back("    pop rax");
    assembly.push_back("    test rax, rax");
    assembly.push_back("    jz " + elseLabel);
    
    generateStmt(*stmt.thenBranch);
    assembly.push_back("    jmp " + endLabel);
    
    assembly.push_back(elseLabel + ":");
    if (stmt.elseBranch) {
        generateStmt(*stmt.elseBranch);
    }
    
    assembly.push_back(endLabel + ":");
}

void CodeGenerator::generateWhileStmt(const WhileStmt& stmt) {
    std::string startLabel = newLabel();
    std::string endLabel = newLabel();
    
    assembly.push_back(startLabel + ":");
    generateExpr(*stmt.condition);
    assembly.push_back("    pop rax");
    assembly.push_back("    test rax, rax");
    assembly.push_back("    jz " + endLabel);
    
    generateStmt(*stmt.body);
    assembly.push_back("    jmp " + startLabel);
    
    assembly.push_back(endLabel + ":");
}

std::string CodeGenerator::generate(const std::vector<std::unique_ptr<Stmt>>& ast) {
    // Generate code for each statement
    for (const auto& stmt : ast) {
        generateStmt(*stmt);
    }
    
    // Add epilogue
    assembly.push_back("    mov rsp, rbp");
    assembly.push_back("    pop rbp");
    assembly.push_back("    mov rax, 60");
    assembly.push_back("    xor rdi, rdi");
    assembly.push_back("    syscall");
    
    // Combine all assembly lines
    std::stringstream ss;
    for (const auto& line : assembly) {
        ss << line << "\n";
    }
    
    return ss.str();
} 