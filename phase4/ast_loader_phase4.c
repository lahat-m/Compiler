#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "code_generator.h"
#include <ctype.h>

// Helper function to trim whitespace
char* trim_whitespace(char* str) {
    char* end;
    
    // Trim leading space
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

// Parse node type from string
int parse_node_type(const char* type_str) {
    if (strstr(type_str, "PROGRAM")) return 1;
    if (strstr(type_str, "ASSIGNMENT")) return 2;
    if (strstr(type_str, "EXPRESSION_STMT")) return 3;
    if (strstr(type_str, "IDENTIFIER")) return 4;
    if (strstr(type_str, "BOOLEAN")) return 5;
    if (strstr(type_str, "AND")) return 6;
    if (strstr(type_str, "OR")) return 7;
    if (strstr(type_str, "NOT")) return 8;
    if (strstr(type_str, "XOR")) return 9;
    if (strstr(type_str, "IMPLIES")) return 10;
    if (strstr(type_str, "IFF")) return 11;
    if (strstr(type_str, "EQUIV")) return 12;
    return 0; // Unknown
}

// Create AST node
ASTNode* create_ast_node(int type, const char* type_str, int line) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    node->node_type_str = strdup(type_str);
    node->line_number = line;
    
    // Initialize data union
    memset(&node->data, 0, sizeof(node->data));
    
    return node;
}

// Extract assignment information from annotated AST
void parse_assignment_info(ASTNode* node, FILE* file) {
    char line[512];
    
    // Look for Variable: line in assignment
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim_whitespace(line);
        
        if (strstr(trimmed, "Variable:") && node->data.assignment.variable == NULL) {
            char* var_start = strstr(trimmed, "Variable:");
            if (var_start) {
                var_start += 9; // Skip "Variable:"
                var_start = trim_whitespace(var_start);
                node->data.assignment.variable = strdup(var_start);
            }
        }
        
        if (strstr(trimmed, "Value:") || strstr(trimmed, "BOOLEAN:")) {
            // Look for boolean value
            if (strstr(trimmed, "TRUE")) {
                ASTNode* value_node = create_ast_node(5, "BOOLEAN", node->line_number);
                value_node->data.bool_literal = 1;
                node->data.assignment.value = value_node;
                break;
            } else if (strstr(trimmed, "FALSE")) {
                ASTNode* value_node = create_ast_node(5, "BOOLEAN", node->line_number);
                value_node->data.bool_literal = 0;
                node->data.assignment.value = value_node;
                break;
            }
        }
        
        // Stop if we hit next statement
        if (strstr(trimmed, "Statement_") || strstr(trimmed, "SEMANTIC_SUMMARY")) {
            fseek(file, -strlen(line), SEEK_CUR);
            break;
        }
    }
}

// Load annotated AST from file
ASTNode* load_annotated_ast(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open annotated AST file %s\n", filename);
        return NULL;
    }
    
    printf("LOADING ANNOTATED AST FROM: %s\n", filename);
    printf("\n");
    
    char line[512];
    ASTNode* root = NULL;
    int nodes_loaded = 0;
    int assignments_found = 0;
    int expressions_found = 0;
    
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = trim_whitespace(line);
        
        // Skip comments and empty lines
        if (trimmed[0] == '#' || strlen(trimmed) == 0) {
            continue;
        }
        
        // Look for main program node
        if (strstr(trimmed, "ANNOTATED_PROGRAM:") && !root) {
            root = create_ast_node(1, "PROGRAM", 1);
            root->data.program.statements = malloc(sizeof(ASTNode*) * 10);
            root->data.program.count = 0;
            nodes_loaded++;
            printf("Found PROGRAM node\n");
            continue;
        }
        
        // Look for statement nodes
        if (strstr(trimmed, "Statement_") && strstr(trimmed, ":")) {
            // Read next few lines to determine statement type
            long pos = ftell(file);
            char next_line[512];
            
            if (fgets(next_line, sizeof(next_line), file)) {
                if (strstr(next_line, "ASSIGNMENT")) {
                    // Create assignment node
                    ASTNode* assign_node = create_ast_node(2, "ASSIGNMENT", 1);
                    parse_assignment_info(assign_node, file);
                    
                    if (root && root->data.program.count < 10) {
                        root->data.program.statements[root->data.program.count++] = assign_node;
                        assignments_found++;
                        printf("Found ASSIGNMENT: %s\n", 
                               assign_node->data.assignment.variable ? 
                               assign_node->data.assignment.variable : "unknown");
                    }
                } else if (strstr(next_line, "EXPRESSION_STMT")) {
                    // Create expression statement node
                    ASTNode* expr_node = create_ast_node(3, "EXPRESSION_STMT", 3);
                    
                    if (root && root->data.program.count < 10) {
                        root->data.program.statements[root->data.program.count++] = expr_node;
                        expressions_found++;
                        printf("Found EXPRESSION_STMT\n");
                    }
                } else {
                    fseek(file, pos, SEEK_SET);
                }
            } else {
                fseek(file, pos, SEEK_SET);
            }
            
            nodes_loaded++;
        }
    }
    
    fclose(file);
    
    printf("AST loaded\n");
    printf("Total nodes: %d\n", nodes_loaded);
    printf("Assignments: %d\n", assignments_found);
    printf("Expressions: %d\n", expressions_found);
    printf("\n\n");
    
    return root;
}

// Free AST node and its children
void free_ast_node(ASTNode* node) {
    if (!node) return;
    
    // Free node-specific data
    if (node->node_type_str) {
        free(node->node_type_str);
    }
    
    if (node->type == 4 && node->data.identifier) {  // IDENTIFIER
        free(node->data.identifier);
    } else if (node->type == 2) {  // ASSIGNMENT
        if (node->data.assignment.variable) {
            free(node->data.assignment.variable);
        }
        if (node->data.assignment.value) {
            free_ast_node(node->data.assignment.value);
        }
    } else if (node->type == 1) {  // PROGRAM
        if (node->data.program.statements) {
            for (int i = 0; i < node->data.program.count; i++) {
                free_ast_node(node->data.program.statements[i]);
            }
            free(node->data.program.statements);
        }
    }
    
    free(node);
}