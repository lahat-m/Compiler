#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "code_generator.h"

// External function declarations
extern ASTNode* load_annotated_ast(const char* filename);
extern int generate_assembly(CodeGenContext* ctx, ASTNode* ast, const char* output_file);

void print_header() {
    printf("CODE GENERATION\n");
    printf("Input:  annotated_ast.txt - from Phase 3\n");
    printf("Output: program.s (x86_64 Assembly)\n");
    printf("\n\n");
}

void print_input_file_info(const char* filename) {
    printf("INPUT FILE: %s\n", filename);
    printf("\n");
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("[ERROR: Cannot read annotated AST file]\n");
        printf("\n\n");
        return;
    }
    
    char line[256];
    int line_count = 0;
    int semantic_annotations = 0;
    int symbols_found = 0;
    
    printf("Annotated AST Preview:\n");
    printf("──────────────────────────\n");
    
    while (fgets(line, sizeof(line), file) && line_count < 10) {
        // Skip comments
        if (line[0] == '#') {
            continue;
        }
        
        // Remove newline for cleaner display
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) > 0) {
            printf(" %s\n", line);
            
            if (strstr(line, "Semantic_Type") || strstr(line, "Type_Check")) {
                semantic_annotations++;
            }
            if (strstr(line, "SYMBOL_REFERENCES") || strstr(line, "Variable")) {
                symbols_found++;
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
    
    if (total_lines > 10) {
        printf(" ... (truncated)\n");
    }
    
    printf("\n");
    printf("Total lines: %d\n", total_lines);
    printf("Semantic annotations: %d\n", semantic_annotations);
    printf("Symbol references: %d\n", symbols_found);
    printf("\n\n");
}

void print_compilation_options() {
    printf("COMPILATION OPTIONS\n");
    printf("\n");
    printf("Target Architecture: x86_64\n");
    printf("Assembler: GNU as (gas)\n");
    printf("Linker: GNU ld\n");
    printf("Output Format: ELF64\n");
    printf("\n");
    printf("Optimization Level: -O0 (none)\n");
    printf("Debug Information: Included\n");
    printf("Symbol Table: Generated\n");
    printf("\n\n");
}

void print_generated_code_info(const char* filename) {
    printf("GENERATED ASSEMBLY: %s\n", filename);
    printf("\n");
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Assembly file not generated\n");
        printf("\n\n");
        return;
    }
    
    char line[256];
    int total_lines = 0;
    int instructions = 0;
    int comments = 0;
    int labels = 0;
    
    printf("Assembly Code Preview:\n");
    printf("──────────────────────────\n");
    
    // Show first few lines
    int preview_lines = 0;
    while (fgets(line, sizeof(line), file) && preview_lines < 12) {
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) > 0) {
            printf(" %s\n", line);
            preview_lines++;
        }
    }
    
    // Count total content
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        total_lines++;
        if (strstr(line, "    ") && !strstr(line, "#")) {
            instructions++;
        }
        if (strstr(line, "#")) {
            comments++;
        }
        if (line[0] != ' ' && line[0] != '#' && line[0] != '\n' && strchr(line, ':')) {
            labels++;
        }
    }
    
    fclose(file);
    
    if (total_lines > 12) {
        printf(" ... (truncated)\n");
    }
    
    printf("\n");
    printf("Statistics:\n");
    printf("Total lines: %d\n", total_lines);
    printf("Instructions: %d\n", instructions);
    printf("Comments: %d\n", comments);
    printf("Labels: %d\n", labels);
    printf("\n");
    printf("\n\n");
}

void print_build_instructions() {
    printf("BUILD INSTRUCTIONS\n");
    printf("\n");
    printf(" To assemble and link the generated code:\n");
    printf("\n");
    printf(" 1. Assemble:  as -64 program.s -o program.o\n");
    printf(" 2. Link:      ld program.o -o program\n");
    printf(" 3. Run:       ./program\n");
    printf("\n");
    printf(" Alternative (GCC):\n");
    printf(" 1. Compile:   gcc program.s -o program\n");
    printf(" 2. Run:       ./program\n");
    printf("\n");
    printf("\n\n");
}

int main(int argc, char* argv[]) {
    print_header();
    
    // Determine input file
    const char* input_file = "annotated_ast.txt";  // Default
    if (argc > 1) {
        input_file = argv[1];
        printf("Using input file: %s\n\n", input_file);
    }
    
    // Check if input file exists
    if (access(input_file, F_OK) != 0) {
        printf("ERROR: %s not found!\n", input_file);
        if (argc == 1) {
            printf("Please run Phase 3 first to generate annotated_ast.txt\n");
        } else {
            printf("Please check the file path and try again\n");
        }
        printf("\n");
        return 1;
    }
    
    // Display input file information
    print_input_file_info(input_file);
    
    // Display compilation options
    print_compilation_options();
    
    // Load annotated AST
    ASTNode* ast = load_annotated_ast(input_file);
    if (!ast) {
        printf("PHASE 4 FAILED: Could not load annotated AST\n\n");
        return 1;
    }
    
    // Create code generation context
    CodeGenContext* ctx = create_codegen_context(TARGET_X86_64);
    if (!ctx) {
        printf("PHASE 4 FAILED: Could not create code generation context\n\n");
        free_ast_node(ast);
        return 1;
    }
    
    // Generate assembly code
    const char* output_file = "program.s";
    int result = generate_assembly(ctx, ast, output_file);
    
    if (result != 0) {
        printf("PHASE 4 FAILED: Code generation errors occurred\n\n");
        free_codegen_context(ctx);
        free_ast_node(ast);
        return 1;
    }
    
    // Display generated code information
    print_generated_code_info(output_file);
    
    // Count symbols
    int symbol_count = 0;
    struct SymbolMap* sym = ctx->symbol_map;
    while (sym) {
        symbol_count++;
        sym = sym->next;
    }
    
    // Print build instructions
    print_build_instructions();
    
    printf("Assembly code generation complete!\n");

    // Cleanup
    free_codegen_context(ctx);
    free_ast_node(ast);
    
    return 0;
}