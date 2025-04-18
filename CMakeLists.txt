cmake_minimum_required(VERSION 3.10)
project(SimpleCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    main.cpp
    lexer.cpp
    parser.cpp
    codegen.cpp
)

# Add header files
set(HEADERS
    lexer.h
    parser.h
    codegen.h
)

# Create executable
add_executable(compiler ${SOURCES} ${HEADERS})

# Set output name
set_target_properties(compiler PROPERTIES OUTPUT_NAME "simplec") 