#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Symbol types
typedef enum {
    SYM_BOOLEAN,
    SYM_IDENTIFIER,
    SYM_FUNCTION,
    SYM_PREDICATE,
    SYM_UNKNOWN
} SymbolType;

// Symbol table entry
typedef struct SymbolEntry {
    char* name;
    SymbolType type;
    int is_defined;         // Has been assigned a value
    int is_used;           // Has been referenced
    int line_declared;     // Line where first declared/assigned
    int line_used;         // Line where first used
    
    // Value information
    union {
        int bool_value;    // For boolean variables
        char* str_value;   // For other types
    } value;
    
    struct SymbolEntry* next;  // For hash table chaining
} SymbolEntry;

// Symbol table structure
typedef struct {
    SymbolEntry** table;
    int size;
    int count;
} SymbolTable;

// Function prototypes
SymbolTable* create_symbol_table(int size);
void free_symbol_table(SymbolTable* table);

// Symbol operations
SymbolEntry* lookup_symbol(SymbolTable* table, const char* name);
SymbolEntry* insert_symbol(SymbolTable* table, const char* name, SymbolType type, int line);
void mark_symbol_used(SymbolTable* table, const char* name, int line);
void set_symbol_value(SymbolTable* table, const char* name, int bool_val, int line);

// Analysis functions
void print_symbol_table(SymbolTable* table);
void print_symbol_table_to_file(SymbolTable* table, const char* filename);
int check_undefined_symbols(SymbolTable* table);
int check_unused_symbols(SymbolTable* table);

// Utility functions
const char* symbol_type_to_string(SymbolType type);
unsigned int hash_string(const char* str, int table_size);

#endif // SYMBOL_TABLE_H