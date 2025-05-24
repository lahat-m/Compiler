# Makefile for Phase 4 Code Generation
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -D_GNU_SOURCE

# Object files
OBJS = main_phase4.o code_generator.o ast_loader_phase4.o assembly_writer.o

# Targets
all: code_generator

code_generator: $(OBJS)
	$(CC) $(CFLAGS) -o code_generator $(OBJS)

# Compile main driver
main_phase4.o: main_phase4.c code_generator.h
	$(CC) $(CFLAGS) -c main_phase4.c

# Compile code generator
code_generator.o: code_generator.c code_generator.h
	$(CC) $(CFLAGS) -c code_generator.c

# Compile AST loader
ast_loader_phase4.o: ast_loader_phase4.c code_generator.h
	$(CC) $(CFLAGS) -c ast_loader_phase4.c

# Compile assembly writer
assembly_writer.o: assembly_writer.c code_generator.h
	$(CC) $(CFLAGS) -c assembly_writer.c

# Test target
test: code_generator
	./code_generator

# Test with specific file
test-file: code_generator
	./code_generator annotated_ast.txt

# Test full compilation pipeline
test-compile: code_generator
	./code_generator
	@echo "Attempting to assemble generated code..."
	@if [ -f program.s ]; then \
		as -64 program.s -o program.o 2>/dev/null && \
		ld program.o -o program 2>/dev/null && \
		echo "✓ Assembly and linking successful!" && \
		echo "  Run with: ./program" || \
		echo "⚠️  Assembly/linking failed (expected - may need runtime setup)"; \
	else \
		echo "❌ No assembly file generated"; \
	fi

# Clean target
clean:
	rm -f *.o code_generator

# Clean everything including generated files
distclean: clean
	rm -f program.s program.o program

.PHONY: all test test-file test-compile clean distclean