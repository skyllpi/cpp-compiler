#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include <fstream>
#include <iostream>
#include <string>

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       (std::istreambuf_iterator<char>()));
    return content;
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }
    
    try {
        // Read source file
        std::string source = readFile(argv[1]);
        
        // Lexical analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Parsing
        Parser parser(tokens);
        auto ast = parser.parse();
        
        // Code generation
        CodeGenerator codegen;
        std::string assembly = codegen.generate(ast);
        
        // Write assembly to file
        std::string outputFile = std::string(argv[1]) + ".asm";
        writeFile(outputFile, assembly);
        
        std::cout << "Compilation successful. Assembly written to " << outputFile << std::endl;
        
        // Assemble and link
        std::string objectFile = std::string(argv[1]) + ".o";
        std::string executable = std::string(argv[1]);
        
        std::string assembleCmd = "nasm -f elf64 " + outputFile + " -o " + objectFile;
        std::string linkCmd = "ld " + objectFile + " -o " + executable;
        
        system(assembleCmd.c_str());
        system(linkCmd.c_str());
        
        std::cout << "Executable created: " << executable << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 