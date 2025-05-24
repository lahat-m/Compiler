#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "symbol_table.h"

// Semantic error types
typedef enum {
    SEM_ERROR_UNDEFINED_VAR,
    SEM_ERROR_TYPE_MISMATCH,
    SEM_ERROR_UNUSED_VAR,
    SEM_ERROR_REDEFINITION,
    SEM_ERROR_INVALID_OPERATION
} SemanticErrorType;

// Semantic error structure
typedef struct SemanticError {
    SemanticErrorType type;
    char* message;
    int line_number;
    char* symbol_name;
    struct SemanticError* next;
} SemanticError;

// Semantic analysis context
typedef struct {
    SymbolTable* symbol_table;
    SemanticError* errors;
    int error_count;
    int warning_count;
} SemanticContext;

// AST Node structure (redefined for semantic analysis)
typedef struct ASTNode {
    int type;  // AST node type (from original AST)
    char* node_type_str;  // String representation
    int line_number;
    
    // Semantic attributes
    SymbolType semantic_type;
    int is_constant;
    int bool_value;  // For constant boolean expressions
    
    // Node data
    union {
        char* identifier;
        int bool_literal;
        struct {
            char* variable;
            struct ASTNode* value;
        } assignment;
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {
            struct ASTNode* operand;
        } unary;
        struct {
            struct ASTNode** statements;
            int count;
        } program;
    } data;
    
    struct ASTNode* parent;
    struct ASTNode** children;
    int child_count;
} ASTNode;

// Function prototypes
SemanticContext* create_semantic_context(void);
void free_semantic_context(SemanticContext* ctx);

// AST loading from file
ASTNode* load_ast_from_file(const char* filename);
void free_ast_node(ASTNode* node);

// Semantic analysis functions
int perform_semantic_analysis(SemanticContext* ctx, ASTNode* ast);
void analyze_node(SemanticContext* ctx, ASTNode* node);
void analyze_assignment(SemanticContext* ctx, ASTNode* node);
void analyze_expression(SemanticContext* ctx, ASTNode* node);
void analyze_binary_operation(SemanticContext* ctx, ASTNode* node);
void analyze_identifier(SemanticContext* ctx, ASTNode* node);

// Error handling
void add_semantic_error(SemanticContext* ctx, SemanticErrorType type, 
                       const char* message, int line, const char* symbol);
void print_semantic_errors(SemanticContext* ctx);
void print_semantic_errors_to_file(SemanticContext* ctx, const char* filename);

// Output generation
void generate_annotated_ast(SemanticContext* ctx, ASTNode* ast, const char* filename);
void print_semantic_summary(SemanticContext* ctx);

// Utility functions
const char* semantic_error_type_to_string(SemanticErrorType type);
SymbolType infer_expression_type(SemanticContext* ctx, ASTNode* node);

#endif // SEMANTIC_ANALYZER_H