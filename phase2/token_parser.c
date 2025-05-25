#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"
#include "parser.tab.h"  // This will contain the token definitions

// Global variables
FILE* token_input = NULL;
int current_line = 1;
static char current_token_type[50];
static char current_lexeme[100];
static int current_value;
static int end_of_tokens = 0;

// Convert token string to token value
int string_to_token(const char* token_str) {
    if (strcmp(token_str, "IDENTIFIER") == 0) return IDENTIFIER;
    if (strcmp(token_str, "T_TRUE") == 0) return T_TRUE;
    if (strcmp(token_str, "T_FALSE") == 0) return T_FALSE;
    if (strcmp(token_str, "AND") == 0) return AND;
    if (strcmp(token_str, "OR") == 0) return OR;
    if (strcmp(token_str, "NOT") == 0) return NOT;
    if (strcmp(token_str, "XOR") == 0) return XOR;
    if (strcmp(token_str, "XNOR") == 0) return XNOR;
    if (strcmp(token_str, "IMPLIES") == 0) return IMPLIES;
    if (strcmp(token_str, "IFF") == 0) return IFF;
    if (strcmp(token_str, "EQUIV") == 0) return EQUIV;
    if (strcmp(token_str, "EXISTS") == 0) return EXISTS;
    if (strcmp(token_str, "FORALL") == 0) return FORALL;
    if (strcmp(token_str, "IF") == 0) return IF;
    if (strcmp(token_str, "IFF_KEYWORD") == 0) return IFF_KEYWORD;
    if (strcmp(token_str, "ASSIGN") == 0) return ASSIGN;
    if (strcmp(token_str, "LPAREN") == 0) return LPAREN;
    if (strcmp(token_str, "RPAREN") == 0) return RPAREN;
    if (strcmp(token_str, "INVALID_TOKEN") == 0) return INVALID_TOKEN;
    if (strcmp(token_str, "EOF") == 0) return EOF_TOKEN;
    return 0;  // Unknown token
}

// Read next token from file
int read_next_token_from_file() {
    if (!token_input || end_of_tokens) {
        return 0;
    }
    
    char line[256];
    
    while (fgets(line, sizeof(line), token_input)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        // Check for EOF marker
        if (strncmp(line, "EOF", 3) == 0) {
            end_of_tokens = 1;
            return 0;
        }
        
        // Parse token line: TOKEN_TYPE LEXEME [VALUE]
        char* token_type = strtok(line, " \t\n\r");
        char* lexeme = strtok(NULL, " \t\n\r");
        char* value_str = strtok(NULL, " \t\n\r");
        
        if (!token_type || !lexeme) {
            continue;  // Invalid line format
        }
        
        // Store current token info
        strncpy(current_token_type, token_type, sizeof(current_token_type) - 1);
        strncpy(current_lexeme, lexeme, sizeof(current_lexeme) - 1);
        current_token_type[sizeof(current_token_type) - 1] = '\0';
        current_lexeme[sizeof(current_lexeme) - 1] = '\0';
        
        // Parse value if present
        if (value_str) {
            current_value = atoi(value_str);
        } else {
            current_value = 0;
        }
        
        return string_to_token(token_type);
    }
    
    end_of_tokens = 1;
    return 0;
}

// Bison's yylex function - interface between parser and token reader
int yylex(void) {
    int token = read_next_token_from_file();
    
    if (token == 0) {
        return 0;  // EOF
    }
    
    // Set yylval based on token type
    extern YYSTYPE yylval;
    
    if (token == T_TRUE || token == T_FALSE) {
        yylval.bool_val = current_value;
    } else if (token == IDENTIFIER) {
        yylval.str = strdup(current_lexeme);
    }
    
    return token;
}

// Initialize token parser
int init_token_parser(const char* token_file) {
    token_input = fopen(token_file, "r");
    if (!token_input) {
        fprintf(stderr, "Error: Cannot open token file '%s'\n", token_file);
        return -1;
    }
    
    current_line = 1;
    end_of_tokens = 0;
    return 0;
}

// Cleanup token parser
void cleanup_token_parser() {
    if (token_input) {
        fclose(token_input);
        token_input = NULL;
    }
}

// Convert token value to string (for debugging)
const char* token_type_to_string(int type) {
    switch (type) {
        case IDENTIFIER: return "IDENTIFIER";
        case T_TRUE: return "T_TRUE";
        case T_FALSE: return "T_FALSE";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case XOR: return "XOR";
        case XNOR: return "XNOR";
        case IMPLIES: return "IMPLIES";
        case IFF: return "IFF";
        case EQUIV: return "EQUIV";
        case EXISTS: return "EXISTS";
        case FORALL: return "FORALL";
        case IF: return "IF";
        case IFF_KEYWORD: return "IFF_KEYWORD";
        case ASSIGN: return "ASSIGN";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case INVALID_TOKEN: return "INVALID_TOKEN";
        case EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}

// Parse tokens from file and build AST
int parse_tokens_from_file(const char* filename) {
    if (init_token_parser(filename) != 0) {
        return -1;
    }
    
    // Call bison parser
    extern int yyparse(void);
    
    printf("PARSING TOKENS FROM: %s\n", filename);
    
    int result = yyparse();
    
    if (result == 0) {
        printf(" Parsing completed successfully\n");
        printf(" AST generated\n");
    } else {
        printf(" Parsing failed\n");
    }
    
    printf("\n\n");
    
    cleanup_token_parser();
    return result;
}