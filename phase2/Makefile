# Makefile for Phase 2 Syntax Analysis
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -D_GNU_SOURCE
BISON = bison
FLEX = flex

# Object files
OBJS = parser.tab.o ast.o token_parser.o main_phase2.o

# Targets
all: parser_test

parser_test: $(OBJS)
	$(CC) $(CFLAGS) -o parser_test $(OBJS)

# Bison generates parser.tab.c and parser.tab.h
parser.tab.c parser.tab.h: parser.y ast.h
	$(BISON) -d parser.y

# Compile generated parser
parser.tab.o: parser.tab.c parser.tab.h ast.h
	$(CC) $(CFLAGS) -c parser.tab.c

# Compile AST implementation
ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

# Compile token parser  
token_parser.o: token_parser.c ast.h parser.tab.h
	$(CC) $(CFLAGS) -c token_parser.c

# Compile main driver
main_phase2.o: main_phase2.c ast.h parser.tab.h
	$(CC) $(CFLAGS) -c main_phase2.c

# Test target
test: parser_test
	./parser_test

# Clean target
clean:
	rm -f *.o parser_test parser.tab.c parser.tab.h

# Clean everything including generated files
distclean: clean
	rm -f ast.txt

.PHONY: all test clean distclean