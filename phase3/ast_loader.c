#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "semantic_analyzer.h"

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
    node->semantic_type = SYM_UNKNOWN;
    node->is_constant = 0;
    node->bool_value = 0;
    node->parent = NULL;
    node->children = NULL;
    node->child_count = 0;
    
    // Initialize data union
    memset(&node->data, 0, sizeof(node->data));
    
    return node;
}

// Parse line and extract information
int parse_ast_line(char* line, char** node_info, int* indent_level) {
    *indent_level = 0;
    char* start = line;
    
    // Count leading spaces for indentation
    while (*start == ' ') {
        (*indent_level)++;
        start++;
    }
    *indent_level /= 2;  // Assume 2 spaces per indent level
    
    *node_info = trim_whitespace(start);
    return strlen(*node_info) > 0;
}

// Extract value from node info (for identifiers and booleans)
void extract_node_data(ASTNode* node, const char* info) {
    if (node->type == 4) {  // IDENTIFIER
        char* colon = strchr(info, ':');
        if (colon) {
            char* name_start = colon + 1;
            while (*name_start == ' ') name_start++;
            char* name_end = strchr(name_start, ' ');
            if (name_end) {
                int len = name_end - name_start;
                node->data.identifier = malloc(len + 1);
                strncpy(node->data.identifier, name_start, len);
                node->data.identifier[len] = '\0';
            } else {
                node->data.identifier = strdup(name_start);
            }
        }
    } else if (node->type == 5) {  // BOOLEAN
        if (strstr(info, "TRUE")) {
            node->data.bool_literal = 1;
            node->is_constant = 1;
            node->bool_value = 1;
        } else if (strstr(info, "FALSE")) {
            node->data.bool_literal = 0;
            node->is_constant = 1;
            node->bool_value = 0;
        }
    } else if (node->type == 2) {  // ASSIGNMENT
        // For assignments, we'll need to parse variable name from subsequent lines
        node->data.assignment.variable = NULL;
        node->data.assignment.value = NULL;
    }
}

// Simplified AST loader that creates a basic structure
ASTNode* load_ast_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open AST file %s\n", filename);
        return NULL;
    }
    
    printf("LOADING AST FROM: %s\n", filename);
    printf("\n");
    
    char line[512];
    ASTNode* root = NULL;
    ASTNode* current_program = NULL;
    int nodes_loaded = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || strlen(trim_whitespace(line)) == 0) {
            continue;
        }
        
        char* node_info;
        int indent_level;
        
        if (parse_ast_line(line, &node_info, &indent_level)) {
            // Extract line number if present
            int line_num = 1;
            char* line_marker = strstr(node_info, "(line ");
            if (line_marker) {
                sscanf(line_marker, "(line %d)", &line_num);
            }
            
            int node_type = parse_node_type(node_info);
            if (node_type > 0) {
                ASTNode* node = create_ast_node(node_type, node_info, line_num);
                extract_node_data(node, node_info);
                
                if (node_type == 1 && !root) {  // First PROGRAM node
                    root = node;
                    current_program = node;
                    // Initialize program data
                    node->data.program.statements = malloc(sizeof(ASTNode*) * 10);
                    node->data.program.count = 0;
                } else if (node_type == 2 || node_type == 3) {  // ASSIGNMENT or EXPRESSION_STMT
                    if (current_program && current_program->data.program.count < 10) {
                        current_program->data.program.statements[current_program->data.program.count++] = node;
                        node->parent = current_program;
                    }
                }
                
                nodes_loaded++;
            }
        }
    }
    
    fclose(file);
    
    printf("AST loaded successfully\n");
    printf("Nodes processed: %d\n", nodes_loaded);
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
    
    // Free children array
    if (node->children) {
        for (int i = 0; i < node->child_count; i++) {
            free_ast_node(node->children[i]);
        }
        free(node->children);
    }
    
    free(node);
}