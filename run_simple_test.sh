#!/bin/bash

# Simple Test Suite for Roadmap Compiler
echo "╔═══════════════════════════════════════════════════════════════╗"
echo "║                ROADMAP COMPILER - SIMPLE TESTS               ║"
echo "║              Testing Basic Functionality Only                ║"
echo "╚═══════════════════════════════════════════════════════════════╝"
echo

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check directories
if [ ! -d "phase1" ] || [ ! -d "phase2" ] || [ ! -d "phase3" ] || [ ! -d "phase4" ]; then
    echo -e "${RED}❌ Phase directories not found!${NC}"
    exit 1
fi

# Check executables
echo "Checking phase executables..."
MISSING=""
if [ ! -f "phase1/lexer" ]; then MISSING="$MISSING phase1/lexer"; fi
if [ ! -f "phase2/parser_test" ]; then MISSING="$MISSING phase2/parser_test"; fi
if [ ! -f "phase3/semantic_analyzer" ]; then MISSING="$MISSING phase3/semantic_analyzer"; fi
if [ ! -f "phase4/code_generator" ]; then MISSING="$MISSING phase4/code_generator"; fi

if [ -n "$MISSING" ]; then
    echo -e "${RED}❌ Missing executables: $MISSING${NC}"
    exit 1
fi

echo -e "${GREEN}✓ All phase executables found${NC}"
echo

TEST_NUM=1
PASSED=0
FAILED=0

# Function to run a simple test
run_simple_test() {
    local test_name="$1"
    local test_input="$2"
    
    echo -e "${BLUE}═══ Test $TEST_NUM: $test_name ═══${NC}"
    echo "Input:"
    echo "$test_input"
    echo
    
    # Create test file
    echo "$test_input" > test.txt
    
    echo -e "${YELLOW}Running complete pipeline...${NC}"
    
    # Phase 1
    echo "Phase 1: Lexical Analysis"
    cd phase1
    if ./lexer > /dev/null 2>&1; then
        echo "✓ Lexical analysis passed"
        TOKEN_COUNT=$(grep -v '^#' tokens.txt | grep -v '^$' | grep -v '^EOF' | wc -l 2>/dev/null || echo "0")
        echo "  Generated $TOKEN_COUNT tokens"
    else
        echo "❌ Lexical analysis failed"
        cd ..
        ((FAILED++))
        ((TEST_NUM++))
        return
    fi
    cd ..
    
    # Phase 2
    echo "Phase 2: Syntax Analysis"
    cd phase2
    if ./parser_test ../phase1/tokens.txt > /dev/null 2>&1; then
        echo "✓ Syntax analysis passed"
    else
        echo "❌ Syntax analysis failed"
        cd ..
        ((FAILED++))
        ((TEST_NUM++))
        return
    fi
    cd ..
    
    # Phase 3
    echo "Phase 3: Semantic Analysis"
    cd phase3
    if ./semantic_analyzer ../phase2/ast.txt > /dev/null 2>&1; then
        echo "✓ Semantic analysis passed"
    else
        echo "❌ Semantic analysis failed"
        cd ..
        ((FAILED++))
        ((TEST_NUM++))
        return
    fi
    cd ..
    
    # Phase 4
    echo "Phase 4: Code Generation"
    cd phase4
    if ./code_generator ../phase3/annotated_ast.txt > /dev/null 2>&1; then
        echo "✓ Code generation passed"
    else
        echo "❌ Code generation failed"
        cd ..
        ((FAILED++))
        ((TEST_NUM++))
        return
    fi
    
    # Assembly and Execution
    echo "Assembly & Execution"
    if as -64 program.s -o program.o 2>/dev/null; then
        if ld program.o -o test_program 2>/dev/null; then
            if ./test_program 2>/dev/null; then
                EXIT_CODE=$?
                echo "✓ Program executed successfully (exit code: $EXIT_CODE)"
                echo -e "${GREEN}✅ TEST PASSED${NC}"
                ((PASSED++))
            else
                echo "❌ Program execution failed"
                echo -e "${RED}❌ TEST FAILED${NC}"
                ((FAILED++))
            fi
        else
            echo "❌ Linking failed"
            echo -e "${RED}❌ TEST FAILED${NC}"
            ((FAILED++))
        fi
    else
        echo "❌ Assembly failed"
        echo -e "${RED}❌ TEST FAILED${NC}"
        ((FAILED++))
    fi
    
    # Cleanup
    rm -f program.s program.o test_program
    cd ..
    
    echo
    ((TEST_NUM++))
}

echo "Starting simple test suite..."
echo

# Run tests
run_simple_test "Boolean Literal" "TRUE"

run_simple_test "Simple Assignment" "A = TRUE"

run_simple_test "Two Assignments" "A = TRUE
B = FALSE"

run_simple_test "Basic OR Operation" "B = TRUE
C = FALSE
B OR C"

run_simple_test "Basic AND Operation" "X = TRUE
Y = FALSE
X AND Y"

run_simple_test "Different Variables" "P = TRUE
Q = TRUE
P OR Q"

run_simple_test "All FALSE Values" "A = FALSE
B = FALSE
A OR B"

run_simple_test "All TRUE Values" "A = TRUE
B = TRUE
A AND B"

# Print results
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}                      SIMPLE TEST RESULTS                      ${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo
echo "Total tests: $((TEST_NUM-1))"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"
echo

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}🎉 ALL SIMPLE TESTS PASSED! 🎉${NC}"
    echo -e "${GREEN}The compiler handles basic logical expressions perfectly!${NC}"
    echo
    echo -e "${BLUE}Ready for complex tests? Run ./run_complex_test.sh${NC}"
else
    PERCENT=$(( (PASSED * 100) / (TEST_NUM-1) ))
    echo -e "${YELLOW}⚠️  $PERCENT% of simple tests passed${NC}"
    
    if [ $PASSED -gt 0 ]; then
        echo -e "${GREEN}✓ $PASSED tests working correctly${NC}"
        echo -e "${YELLOW}Some functionality needs attention${NC}"
    else
        echo -e "${RED}Basic functionality needs attention${NC}"
    fi
fi

# Cleanup
rm -f test.txt

echo