#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Target architecture
typedef enum {
    TARGET_X86_64,
    TARGET_ARM64,
    TARGET_MIPS
} TargetArch;

// Register allocation
typedef enum {
    REG_RAX = 0,    // Accumulator
    REG_RBX,        // Base
    REG_RCX,        // Counter
    REG_RDX,        // Data
    REG_RSI,        // Source Index
    REG_RDI,        // Destination Index
    REG_R8,         // General purpose
    REG_R9,         // General purpose
    REG_COUNT
} Register;

// Assembly instruction types
typedef enum {
    INST_MOV,       // Move
    INST_ADD,       // Add
    INST_SUB,       // Subtract
    INST_CMP,       // Compare
    INST_JMP,       // Jump
    INST_JE,        // Jump if equal
    INST_JNE,       // Jump if not equal
    INST_CALL,      // Function call
    INST_RET,       // Return
    INST_PUSH,      // Push to stack
    INST_POP,       // Pop from stack
    INST_OR,        // Bitwise OR
    INST_AND,       // Bitwise AND
    INST_XOR,       // Bitwise XOR
    INST_NOT,       // Bitwise NOT
    INST_TEST,      // Test (AND without storing result)
    INST_LABEL      // Label definition
} InstructionType;

// Operand types
typedef enum {
    OPERAND_REGISTER,
    OPERAND_IMMEDIATE,
    OPERAND_MEMORY,
    OPERAND_LABEL
} OperandType;

// Assembly operand
typedef struct {
    OperandType type;
    union {
        Register reg;
        int immediate;
        char* label;
        struct {
            Register base;
            int offset;
        } memory;
    } value;
} Operand;

// Assembly instruction
typedef struct Instruction {
    InstructionType type;
    Operand operands[3];  // Max 3 operands for most instructions
    int operand_count;
    char* comment;        // Optional comment
    struct Instruction* next;
} Instruction;

// Code generation context
typedef struct {
    Instruction* instructions;
    Instruction* last_instruction;
    int instruction_count;
    
    // Register allocation
    int register_usage[REG_COUNT];
    int next_label_id;
    int stack_offset;
    
    // Symbol mapping (variable name -> stack offset)
    struct SymbolMap {
        char* name;
        int stack_offset;
        int is_boolean;
        struct SymbolMap* next;
    } *symbol_map;
    
    // Target architecture
    TargetArch target;
    
} CodeGenContext;

// AST Node structure (simplified for code generation)
typedef struct ASTNode {
    int type;
    char* node_type_str;
    int line_number;
    
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
} ASTNode;

// Function prototypes
CodeGenContext* create_codegen_context(TargetArch target);
void free_codegen_context(CodeGenContext* ctx);

// AST loading
ASTNode* load_annotated_ast(const char* filename);
void free_ast_node(ASTNode* node);

// Code generation
int generate_assembly(CodeGenContext* ctx, ASTNode* ast, const char* output_file);
void generate_program(CodeGenContext* ctx, ASTNode* node);
void generate_statement(CodeGenContext* ctx, ASTNode* node);
void generate_assignment(CodeGenContext* ctx, ASTNode* node);
void generate_expression(CodeGenContext* ctx, ASTNode* node, Register result_reg);
void generate_binary_op(CodeGenContext* ctx, ASTNode* node, Register result_reg);
void generate_identifier(CodeGenContext* ctx, const char* name, Register result_reg);

// Instruction generation
void emit_instruction(CodeGenContext* ctx, InstructionType type, int operand_count, ...);
void emit_label(CodeGenContext* ctx, const char* label);
void emit_comment(CodeGenContext* ctx, const char* comment);

// Register management
Register allocate_register(CodeGenContext* ctx);
void free_register(CodeGenContext* ctx, Register reg);
int is_register_free(CodeGenContext* ctx, Register reg);

// Symbol management
void add_symbol(CodeGenContext* ctx, const char* name, int is_boolean);
int get_symbol_offset(CodeGenContext* ctx, const char* name);
int symbol_exists(CodeGenContext* ctx, const char* name);

// Assembly output
void write_assembly_header(FILE* file, TargetArch target);
void write_assembly_footer(FILE* file, TargetArch target);
void write_instruction(FILE* file, Instruction* inst, TargetArch target);
void write_data_section(FILE* file, CodeGenContext* ctx);

// Utility functions
const char* register_to_string(Register reg, TargetArch target);
const char* instruction_to_string(InstructionType type);
char* generate_label(CodeGenContext* ctx, const char* prefix);

#endif // CODE_GENERATOR_H