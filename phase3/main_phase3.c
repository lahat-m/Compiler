#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "semantic_analyzer.h"

// External function declarations
extern ASTNode* load_ast_from_file(const char* filename);

void print_header() {
    printf("SEMANTIC ANALYSIS\n");
}

void print_input_file_info(const char* filename) {
    printf("INPUT FILE: %s\n", filename);
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf(" [ERROR: Cannot read AST file]\n");
        printf("\n\n");
        return;
    }
    
    char line[256];
    int line_count = 0;
    int node_count = 0;
    
    printf(" AST Preview:\n");
    printf(" ────────────────\n");
    
    while (fgets(line, sizeof(line), file) && line_count < 12) {
        // Skip comments
        if (line[0] == '#') {
            continue;
        }
        
        // Remove newline for cleaner display
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) > 0) {
            printf(" %s\n", line);
            if (strstr(line, "IDENTIFIER") || strstr(line, "ASSIGNMENT") || 
                strstr(line, "BOOLEAN") || strstr(line, "OR")) {
                node_count++;
            }
            line_count++;
        }
    }
    
    // Count total lines
    int total_lines = line_count;
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) > 1 && line[0] != '#') {
            total_lines++;
        }
    }
    
    fclose(file);
    
    if (total_lines > 12) {
        printf(" ... (truncated)\n");
    }
    
    printf("\n");
    printf(" Total lines: %d\n", total_lines);
    printf(" AST nodes detected: %d\n", node_count);
    printf("\n\n");
}

void print_output_files_info() {
    printf(" OUTPUT FILES GENERATED\n");
    
    // Check annotated_ast.txt
    if (access("annotated_ast.txt", F_OK) == 0) {
        FILE* file = fopen("annotated_ast.txt", "r");
        if (file) {
            char line[256];
            int lines = 0;
            while (fgets(line, sizeof(line), file)) lines++;
            fclose(file);
            printf("annotated_ast.txt (%d lines)\n", lines);
        }
    } else {
        printf("annotated_ast.txt (not created)\n");
    }
    
    // Check symbol_table.txt  
    if (access("symbol_table.txt", F_OK) == 0) {
        FILE* file = fopen("symbol_table.txt", "r");
        if (file) {
            char line[256];
            int lines = 0;
            while (fgets(line, sizeof(line), file)) lines++;
            fclose(file);
            printf(" symbol_table.txt (%d lines)\n", lines);
        }
    } else {
        printf("symbol_table.txt (not created)\n");
    }
    
    // Check semantic_errors.txt
    if (access("semantic_errors.txt", F_OK) == 0) {
        printf("semantic_errors.txt\n");
    }
    
    printf("\n\n");
}

void display_analysis_results(SemanticContext* ctx) {
    printf("ANALYSIS RESULTS\n");
    printf("\n");
    printf("Symbol Table:\n");
    printf("Total symbols: %d\n", ctx->symbol_table->count);
    printf("Defined symbols: %d\n", ctx->symbol_table->count - check_undefined_symbols(ctx->symbol_table));
    printf("Used symbols: %d\n", ctx->symbol_table->count - check_unused_symbols(ctx->symbol_table));
    printf("\n");
    printf("Semantic Validation:\n");
    
    if (ctx->error_count == 0) {
        printf("No errors found\n");
    } else {
        printf("%d errors found\n", ctx->error_count);
    }
    
    if (ctx->warning_count == 0) {
        printf("  No warnings\n");
    } else {
        printf("  %d warnings\n", ctx->warning_count);
    }
    
    printf("\n\n");
}

int main(int argc, char* argv[]) {
    print_header();
    
    // Determine input file
    const char* input_file = "ast.txt";  // Default
    if (argc > 1) {
        input_file = argv[1];
        printf("Using input file: %s\n\n", input_file);
    }
    
    // Check if input file exists
    if (access(input_file, F_OK) != 0) {
        printf("ERROR: %s not found!\n", input_file);
        if (argc == 1) {
            printf("   Please run Phase 2 first to generate ast.txt\n");
        } else {
            printf("   Please check the file path and try again\n");
        }
        printf("\n");
        return 1;
    }
    
    // Display input file information
    print_input_file_info(input_file);
    
    // Load AST from file
    ASTNode* ast = load_ast_from_file(input_file);
    if (!ast) {
        printf(" PHASE 3 Failed: Could not load AST from file\n\n");
        return 1;
    }
    
    // Create semantic analysis context
    SemanticContext* ctx = create_semantic_context();
    if (!ctx) {
        printf(" PHASE 3 Failed: Could not create semantic context\n\n");
        free_ast_node(ast);
        return 1;
    }
    
    // Perform semantic analysis
    int analysis_result = perform_semantic_analysis(ctx, ast);
    
    // Display symbol table
    print_symbol_table(ctx->symbol_table);
    
    // Display semantic errors
    print_semantic_errors(ctx);
    
    // Display analysis results summary
    display_analysis_results(ctx);
    
    // Generate output files
    printf(" GENERATING OUTPUT FILES\n");
    printf(" Creating annotated_ast.txt...\n");
    generate_annotated_ast(ctx, ast, "annotated_ast.txt");
    printf(" Annotated AST generated\n");
    printf("\n");
    printf(" Creating symbol_table.txt...\n");
    print_symbol_table_to_file(ctx->symbol_table, "symbol_table.txt");
    printf(" Symbol table exported\n");
    printf("\n");
    printf(" Creating semantic_errors.txt...\n");
    print_semantic_errors_to_file(ctx, "semantic_errors.txt");
    printf(" Semantic errors logged\n");
    printf("\n\n");
    
    // Display output files info
    print_output_files_info();
    
    // Cleanup
    free_semantic_context(ctx);
    free_ast_node(ast);
    
    return analysis_result == 0 ? 0 : 1;
}