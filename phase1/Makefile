# Makefile for Phase 1 Lexical Analyzer
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L
FLEX = flex

# Targets
all: lexer

lexer: lex.yy.c main.c tokens.h
	$(CC) $(CFLAGS) -o lexer lex.yy.c main.c

lex.yy.c: lexer.l tokens.h
	$(FLEX) lexer.l

clean:
	rm -f lex.yy.c lexer *.o

test: lexer
	./lexer

.PHONY: all clean test