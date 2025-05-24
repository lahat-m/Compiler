#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ast.h"
#include "parser.tab.h"  // This will contain token definitions

// External declarations
extern ASTNode* ast_root;
extern int parse_tokens_from_file(const char* filename);

void print_header() {
    printf("ROADMAP COMPILER - PHASE 2\n");
    printf("SYNTAX ANALYSIS\n");
    printf(" Input:  tokens.txt (from Phase 1)\n");
    printf(" Output: ast.txt (Abstract Syntax Tree)\n");
    printf("\n\n");
}

void print_token_file_info(const char* filename) {
    printf(" INPUT FILE: %s\n", filename);
    printf("\n");
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf(" [ERROR: Cannot read token file]\n");
        printf("\n\n");
        return;
    }
    
    char line[256];
    int token_count = 0;
    int line_num = 1;
    
    printf(" Tokens Preview:\n");
    printf(" ────────────────────────────────────────\n");
    
    while (fgets(line, sizeof(line), file) && line_num <= 10) {
        // Skip comments
        if (line[0] == '#') {
            line_num++;
            continue;
        }
        
        // Check for EOF
        if (strncmp(line, "EOF", 3) == 0) {
            break;
        }
        
        // Remove newline for cleaner display
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) > 0) {
            printf(" %2d: %s\n", token_count + 1, line);
            token_count++;
        }
        line_num++;
    }
    
    // Count remaining tokens
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#' && strlen(line) > 1 && strncmp(line, "EOF", 3) != 0) {
            token_count++;
        }
    }
    
    fclose(file);
    
    if (token_count > 10) {
        printf(" ... and %d more tokens\n", token_count - 10);
    }
    
    printf(" Total tokens: %d\n", token_count);
    printf("\n\n");
}

void print_ast_summary(ASTNode* root) {
    if (!root) {
        printf(" AST GENERATION\n");
        printf(" No AST generated\n");
        printf("\n\n");
        return;
    }
    
    printf(" AST STRUCTURE OVERVIEW\n");
    printf("\n");
    
    switch (root->type) {
        case AST_PROGRAM:
            printf("Root: PROGRAM node\n");
            printf("Statements: %d\n", root->data.program.count);
            
            // Analyze statement types
            int assignments = 0, expressions = 0;
            for (int i = 0; i < root->data.program.count; i++) {
                ASTNode* stmt = root->data.program.statements[i];
                if (stmt->type == AST_ASSIGNMENT) {
                    assignments++;
                } else if (stmt->type == AST_EXPRESSION_STMT) {
                    expressions++;
                }
            }
            
            printf("Assignments: %d\n", assignments);
            printf("Expressions: %d\n", expressions);
            break;
            
        default:
            printf("Root: %s node\n", ast_node_type_to_string(root->type));
            break;
    }
    
    printf("AST successfully constructed\n");
    printf("\n\n");
}

void print_output_file_info(const char* filename) {
    printf("OUTPUT FILE: %s\n", filename);


    FILE* file = fopen(filename, "r");
    if (!file) {
        printf(" File not created\n");
        printf("\n\n");
        return;
    }
    
    // Count lines in AST file
    char line[256];
    int line_count = 0;
    while (fgets(line, sizeof(line), file)) {
        line_count++;
    }
    fclose(file);
    
    printf("Format: Hierarchical tree structure\n");
    printf("Lines: %d\n", line_count);
    printf("\n\n");
}


void display_sample_ast(ASTNode* root) {
    printf(" AST SAMPLE\n");
    
    if (!root) {
        printf(" (No AST to display)\n");
        printf("\n\n");
        return;
    }
    
    // Redirect to a string buffer for limited display
    FILE* old_stdout = stdout;
    FILE* temp_file = tmpfile();
    if (!temp_file) {
        printf(" (Cannot display AST sample)\n");
        printf("\n\n");
        return;
    }
    
    stdout = temp_file;
    print_ast(root, 0);
    stdout = old_stdout;
    
    // Read back and display first 15 lines
    rewind(temp_file);
    char line[256];
    int line_count = 0;
    
    while (fgets(line, sizeof(line), temp_file) && line_count < 15) {
        printf(" %s", line);
        line_count++;
    }
    
    if (line_count >= 15) {
        printf(" ... (truncated, see ast.txt for complete tree)\n");
    }
    
    fclose(temp_file);
    printf("\n\n");
}

int main(int argc, char* argv[]) {
    print_header();
    
    // Determine input file
    const char* input_file = "tokens.txt";  // Default
    if (argc > 1) {
        input_file = argv[1];
        printf("Using input file: %s\n\n", input_file);
    }
    
    // Check if input file exists
    if (access(input_file, F_OK) != 0) {
        printf("ERROR: %s not found!\n", input_file);
        if (argc == 1) {
            printf("   Please run Phase 1 first to generate tokens.txt\n");
        } else {
            printf("   Please check the file path and try again\n");
        }
        printf("\n");
        return 1;
    }
    
    // Display input file information
    print_token_file_info(input_file);
    
    // Parse tokens and build AST
    int parse_result = parse_tokens_from_file(input_file);
    
    if (parse_result != 0) {
        printf("PHASE 2 FAILED: Parsing errors occurred\n\n");
        return 1;
    }
    
    if (!ast_root) {
        printf("PHASE 2 FAILED: No AST generated\n\n");
        return 1;
    }
    
    // Display AST summary
    print_ast_summary(ast_root);
    
    // Generate AST output file
    printf(" GENERATING AST\n");
    printf(" Writing AST to ast.txt...\n");
    
    print_ast_to_file(ast_root, "ast.txt");
    
    printf("ast.txt created successfully\n");
    printf("\n");
    printf("\n\n");
    
    // Display output file info
    print_output_file_info("ast.txt");
    
    // Show sample of AST
    display_sample_ast(ast_root);
    
    // Cleanup
    free_ast(ast_root);
    
    return 0;
}