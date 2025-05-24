#include "ast.h"
#include <string.h>

// Create identifier node
ASTNode* create_identifier_node(const char* name, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->data.identifier = strdup(name);
    node->line_number = line;
    return node;
}

// Create boolean literal node
ASTNode* create_boolean_node(int value, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BOOLEAN_LITERAL;
    node->data.bool_value = value;
    node->line_number = line;
    return node;
}

// Create binary operation node
ASTNode* create_binary_node(ASTNodeType type, ASTNode* left, ASTNode* right, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->data.binary.left = left;
    node->data.binary.right = right;
    node->line_number = line;
    return node;
}

// Create unary operation node
ASTNode* create_unary_node(ASTNodeType type, ASTNode* operand, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->data.unary.operand = operand;
    node->line_number = line;
    return node;
}

// Create assignment node
ASTNode* create_assignment_node(const char* variable, ASTNode* value, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->data.assignment.variable = strdup(variable);
    node->data.assignment.value = value;
    node->line_number = line;
    return node;
}

// Create quantifier node
ASTNode* create_quantifier_node(ASTNodeType type, const char* variable, ASTNode* expression, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->data.quantifier.variable = strdup(variable);
    node->data.quantifier.expression = expression;
    node->line_number = line;
    return node;
}

// Create program node
ASTNode* create_program_node(int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_PROGRAM;
    node->data.program.statements = malloc(sizeof(ASTNode*) * 10);
    node->data.program.count = 0;
    node->data.program.capacity = 10;
    node->line_number = line;
    return node;
}

// Create expression statement node
ASTNode* create_expression_stmt_node(ASTNode* expression, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_EXPRESSION_STMT;
    node->data.unary.operand = expression;
    node->line_number = line;
    return node;
}

// Add statement to program
void add_statement_to_program(ASTNode* program, ASTNode* statement) {
    if (program->type != AST_PROGRAM && program->type != AST_STATEMENT_LIST) {
        return;
    }
    
    if (program->data.program.count >= program->data.program.capacity) {
        program->data.program.capacity *= 2;
        program->data.program.statements = realloc(program->data.program.statements,
                                                 sizeof(ASTNode*) * program->data.program.capacity);
    }
    
    program->data.program.statements[program->data.program.count++] = statement;
}

// Free AST memory
void free_ast(ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_IDENTIFIER:
            free(node->data.identifier);
            break;
            
        case AST_ASSIGNMENT:
            free(node->data.assignment.variable);
            free_ast(node->data.assignment.value);
            break;
            
        case AST_AND:
        case AST_OR:
        case AST_XOR:
        case AST_XNOR:
        case AST_IMPLIES:
        case AST_IFF:
        case AST_EQUIV:
            free_ast(node->data.binary.left);
            free_ast(node->data.binary.right);
            break;
            
        case AST_NOT:
        case AST_EXPRESSION_STMT:
            free_ast(node->data.unary.operand);
            break;
            
        case AST_EXISTS:
        case AST_FORALL:
            free(node->data.quantifier.variable);
            free_ast(node->data.quantifier.expression);
            break;
            
        case AST_PROGRAM:
        case AST_STATEMENT_LIST:
            for (int i = 0; i < node->data.program.count; i++) {
                free_ast(node->data.program.statements[i]);
            }
            free(node->data.program.statements);
            break;
            
        default:
            break;
    }
    
    free(node);
}

// Convert AST node type to string
const char* ast_node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_BOOLEAN_LITERAL: return "BOOLEAN";
        case AST_ASSIGNMENT: return "ASSIGNMENT";
        case AST_AND: return "AND";
        case AST_OR: return "OR";
        case AST_XOR: return "XOR";
        case AST_XNOR: return "XNOR";
        case AST_NOT: return "NOT";
        case AST_IMPLIES: return "IMPLIES";
        case AST_IFF: return "IFF";
        case AST_EQUIV: return "EQUIV";
        case AST_EXISTS: return "EXISTS";
        case AST_FORALL: return "FORALL";
        case AST_IF: return "IF";
        case AST_IFF_STMT: return "IFF_STMT";
        case AST_PROGRAM: return "PROGRAM";
        case AST_STATEMENT_LIST: return "STMT_LIST";
        case AST_EXPRESSION_STMT: return "EXPR_STMT";
        default: return "UNKNOWN";
    }
}

// Print AST with indentation
void print_ast(ASTNode* node, int indent) {
    if (!node) {
        for (int i = 0; i < indent; i++) printf("  ");
        printf("(null)\n");
        return;
    }
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_IDENTIFIER:
            printf("IDENTIFIER: %s (line %d)\n", node->data.identifier, node->line_number);
            break;
            
        case AST_BOOLEAN_LITERAL:
            printf("BOOLEAN: %s (line %d)\n", 
                   node->data.bool_value ? "TRUE" : "FALSE", node->line_number);
            break;
            
        case AST_ASSIGNMENT:
            printf("ASSIGNMENT (line %d)\n", node->line_number);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Variable: %s\n", node->data.assignment.variable);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Value:\n");
            print_ast(node->data.assignment.value, indent + 2);
            break;
            
        case AST_AND:
        case AST_OR:
        case AST_XOR:
        case AST_XNOR:
        case AST_IMPLIES:
        case AST_IFF:
        case AST_EQUIV:
            printf("%s (line %d)\n", ast_node_type_to_string(node->type), node->line_number);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Left:\n");
            print_ast(node->data.binary.left, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Right:\n");
            print_ast(node->data.binary.right, indent + 2);
            break;
            
        case AST_NOT:
            printf("NOT (line %d)\n", node->line_number);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Operand:\n");
            print_ast(node->data.unary.operand, indent + 2);
            break;
            
        case AST_EXISTS:
        case AST_FORALL:
            printf("%s (line %d)\n", ast_node_type_to_string(node->type), node->line_number);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Variable: %s\n", node->data.quantifier.variable);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("Expression:\n");
            print_ast(node->data.quantifier.expression, indent + 2);
            break;
            
        case AST_PROGRAM:
            printf("PROGRAM (line %d) - %d statements\n", node->line_number, node->data.program.count);
            for (int i = 0; i < node->data.program.count; i++) {
                for (int j = 0; j < indent + 1; j++) printf("  ");
                printf("Statement %d:\n", i + 1);
                print_ast(node->data.program.statements[i], indent + 2);
            }
            break;
            
        case AST_EXPRESSION_STMT:
            printf("EXPRESSION_STMT (line %d)\n", node->line_number);
            print_ast(node->data.unary.operand, indent + 1);
            break;
            
        default:
            printf("UNKNOWN NODE TYPE\n");
            break;
    }
}

// Print AST to file
void print_ast_to_file(ASTNode* node, const char* filename) {
    FILE* old_stdout = stdout;
    FILE* file = fopen(filename, "w");
    
    if (!file) {
        fprintf(stderr, "Error: Cannot create AST output file %s\n", filename);
        return;
    }
    
    // Redirect stdout to file
    stdout = file;
    
    printf("# Abstract Syntax Tree (AST)\n");
    printf("# Generated by Phase 2: Syntax Analysis\n");
    printf("# Input: tokens.txt\n");
    printf("#\n\n");
    
    print_ast(node, 0);
    
    printf("\n# End of AST\n");
    
    // Restore stdout
    fclose(file);
    stdout = old_stdout;
}