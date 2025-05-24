%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

extern int yylex();
extern int yylineno;
extern FILE* yyin;

ASTNode* ast_root = NULL;

void yyerror(const char* s);
%}

%union {
    int bool_val;
    char* str;
    struct ASTNode* node;
    TokenType token;
}

%token <bool_val> T_TRUE T_FALSE
%token <str> IDENTIFIER
%token AND OR NOT XOR XNOR IMPLIES IFF ASSIGN EQUIV
%token EXISTS FORALL IF IFF_KEYWORD
%token LPAREN RPAREN INVALID_TOKEN

%type <node> expression logical_expr logical_term logical_factor
%type <node> quantified_expr assignment conditional
%type <node> primary program statement_list statement

// Operator precedence and associativity (lowest to highest precedence)
%right ASSIGN
%left IFF EQUIV
%left IMPLIES
%left OR XOR
%left AND XNOR
%right NOT
%right EXISTS FORALL

%start program

%%

program:
    statement_list { 
        ast_root = $1; 
        printf("Parse tree root created successfully\n");
    }
    | /* empty */ { 
        ast_root = NULL; 
        printf("Empty program parsed\n");
    }
    ;

statement_list:
    statement { 
        $$ = $1;
    }
    | statement_list statement { 
        // Create a sequence node to hold multiple statements
        $$ = create_binary_node(AND, $1, $2); 
        $$->line_number = yylineno;
    }
    ;

statement:
    assignment { 
        $$ = $1; 
    }
    | expression { 
        $$ = $1; 
    }
    | conditional { 
        $$ = $1; 
    }
    ;

assignment:
    IDENTIFIER ASSIGN expression { 
        $$ = create_assignment_node($1, $3); 
        $$->line_number = yylineno;
        printf("Assignment parsed: %s at line %d\n", $1, yylineno);
    }
    ;

conditional:
    IF LPAREN expression RPAREN expression {
        $$ = create_conditional_node($3, $5, NULL);
        $$->line_number = yylineno;
        printf("Conditional parsed at line %d\n", yylineno);
    }
    ;

expression:
    logical_expr { 
        $$ = $1; 
    }
    | quantified_expr { 
        $$ = $1; 
    }
    ;

quantified_expr:
    EXISTS IDENTIFIER expression {
        $$ = create_quantifier_node(EXISTS, $2, $3);
        $$->line_number = yylineno;
        printf("Existential quantifier parsed: %s at line %d\n", $2, yylineno);
    }
    | FORALL IDENTIFIER expression {
        $$ = create_quantifier_node(FORALL, $2, $3);
        $$->line_number = yylineno;
        printf("Universal quantifier parsed: %s at line %d\n", $2, yylineno);
    }
    ;

logical_expr:
    logical_expr IFF logical_term {
        $$ = create_binary_node(IFF, $1, $3);
        $$->line_number = yylineno;
    }
    | logical_expr IMPLIES logical_term {
        $$ = create_binary_node(IMPLIES, $1, $3);
        $$->line_number = yylineno;
    }
    | logical_expr EQUIV logical_term {
        $$ = create_binary_node(EQUIV, $1, $3);
        $$->line_number = yylineno;
    }
    | logical_term { 
        $$ = $1; 
    }
    ;

logical_term:
    logical_term OR logical_factor {
        $$ = create_binary_node(OR, $1, $3);
        $$->line_number = yylineno;
    }
    | logical_term XOR logical_factor {
        $$ = create_binary_node(XOR, $1, $3);
        $$->line_number = yylineno;
    }
    | logical_factor { 
        $$ = $1; 
    }
    ;

logical_factor:
    logical_factor AND primary {
        $$ = create_binary_node(AND, $1, $3);
        $$->line_number = yylineno;
    }
    | logical_factor XNOR primary {
        $$ = create_binary_node(XNOR, $1, $3);
        $$->line_number = yylineno;
    }
    | primary { 
        $$ = $1; 
    }
    ;

primary:
    NOT primary {
        $$ = create_unary_node(NOT, $2);
        $$->line_number = yylineno;
    }
    | LPAREN expression RPAREN { 
        $$ = $2; 
    }
    | T_TRUE {
        $$ = create_boolean_node($1);
        $$->line_number = yylineno;
    }
    | T_FALSE {
        $$ = create_boolean_node($1);
        $$->line_number = yylineno;
    }
    | IDENTIFIER {
        $$ = create_identifier_node($1);
        $$->line_number = yylineno;
    }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parse error at line %d: %s\n", yylineno, s);
    fprintf(stderr, "This usually indicates a syntax error in your input.\n");
    fprintf(stderr, "Check for missing operators, parentheses, or malformed expressions.\n");
}