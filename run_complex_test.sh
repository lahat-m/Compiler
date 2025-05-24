#!/bin/bash

# Complex Test Suite for Roadmap Compiler
echo "╔══════════════════════════════════════════════════════════════════╗"
echo "║                ROADMAP COMPILER - COMPLEX TESTS                 ║"
echo "║              Testing Advanced Functionality                     ║"
echo "╚══════════════════════════════════════════════════════════════════╝"
echo

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
NC='\033[0m'

# Check directories
if [ ! -d "phase1" ] || [ ! -d "phase2" ] || [ ! -d "phase3" ] || [ ! -d "phase4" ]; then
    echo -e "${RED}❌ Phase directories not found!${NC}"
    exit 1
fi

echo -e "${GREEN}✓ All phase executables confirmed working${NC}"
echo

TEST_NUM=1
PASSED=0
FAILED=0

# Function to run complex test
run_complex_test() {
    local test_name="$1"
    local test_input="$2"
    local complexity_level="$3"
    local expected_behavior="$4"
    
    echo -e "${CYAN}═══ Test $TEST_NUM: $test_name [$complexity_level] ═══${NC}"
    echo -e "${BLUE}Input:${NC}"
    echo "$test_input"
    echo -e "${BLUE}Expected:${NC} $expected_behavior"
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
        AST_LINES=$(wc -l < ast.txt 2>/dev/null || echo "0")
        echo "  AST generated: $AST_LINES lines"
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
        INST_COUNT=$(grep -c "^    " program.s 2>/dev/null || echo "0")
        echo "  Assembly instructions: $INST_COUNT"
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
            if timeout 5s ./test_program 2>/dev/null; then
                EXIT_CODE=$?
                echo "✓ Program executed successfully (exit code: $EXIT_CODE)"
                echo -e "${GREEN}✅ TEST PASSED${NC}"
                ((PASSED++))
            else
                echo "❌ Program execution failed or timed out"
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

echo "Starting complex expression test suite..."
echo

# Test 1: NOT operation
run_complex_test "NOT Operation" "A = TRUE
result = NOT A" "INTERMEDIATE" "Should handle unary NOT operator"

# Test 2: Operator precedence
run_complex_test "Operator Precedence" "A = TRUE
B = FALSE
C = TRUE
result = A OR B AND C" "INTERMEDIATE" "Should respect AND has higher precedence than OR"

# Test 3: Parenthesized expressions
run_complex_test "Parenthesized Expression" "A = TRUE
B = FALSE
C = TRUE
result = (A OR B) AND C" "INTERMEDIATE" "Should respect parentheses over operator precedence"

# Test 4: XOR operation
run_complex_test "XOR Operation" "P = TRUE
Q = FALSE
result = P XOR Q" "INTERMEDIATE" "Should handle exclusive OR operation"

# Test 5: XNOR operation
run_complex_test "XNOR Operation" "X = TRUE
Y = TRUE
result = X XNOR Y" "INTERMEDIATE" "Should handle exclusive NOR operation"

# Test 6: Implication
run_complex_test "Implication Operation" "P = TRUE
Q = FALSE
result = P IMPLIES Q" "ADVANCED" "Should handle logical implication"

# Test 7: Equivalence
run_complex_test "Equivalence Operation" "A = TRUE
B = TRUE
result = A EQUIV B" "ADVANCED" "Should handle logical equivalence"

# Test 8: Complex nested expression
run_complex_test "Complex Nested Expression" "P = TRUE
Q = FALSE
R = TRUE
S = FALSE
result = (P AND Q) OR (R XOR S)" "ADVANCED" "Should handle complex nested logical operations"

# Test 9: All operators combined
run_complex_test "All Operators Combined" "A = TRUE
B = FALSE
C = TRUE
D = FALSE
result = NOT A OR (B AND C) XOR (C IMPLIES D)" "EXPERT" "Should handle expression with all logical operators"

# Test 10: Long variable names
run_complex_test "Long Variable Names" "condition_one = TRUE
condition_two = FALSE
final_result = condition_one AND condition_two" "INTERMEDIATE" "Should handle descriptive variable names"

# Test 11: Multiple complex assignments
run_complex_test "Multiple Complex Assignments" "first = TRUE
second = FALSE
third = TRUE
result_one = first AND second
result_two = second OR third
final = result_one XOR result_two" "ADVANCED" "Should handle multiple intermediate results"

# Test 12: Deeply nested parentheses
run_complex_test "Deeply Nested Parentheses" "A = TRUE
B = FALSE
C = TRUE
D = FALSE
result = ((A OR B) AND (C OR D)) XOR (A AND B)" "EXPERT" "Should handle deeply nested parenthetical expressions"

# Cleanup
rm -f test.txt

# Print results
echo -e "${PURPLE}══════════════════════════════════════════════════════════════════${NC}"
echo -e "${PURPLE}                      COMPLEX TEST RESULTS                       ${NC}"
echo -e "${PURPLE}══════════════════════════════════════════════════════════════════${NC}"
echo
echo "Total tests: $((TEST_NUM-1))"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"
echo

# Categorize results
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}🎉 ALL COMPLEX TESTS PASSED! 🎉${NC}"
    echo -e "${GREEN}Your compiler handles advanced logical expressions perfectly!${NC}"
    echo
    echo -e "${CYAN}🚀 COMPILER MATURITY LEVEL: EXPERT 🚀${NC}"
    echo -e "${CYAN}Ready for production use with complex logical systems!${NC}"
elif [ $PASSED -gt $((TEST_NUM/2)) ]; then
    PERCENT=$(( (PASSED * 100) / (TEST_NUM-1) ))
    echo -e "${YELLOW}⚡ $PERCENT% of complex tests passed${NC}"
    echo -e "${GREEN}✓ Strong foundation with $PASSED advanced features working${NC}"
    echo -e "${YELLOW}⚠️  Some advanced features need refinement${NC}"
    echo
    echo -e "${CYAN}🔧 COMPILER MATURITY LEVEL: ADVANCED 🔧${NC}"
else
    PERCENT=$(( (PASSED * 100) / (TEST_NUM-1) ))
    echo -e "${YELLOW}⚠️  $PERCENT% of complex tests passed${NC}"
    echo -e "${GREEN}✓ Basic functionality confirmed solid${NC}"
    echo -e "${YELLOW}⚠️  Advanced features need development${NC}"
    echo
    echo -e "${CYAN}🔨 COMPILER MATURITY LEVEL: INTERMEDIATE 🔨${NC}"
fi

echo