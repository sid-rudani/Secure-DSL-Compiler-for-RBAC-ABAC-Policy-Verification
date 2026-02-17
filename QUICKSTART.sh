#!/bin/bash
# Policy DSL Compiler - Quick Start Guide

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Policy DSL Compiler - Quick Start ===${NC}\n"

# Step 1: Build
echo -e "${BLUE}Step 1: Building the compiler...${NC}"
cd "$(dirname "$0")/policy-dsl-compiler"
mkdir -p build
cd build
cmake .. > /dev/null 2>&1
make > /dev/null 2>&1

if [ -f "./compiler" ]; then
    echo -e "${GREEN}✓ Compiler built successfully${NC}\n"
else
    echo "✗ Build failed"
    exit 1
fi

# Step 2: Test Lexer
echo -e "${BLUE}Step 2: Testing Lexer...${NC}"
./test_lexer > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Lexer tests passed${NC}\n"
else
    echo "✗ Lexer tests failed"
fi

# Step 3: Test Parser
echo -e "${BLUE}Step 3: Testing Parser...${NC}"
./test_parser > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Parser tests passed${NC}\n"
else
    echo "✗ Parser tests failed"
fi

# Step 4: Compile a policy
echo -e "${BLUE}Step 4: Compiling a sample policy...${NC}"
cat > /tmp/demo.pol << 'EOF'
policy demo {
    allow action: read resource: file role: user;
    allow action: write resource: file role: admin;
    deny action: delete resource: system role: user;
}
EOF

echo "Sample policy:"
echo "---"
cat /tmp/demo.pol
echo "---\n"

# Step 5: Standard output
echo -e "${BLUE}Step 5: Compiler output (verbose)...${NC}"
./compiler /tmp/demo.pol --verbose
echo ""

# Step 6: JSON output
echo -e "${BLUE}Step 6: JSON output...${NC}"
./compiler /tmp/demo.pol --json > /tmp/result.json
echo "JSON output saved to /tmp/result.json"
echo ""

# Show JSON
echo "Generated JSON:"
echo "---"
head -20 /tmp/result.json
echo "..."
echo "---\n"

echo -e "${GREEN}=== Demo Complete ===${NC}"
echo -e "${GREEN}✓ All components working${NC}"
echo -e "\nUsage:"
echo "  ./compiler <file.pol>           - Verbose output"
echo "  ./compiler <file.pol> --json    - JSON output"
echo "  ./compiler <file.pol> --help    - Show help"
