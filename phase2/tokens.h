#ifndef TOKENS_H
#define TOKENS_H

// Token types enumeration - matches your original lexer.l
typedef enum {
    // Logical operators
    AND = 256,
    OR,
    NOT,
    XOR,
    XNOR,
    
    // Implication operators
    IMPLIES,
    IFF,
    
    // Assignment and equivalence
    ASSIGN,
    EQUIV,
    
    // Quantifiers
    EXISTS,
    FORALL,
    
    // Keywords
    IF,
    IFF_KEYWORD,
    
    // Parentheses
    LPAREN,
    RPAREN,
    
    // Boolean literals
    T_TRUE,
    T_FALSE,
    
    // Identifiers
    IDENTIFIER,
    
    // Special tokens
    INVALID_TOKEN,
    EOF_TOKEN
} TokenType;

// YYSTYPE for compatibility with bison
typedef union {
    int bool_val;
    char *str;
} YYSTYPE;

extern YYSTYPE yylval;

// Function prototypes for lexer interface
int yylex(void);
void yyerror(const char* msg);
extern int yylineno;
extern char* yytext;

// Utility functions
const char* token_type_to_string(TokenType type);
void print_token_info(TokenType type, const char* lexeme);

#endif // TOKENS_H