/*
 * Lexer for a logical expression parser.
 * This lexer recognizes various logical operators, quantifiers, and identifiers.
 * It uses Flex for tokenization and outputs tokens for further processing.
 * 
 * flex lexer.l
 * gcc -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L -o lexer lex.yy.c main.c
 */

%{
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tokens.h"

YYSTYPE yylval;

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case XOR: return "XOR";
        case XNOR: return "XNOR";
        case IMPLIES: return "IMPLIES";
        case IFF: return "IFF";
        case ASSIGN: return "ASSIGN";
        case EQUIV: return "EQUIV";
        case EXISTS: return "EXISTS";
        case FORALL: return "FORALL";
        case IF: return "IF";
        case IFF_KEYWORD: return "IFF_KEYWORD";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case T_TRUE: return "T_TRUE";
        case T_FALSE: return "T_FALSE";
        case IDENTIFIER: return "IDENTIFIER";
        case INVALID_TOKEN: return "INVALID_TOKEN";
        case EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}
%}

%option noyywrap
%option yylineno
%option noinput nounput

%%

"AND"|"and"|"&&"                     { return AND; }
"OR"|"or"|"||"                       { return OR; }
"NOT"|"not"|"~"                      { return NOT; }
"XOR"|"xor"                          { return XOR; }
"XNOR"|"xnor"                        { return XNOR; }

"->"|"-->"|"==>"|"=>"|"IMPLIES"|"implies" { return IMPLIES; }
"<->"|"<=>"|"<-->"|"<==>"|"DOUBLEIMPLIES"|"D_IMPLIES" { return IFF; }

"="|"EQUALS"|"equals"                { return ASSIGN; }
"==="|"EQUIVALENT"|"equivalent"      { return EQUIV; }

"E_Q"                                { return EXISTS; }
"U_Q"                                { return FORALL; }

"IF"|"if"                            { return IF; }
"IFF"|"iff"                          { return IFF_KEYWORD; }

"("                                  { return LPAREN; }
")"                                  { return RPAREN; }

"true"|"TRUE"    { 
    yylval.bool_val = 1; 
    return T_TRUE; 
}
"false"|"FALSE"  { 
    yylval.bool_val = 0; 
    return T_FALSE; 
}

[A-Za-z_][A-Za-z0-9_]*               { 
    yylval.str = yytext; 
    return IDENTIFIER; 
}

[ \t\n\r]+                           ;

.   {
    fprintf(stderr, "Lexer error: Unrecognized character '%s' at line %d\n", yytext, yylineno);
    return INVALID_TOKEN;
}

%%