#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration to avoid circular dependency
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// AST Node types
typedef enum {
    // Expressions
    AST_IDENTIFIER,
    AST_BOOLEAN_LITERAL,
    AST_ASSIGNMENT,
    
    // Binary operations
    AST_AND,
    AST_OR,
    AST_XOR,
    AST_XNOR,
    AST_IMPLIES,
    AST_IFF,
    AST_EQUIV,
    
    // Unary operations
    AST_NOT,
    
    // Quantifiers
    AST_EXISTS,
    AST_FORALL,
    
    // Conditionals
    AST_IF,
    AST_IFF_STMT,
    
    // Program structure
    AST_PROGRAM,
    AST_STATEMENT_LIST,
    AST_EXPRESSION_STMT
} ASTNodeType;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;
    
    union {
        // For identifiers
        char* identifier;
        
        // For boolean literals
        int bool_value;
        
        // For binary operations
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        
        // For unary operations
        struct {
            struct ASTNode* operand;
        } unary;
        
        // For assignments
        struct {
            char* variable;
            struct ASTNode* value;
        } assignment;
        
        // For quantifiers
        struct {
            char* variable;
            struct ASTNode* expression;
        } quantifier;
        
        // For program/statement lists
        struct {
            struct ASTNode** statements;
            int count;
            int capacity;
        } program;
        
    } data;
    
    // Source location info
    int line_number;
    
} ASTNode;

// Function prototypes
ASTNode* create_identifier_node(const char* name, int line);
ASTNode* create_boolean_node(int value, int line);
ASTNode* create_binary_node(ASTNodeType type, ASTNode* left, ASTNode* right, int line);
ASTNode* create_unary_node(ASTNodeType type, ASTNode* operand, int line);
ASTNode* create_assignment_node(const char* variable, ASTNode* value, int line);
ASTNode* create_quantifier_node(ASTNodeType type, const char* variable, ASTNode* expression, int line);
ASTNode* create_program_node(int line);
ASTNode* create_expression_stmt_node(ASTNode* expression, int line);

void add_statement_to_program(ASTNode* program, ASTNode* statement);
void free_ast(ASTNode* node);

// AST printing functions
void print_ast(ASTNode* node, int indent);
void print_ast_to_file(ASTNode* node, const char* filename);
const char* ast_node_type_to_string(ASTNodeType type);

#endif // AST_H