#!/bin/bash

# Error Handling Test Suite for Roadmap Compiler
# Tests compiler's ability to detect and report various error conditions

echo "╔══════════════════════════════════════════════════════════════════╗"
echo "║              ROADMAP COMPILER - ERROR HANDLING TESTS            ║"
echo "║               Testing Error Detection & Reporting               ║"
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

echo -e "${GREEN}✓ Testing error detection capabilities across all phases${NC}"
echo

TEST_NUM=1
LEXICAL_ERRORS=0
SYNTAX_ERRORS=0  
SEMANTIC_ERRORS=0
CODE_GEN_ERRORS=0
TOTAL_ERRORS_DETECTED=0

# Function to run error test (expects failure at specific phase)
run_error_test() {
    local test_name="$1"
    local test_input="$2"
    local expected_phase="$3"
    local error_type="$4"
    local description="$5"
    
    echo -e "${CYAN}═══ Error Test $TEST_NUM: $test_name ═══${NC}"
    echo -e "${BLUE}Input:${NC}"
    echo "$test_input"
    echo -e "${BLUE}Expected:${NC} Should fail in $expected_phase with $error_type"
    echo -e "${BLUE}Description:${NC} $description"
    echo
    
    # Create test file
    echo "$test_input" > test.txt
    
    echo -e "${YELLOW}Running pipeline until error...${NC}"
    
    local phase_failed=""
    local error_detected=0
    
    # Phase 1: Lexical Analysis
    echo "Phase 1: Lexical Analysis"
    cd phase1
    if ./lexer > lexer_error.log 2>&1; then
        echo "✓ Lexical analysis passed"
        TOKEN_COUNT=$(grep -v '^#' tokens.txt | grep -v '^$' | grep -v '^EOF' | wc -l 2>/dev/null || echo "0")
        echo "  Generated $TOKEN_COUNT tokens"
    else
        echo -e "${RED}❌ Lexical analysis failed${NC}"
        if [ "$expected_phase" = "Phase1" ]; then
            echo -e "${GREEN}✅ Expected error detected!${NC}"
            echo "Error details:"
            cat lexer_error.log | head -3 | sed 's/^/  /'
            ((LEXICAL_ERRORS++))
            ((TOTAL_ERRORS_DETECTED++))
            error_detected=1
            phase_failed="Phase1"
        else
            echo -e "${YELLOW}⚠️  Unexpected lexical error${NC}"
        fi
        cd ..
        rm -f lexer_error.log
        echo
        ((TEST_NUM++))
        return
    fi
    cd ..
    
    # Phase 2: Syntax Analysis
    echo "Phase 2: Syntax Analysis"
    cd phase2
    if ./parser_test ../phase1/tokens.txt > parser_error.log 2>&1; then
        echo "✓ Syntax analysis passed"
        AST_LINES=$(wc -l < ast.txt 2>/dev/null || echo "0")
        echo "  AST generated: $AST_LINES lines"
    else
        echo -e "${RED}❌ Syntax analysis failed${NC}"
        if [ "$expected_phase" = "Phase2" ]; then
            echo -e "${GREEN}✅ Expected error detected!${NC}"
            echo "Parse error details:"
            cat parser_error.log | head -3 | sed 's/^/  /'
            ((SYNTAX_ERRORS++))
            ((TOTAL_ERRORS_DETECTED++))
            error_detected=1
            phase_failed="Phase2"
        else
            echo -e "${YELLOW}⚠️  Unexpected syntax error${NC}"
        fi
        cd ..
        rm -f parser_error.log
        echo
        ((TEST_NUM++))
        return
    fi
    cd ..
    
    # Phase 3: Semantic Analysis
    echo "Phase 3: Semantic Analysis"
    cd phase3
    if ./semantic_analyzer ../phase2/ast.txt > semantic_error.log 2>&1; then
        # Check if semantic errors were reported but analysis continued
        if grep -q "error\|Error\|❌" semantic_error.log; then
            echo -e "${RED}❌ Semantic errors detected${NC}"
            if [ "$expected_phase" = "Phase3" ]; then
                echo -e "${GREEN}✅ Expected semantic errors detected!${NC}"
                echo "Semantic error details:"
                grep -i "error\|❌" semantic_error.log | head -3 | sed 's/^/  /'
                ((SEMANTIC_ERRORS++))
                ((TOTAL_ERRORS_DETECTED++))
                error_detected=1
                phase_failed="Phase3"
            fi
        else
            echo "✓ Semantic analysis passed"
        fi
    else
        echo -e "${RED}❌ Semantic analysis failed${NC}"
        if [ "$expected_phase" = "Phase3" ]; then
            echo -e "${GREEN}✅ Expected error detected!${NC}"
            echo "Semantic error details:"
            cat semantic_error.log | head -3 | sed 's/^/  /'
            ((SEMANTIC_ERRORS++))
            ((TOTAL_ERRORS_DETECTED++))
            error_detected=1
            phase_failed="Phase3"
        fi
        cd ..
        rm -f semantic_error.log
        echo
        ((TEST_NUM++))
        return
    fi
    cd ..
    
    # Phase 4: Code Generation (only if semantic analysis passed)
    if [ "$phase_failed" != "Phase3" ]; then
        echo "Phase 4: Code Generation"
        cd phase4
        if ./code_generator ../phase3/annotated_ast.txt > codegen_error.log 2>&1; then
            echo "✓ Code generation passed"
            INST_COUNT=$(grep -c "^    " program.s 2>/dev/null || echo "0")
            echo "  Assembly instructions: $INST_COUNT"
            
            # Test assembly stage
            if as -64 program.s -o program.o 2>assembly_error.log; then
                if ld program.o -o test_program 2>link_error.log; then
                    echo "✓ Assembly and linking successful"
                    if [ "$expected_phase" = "Phase4" ]; then
                        echo -e "${YELLOW}⚠️  Expected code generation error but test passed${NC}"
                    fi
                else
                    echo -e "${RED}❌ Linking failed${NC}"
                    if [ "$expected_phase" = "Phase4" ]; then
                        echo -e "${GREEN}✅ Expected linking error detected!${NC}"
                        echo "Linking error:"
                        cat link_error.log | head -2 | sed 's/^/  /'
                        ((CODE_GEN_ERRORS++))
                        ((TOTAL_ERRORS_DETECTED++))
                        error_detected=1
                    fi
                fi
            else
                echo -e "${RED}❌ Assembly failed${NC}"
                if [ "$expected_phase" = "Phase4" ]; then
                    echo -e "${GREEN}✅ Expected assembly error detected!${NC}"
                    echo "Assembly error:"
                    cat assembly_error.log | head -2 | sed 's/^/  /'
                    ((CODE_GEN_ERRORS++))
                    ((TOTAL_ERRORS_DETECTED++))
                    error_detected=1
                fi
            fi
        else
            echo -e "${RED}❌ Code generation failed${NC}"
            if [ "$expected_phase" = "Phase4" ]; then
                echo -e "${GREEN}✅ Expected error detected!${NC}"
                echo "Code generation error:"
                cat codegen_error.log | head -3 | sed 's/^/  /'
                ((CODE_GEN_ERRORS++))
                ((TOTAL_ERRORS_DETECTED++))
                error_detected=1
            fi
        fi
        cd ..
        rm -f codegen_error.log assembly_error.log link_error.log
    fi
    
    # Summary for this test
    if [ $error_detected -eq 1 ]; then
        echo -e "${GREEN}✅ ERROR DETECTION TEST PASSED${NC}"
    else
        echo -e "${YELLOW}⚠️  Expected error not detected - compiler may be too permissive${NC}"
    fi
    
    # Cleanup
    rm -f test.txt program.s program.o test_program
    
    echo
    ((TEST_NUM++))
}

echo "Starting error handling test suite..."
echo

# Test 1: Lexical Error - Invalid characters
run_error_test "Invalid Characters" \
"A = TRUE
B = @#$%^
result = A AND B" \
"Phase1" \
"Invalid character error" \
"Special characters should be rejected by lexer"

# Test 2: Lexical Error - Unclosed string (if strings were supported)
run_error_test "Invalid Token Sequence" \
"A = TRUE =
B = FALSE
result = A AND B" \
"Phase1" \
"Invalid token sequence" \
"Malformed assignment with double equals"

# Test 3: Syntax Error - Missing operand
run_error_test "Missing Operand" \
"A = TRUE
B = FALSE
result = A AND" \
"Phase2" \
"Parse error - missing operand" \
"AND operator without right operand should fail parsing"

# Test 4: Syntax Error - Unmatched parentheses
run_error_test "Unmatched Parentheses" \
"A = TRUE
B = FALSE
result = (A AND B" \
"Phase2" \
"Parse error - unmatched parentheses" \
"Missing closing parenthesis should cause parse error"

# Test 5: Syntax Error - Invalid operator sequence
run_error_test "Invalid Operator Sequence" \
"A = TRUE
B = FALSE
result = A AND OR B" \
"Phase2" \
"Parse error - invalid operator sequence" \
"Two consecutive operators should fail parsing"

# Test 6: Semantic Error - Undefined variable
run_error_test "Undefined Variable" \
"A = TRUE
result = A AND undefined_var" \
"Phase3" \
"Undefined variable error" \
"Using undefined variable should be caught by semantic analysis"

# Test 7: Semantic Error - Type mismatch (if types were stricter)
run_error_test "Reserved Keyword Misuse" \
"TRUE = FALSE
result = TRUE AND FALSE" \
"Phase3" \
"Invalid assignment to reserved word" \
"Assigning to reserved keywords should be semantic error"

# Test 8: Complex Syntax Error - Malformed expression
run_error_test "Malformed Complex Expression" \
"A = TRUE
B = FALSE
result = A AND (B OR AND C)" \
"Phase2" \
"Parse error in nested expression" \
"Invalid nested expression structure should fail parsing"

# Test 9: Empty input error
run_error_test "Empty Input" \
"" \
"Phase1" \
"Empty input error" \
"Empty input should be handled gracefully"

# Test 10: Only whitespace
run_error_test "Whitespace Only Input" \
"   
   
   " \
"Phase1" \
"No meaningful tokens" \
"Input with only whitespace should be handled"

# Cleanup
rm -f test.txt

# Print detailed results
echo -e "${PURPLE}══════════════════════════════════════════════════════════════════${NC}"
echo -e "${PURPLE}                    ERROR HANDLING TEST RESULTS                   ${NC}"
echo -e "${PURPLE}══════════════════════════════════════════════════════════════════${NC}"
echo
echo "Total error tests: $((TEST_NUM-1))"
echo -e "Total errors detected: ${GREEN}$TOTAL_ERRORS_DETECTED${NC}"
echo
echo "Errors detected by phase:"
echo -e "  Phase 1 (Lexical):   ${GREEN}$LEXICAL_ERRORS${NC}"
echo -e "  Phase 2 (Syntax):    ${GREEN}$SYNTAX_ERRORS${NC}"
echo -e "  Phase 3 (Semantic):  ${GREEN}$SEMANTIC_ERRORS${NC}"
echo -e "  Phase 4 (Code Gen):  ${GREEN}$CODE_GEN_ERRORS${NC}"
echo

# Evaluate error handling capability
if [ $TOTAL_ERRORS_DETECTED -ge 7 ]; then
    echo -e "${GREEN}🎉 EXCELLENT ERROR HANDLING! 🎉${NC}"
    echo -e "${GREEN}Your compiler has robust error detection across all phases!${NC}"
    echo
    echo -e "${CYAN}🔍 ERROR HANDLING MATURITY: EXPERT 🔍${NC}"
elif [ $TOTAL_ERRORS_DETECTED -ge 5 ]; then
    echo -e "${YELLOW}⚡ GOOD ERROR HANDLING ⚡${NC}"
    echo -e "${GREEN}Most error conditions properly detected${NC}"
    echo
    echo -e "${CYAN}🔍 ERROR HANDLING MATURITY: ADVANCED 🔍${NC}"
elif [ $TOTAL_ERRORS_DETECTED -ge 3 ]; then
    echo -e "${YELLOW}⚠️  MODERATE ERROR HANDLING ⚠️${NC}"
    echo -e "${YELLOW}Some errors detected, room for improvement${NC}"
    echo
    echo -e "${CYAN}🔍 ERROR HANDLING MATURITY: INTERMEDIATE 🔍${NC}"
else
    echo -e "${RED}❌ LIMITED ERROR HANDLING${NC}"
    echo -e "${YELLOW}Consider improving error detection mechanisms${NC}"
    echo
    echo -e "${CYAN}🔍 ERROR HANDLING MATURITY: BASIC 🔍${NC}"
fi

echo
echo -e "${BLUE}Error handling tests help ensure your compiler is robust${NC}"
echo -e "${BLUE}and provides meaningful feedback for debugging!${NC}"
echo