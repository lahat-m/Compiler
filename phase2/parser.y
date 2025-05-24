%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
typedef struct ASTNode ASTNode;

#include "ast.h"

// Global variables
extern FILE* token_input;
extern int current_line;
ASTNode* ast_root = NULL;

// Function prototypes
int yylex(void);
void yyerror(const char* msg);
int parse_tokens_from_file(const char* filename);
%}

%union {
    int bool_val;
    char* str;
    ASTNode* node;
}

// Token declarations from Phase 1
%token <bool_val> T_TRUE T_FALSE
%token <str> IDENTIFIER
%token AND OR NOT XOR XNOR
%token IMPLIES IFF EQUIV
%token EXISTS FORALL
%token IF IFF_KEYWORD
%token ASSIGN
%token LPAREN RPAREN
%token INVALID_TOKEN EOF_TOKEN

// Non-terminal types
%type <node> program statement_list statement expression
%type <node> logical_expr term factor
%type <node> assignment quantified_expr

// Operator precedence and associativity
%right ASSIGN
%left IFF EQUIV
%left IMPLIES
%left OR XOR XNOR
%left AND
%right NOT
%left EXISTS FORALL
%left LPAREN RPAREN

// Start symbol
%start program

%%

program:
    statement_list {
        ast_root = create_program_node(current_line);
        if ($1) {
            // If we have a statement list, copy its statements to the program
            if ($1->type == AST_STATEMENT_LIST) {
                for (int i = 0; i < $1->data.program.count; i++) {
                    add_statement_to_program(ast_root, $1->data.program.statements[i]);
                }
            } else {
                add_statement_to_program(ast_root, $1);
            }
        }
        $$ = ast_root;
    }
    | /* empty */ {
        ast_root = create_program_node(current_line);
        $$ = ast_root;
    }
    ;

statement_list:
    statement {
        $$ = create_program_node(current_line);
        if ($1) {
            add_statement_to_program($$, $1);
        }
    }
    | statement_list statement {
        $$ = $1;
        if ($2) {
            add_statement_to_program($$, $2);
        }
    }
    ;

statement:
    assignment {
        $$ = $1;
    }
    | expression {
        $$ = create_expression_stmt_node($1, current_line);
    }
    ;

assignment:
    IDENTIFIER ASSIGN expression {
        $$ = create_assignment_node($1, $3, current_line);
        free($1); // Free the string since we copied it
    }
    ;

expression:
    logical_expr {
        $$ = $1;
    }
    ;

logical_expr:
    logical_expr IFF logical_expr {
        $$ = create_binary_node(AST_IFF, $1, $3, current_line);
    }
    | logical_expr EQUIV logical_expr {
        $$ = create_binary_node(AST_EQUIV, $1, $3, current_line);
    }
    | logical_expr IMPLIES logical_expr {
        $$ = create_binary_node(AST_IMPLIES, $1, $3, current_line);
    }
    | logical_expr OR logical_expr {
        $$ = create_binary_node(AST_OR, $1, $3, current_line);
    }
    | logical_expr XOR logical_expr {
        $$ = create_binary_node(AST_XOR, $1, $3, current_line);
    }
    | logical_expr XNOR logical_expr {
        $$ = create_binary_node(AST_XNOR, $1, $3, current_line);
    }
    | logical_expr AND logical_expr {
        $$ = create_binary_node(AST_AND, $1, $3, current_line);
    }
    | term {
        $$ = $1;
    }
    ;

term:
    NOT factor {
        $$ = create_unary_node(AST_NOT, $2, current_line);
    }
    | factor {
        $$ = $1;
    }
    ;

factor:
    IDENTIFIER {
        $$ = create_identifier_node($1, current_line);
        free($1); // Free the string since we copied it
    }
    | T_TRUE {
        $$ = create_boolean_node(1, current_line);
    }
    | T_FALSE {
        $$ = create_boolean_node(0, current_line);
    }
    | LPAREN logical_expr RPAREN {
        $$ = $2;
    }
    | quantified_expr {
        $$ = $1;
    }
    ;

quantified_expr:
    EXISTS IDENTIFIER logical_expr {
        $$ = create_quantifier_node(AST_EXISTS, $2, $3, current_line);
        free($2);
    }
    | FORALL IDENTIFIER logical_expr {
        $$ = create_quantifier_node(AST_FORALL, $2, $3, current_line);
        free($2);
    }
    ;

%%

void yyerror(const char* msg) {
    fprintf(stderr, "Parse error at line %d: %s\n", current_line, msg);
}