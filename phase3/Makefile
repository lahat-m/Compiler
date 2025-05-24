# Makefile for Phase 3 Semantic Analysis
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -D_GNU_SOURCE

# Object files
OBJS = main_phase3.o semantic_analyzer.o symbol_table.o ast_loader.o

# Targets
all: semantic_analyzer

semantic_analyzer: $(OBJS)
	$(CC) $(CFLAGS) -o semantic_analyzer $(OBJS)

# Compile main driver
main_phase3.o: main_phase3.c semantic_analyzer.h symbol_table.h
	$(CC) $(CFLAGS) -c main_phase3.c

# Compile semantic analyzer
semantic_analyzer.o: semantic_analyzer.c semantic_analyzer.h symbol_table.h
	$(CC) $(CFLAGS) -c semantic_analyzer.c

# Compile symbol table
symbol_table.o: symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c

# Compile AST loader
ast_loader.o: ast_loader.c semantic_analyzer.h
	$(CC) $(CFLAGS) -c ast_loader.c

# Test target
test: semantic_analyzer
	./semantic_analyzer

# Test with specific file
test-file: semantic_analyzer
	./semantic_analyzer ast.txt

# Clean target
clean:
	rm -f *.o semantic_analyzer

# Clean everything including generated files
distclean: clean
	rm -f annotated_ast.txt symbol_table.txt semantic_errors.txt

.PHONY: all test test-file clean distclean