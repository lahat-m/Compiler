#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "symbol_table.h"

// Hash function for symbol names
unsigned int hash_string(const char* str, int table_size) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % table_size;
}

// Create symbol table
SymbolTable* create_symbol_table(int size) {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    table->table = calloc(size, sizeof(SymbolEntry*));
    table->size = size;
    table->count = 0;
    return table;
}

// Free symbol table
void free_symbol_table(SymbolTable* table) {
    if (!table) return;
    
    for (int i = 0; i < table->size; i++) {
        SymbolEntry* entry = table->table[i];
        while (entry) {
            SymbolEntry* next = entry->next;
            free(entry->name);
            if (entry->type != SYM_BOOLEAN && entry->value.str_value) {
                free(entry->value.str_value);
            }
            free(entry);
            entry = next;
        }
    }
    
    free(table->table);
    free(table);
}

// Lookup symbol in table
SymbolEntry* lookup_symbol(SymbolTable* table, const char* name) {
    if (!table || !name) return NULL;
    
    unsigned int index = hash_string(name, table->size);
    SymbolEntry* entry = table->table[index];
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    
    return NULL;
}

// Insert symbol into table
SymbolEntry* insert_symbol(SymbolTable* table, const char* name, SymbolType type, int line) {
    if (!table || !name) return NULL;
    
    // Check if symbol already exists
    SymbolEntry* existing = lookup_symbol(table, name);
    if (existing) {
        return existing;  // Return existing entry
    }
    
    // Create new entry
    SymbolEntry* entry = malloc(sizeof(SymbolEntry));
    entry->name = strdup(name);
    entry->type = type;
    entry->is_defined = 0;
    entry->is_used = 0;
    entry->line_declared = line;
    entry->line_used = -1;
    
    // Initialize value
    if (type == SYM_BOOLEAN) {
        entry->value.bool_value = 0;
    } else {
        entry->value.str_value = NULL;
    }
    
    // Insert into hash table
    unsigned int index = hash_string(name, table->size);
    entry->next = table->table[index];
    table->table[index] = entry;
    table->count++;
    
    return entry;
}

// Mark symbol as used
void mark_symbol_used(SymbolTable* table, const char* name, int line) {
    SymbolEntry* entry = lookup_symbol(table, name);
    if (entry) {
        if (!entry->is_used) {
            entry->is_used = 1;
            entry->line_used = line;
        }
    }
}

// Set symbol value
void set_symbol_value(SymbolTable* table, const char* name, int bool_val, int line) {
    SymbolEntry* entry = lookup_symbol(table, name);
    if (entry) {
        entry->is_defined = 1;
        entry->line_declared = line;
        if (entry->type == SYM_BOOLEAN) {
            entry->value.bool_value = bool_val;
        }
    }
}

// Convert symbol type to string
const char* symbol_type_to_string(SymbolType type) {
    switch (type) {
        case SYM_BOOLEAN: return "BOOLEAN";
        case SYM_IDENTIFIER: return "IDENTIFIER";
        case SYM_FUNCTION: return "FUNCTION";
        case SYM_PREDICATE: return "PREDICATE";
        case SYM_UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

// Print symbol table
void print_symbol_table(SymbolTable* table) {
    printf("SYMBOL TABLE\n");
    printf("%-12s %-10s %-8s %-8s %-6s %-6s %s\n", 
           "Name", "Type", "Defined", "Used", "Decl", "Use", "Value");
    printf("────────────────────────────────────────────────────────────────\n");
    
    int displayed = 0;
    for (int i = 0; i < table->size; i++) {
        SymbolEntry* entry = table->table[i];
        while (entry) {
            printf("%-12s %-10s %-8s %-8s %-6d %-6d", 
                   entry->name,
                   symbol_type_to_string(entry->type),
                   entry->is_defined ? "Yes" : "No",
                   entry->is_used ? "Yes" : "No",
                   entry->line_declared,
                   entry->line_used > 0 ? entry->line_used : 0);
            
            if (entry->type == SYM_BOOLEAN && entry->is_defined) {
                printf(" %s", entry->value.bool_value ? "TRUE" : "FALSE");
            } else {
                printf(" --");
            }
            printf("\n");
            
            entry = entry->next;
            displayed++;
        }
    }
    
    if (displayed == 0) {
        printf("(No symbols found)\n");
    }
    
    printf("Total symbols: %d\n", table->count);
    printf("\n\n");
}

// Print symbol table to file
void print_symbol_table_to_file(SymbolTable* table, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Cannot create symbol table file %s\n", filename);
        return;
    }
    
    fprintf(file, "# Symbol Table\n");
    fprintf(file, "# Generated by Phase 3: Semantic Analysis\n");
    fprintf(file, "#\n\n");
    
    fprintf(file, "%-12s %-10s %-8s %-8s %-6s %-6s %s\n", 
            "Name", "Type", "Defined", "Used", "Decl", "Use", "Value");
    fprintf(file, "────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < table->size; i++) {
        SymbolEntry* entry = table->table[i];
        while (entry) {
            fprintf(file, "%-12s %-10s %-8s %-8s %-6d %-6d", 
                    entry->name,
                    symbol_type_to_string(entry->type),
                    entry->is_defined ? "Yes" : "No",
                    entry->is_used ? "Yes" : "No",
                    entry->line_declared,
                    entry->line_used > 0 ? entry->line_used : 0);
            
            if (entry->type == SYM_BOOLEAN && entry->is_defined) {
                fprintf(file, " %s", entry->value.bool_value ? "TRUE" : "FALSE");
            } else {
                fprintf(file, " --");
            }
            fprintf(file, "\n");
            
            entry = entry->next;
        }
    }
    
    fprintf(file, "\nTotal symbols: %d\n", table->count);
    fclose(file);
}

// Check for undefined symbols
int check_undefined_symbols(SymbolTable* table) {
    int undefined_count = 0;
    
    for (int i = 0; i < table->size; i++) {
        SymbolEntry* entry = table->table[i];
        while (entry) {
            if (entry->is_used && !entry->is_defined) {
                undefined_count++;
            }
            entry = entry->next;
        }
    }
    
    return undefined_count;
}

// Check for unused symbols
int check_unused_symbols(SymbolTable* table) {
    int unused_count = 0;
    
    for (int i = 0; i < table->size; i++) {
        SymbolEntry* entry = table->table[i];
        while (entry) {
            if (entry->is_defined && !entry->is_used) {
                unused_count++;
            }
            entry = entry->next;
        }
    }
    
    return unused_count;
}