# Policy DSL Compiler

A comprehensive C++ compiler for policy definition language (DSL) with support for lexical analysis, parsing, semantic analysis, conflict detection, privilege escalation detection, redundancy detection, and access matrix generation.

## Features

- **Lexical Analysis**: Tokenizes policy DSL source code
- **Parsing**: Builds Abstract Syntax Tree (AST)
- **Semantic Analysis**: Validates semantic correctness
- **Conflict Detection**: Identifies conflicting policy statements
- **Privilege Escalation Detection**: Detects potential security issues
- **Redundancy Detection**: Finds redundant policy statements
- **Access Matrix Generation**: Generates role-based access control matrices
- **JSON Output**: Full support for JSON formatted output

## Building

### Requirements
- CMake 3.10+
- C++17 compatible compiler
- macOS, Linux, or Windows with appropriate build tools

### Build Instructions

```bash
cd policy-dsl-compiler
mkdir build
cd build
cmake ..
make
```

This will generate:
- `compiler` - Main executable
- `test_lexer` - Lexer tests
- `test_parser` - Parser tests

## Usage

### Basic Compilation

```bash
./compiler <input_file.pol> [options]
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `-o <file>` | Specify output file for results |
| `--format <fmt>` | Output format: `matrix`, `json`, `yaml` (default: matrix) |
| `--analysis <type>` | Run specific analysis: `conflict`, `priv-esc`, `all` (default: all) |
| `--warnings-as-errors` | Treat warnings as errors and exit with code 1 |
| `--no-warnings` | Suppress warning messages |
| `--verbose` | Enable verbose diagnostic output with compilation phases |
| `--help` | Display help message and exit |
| `--version` | Display version information and exit |

### Examples

```bash
# Default: access control matrix to console
./compiler policy.pol

# Output access matrix to file
./compiler policy.pol -o matrix.txt

# JSON output to console
./compiler policy.pol --format json

# JSON output to file
./compiler policy.pol --format json -o result.json

# YAML output with verbose logging
./compiler policy.pol --format yaml --verbose

# Run only conflict analysis
./compiler policy.pol --analysis conflict

# Run privilege escalation analysis only
./compiler policy.pol --analysis priv-esc

# Full analysis with verbose output
./compiler policy.pol --analysis all --verbose

# Treat warnings as errors
./compiler policy.pol --warnings-as-errors
```

## Policy DSL Syntax

### Keywords
- `policy` - Policy definition
- `allow`, `deny` - Effect
- `action` - Action definition
- `resource` - Resource definition
- `role` - Role definition
- `subject` - Subject specification
- `when` - Condition specification
- `and`, `or`, `not` - Logical operators

### Types
- `int` - Integer type
- `string` - String type
- `bool` - Boolean type
- `time` - Time type

### Operators
- `==` - Equality
- `!=` - Not equals
- `<` - Less than
- `>` - Greater than
- `<=` - Less than or equal
- `>=` - Greater than or equal

### Example Policy

```
policy ecommerce_access {
    allow action: read resource: catalog role: customer;
    allow action: write resource: cart role: customer;
    allow action: read resource: orders role: customer when: "owner == user.id";
    deny action: delete resource: customer role: customer;
}
```

## Project Structure

```
policy-dsl-compiler/
├── CMakeLists.txt              # Build configuration
├── include/
│   ├── lexer/                  # Lexical analysis
│   │   ├── Token.h
│   │   ├── Lexer.h
│   │   └── TokenType.h
│   ├── parser/                 # Parsing & AST
│   │   ├── AST.h
│   │   ├── Parser.h
│   │   └── ASTNodes.h
│   ├── semantic/               # Semantic analysis
│   │   ├── SymbolTable.h
│   │   ├── SemanticAnalyzer.h
│   │   └── TypeChecker.h
│   ├── analyzer/               # Policy analysis
│   │   ├── ConflictDetector.h
│   │   ├── PrivilegeEscalation.h
│   │   └── RedundancyDetector.h
│   ├── codegen/                # Code generation
│   │   ├── AccessMatrix.h
│   │   └── MatrixGenerator.h
│   └── utils/
│       ├── Error.h
│       ├── SourceLocation.h
│       └── JsonOutput.h
├── src/
│   ├── lexer/                  # Lexer implementation
│   ├── parser/                 # Parser implementation
│   ├── semantic/               # Semantic analysis implementation
│   ├── analyzer/               # Analysis implementations
│   ├── codegen/                # Code generation implementation
│   ├── utils/                  # Utility implementations
│   └── main.cpp                # Compiler driver
├── tests/
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   └── policies/               # Sample policies
│       ├── basic_rbac.pol
│       ├── privilege_escalation.pol
│       └── conflicts.pol
└── examples/
    └── sample_policies/        # Example policies
        ├── sample1.pol
        ├── sample2.pol
        └── sample3.pol
```

## Compilation Pipeline

1. **Lexical Analysis** - Source code → Tokens
2. **Parsing** - Tokens → Abstract Syntax Tree
3. **Semantic Analysis** - Type checking, symbol validation
4. **Analysis Phase**
   - Conflict detection
   - Privilege escalation detection
   - Redundancy detection
5. **Code Generation** - Access matrix generation
6. **Output** - Standard or JSON format

## Output Formats

The compiler supports three output formats:

### 1. Matrix Format (Default)
Displays role-based access control matrix with policies and analysis results.

```
=== Access Control Matrix ===

Roles: admin user guest 
Resources: file database system 

Matrix (1=allow, -1=deny, 0=no access):
        file	database	system	
admin   ALLOW   ALLOW       ALLOW   
user    ALLOW   NONE        DENY    
guest   NONE    NONE        DENY    

=== Analysis Report ===
Conflicts Found: 0
Privilege Issues Found: 1
...
```

### 2. JSON Format
Complete JSON output with AST, analysis results, and matrix.

```bash
./compiler policy.pol --format json
```

Output includes:
- `status`: "success" or "error"
- `ast`: Complete abstract syntax tree of parsed policies
- `analysis`: All detected conflicts, privilege issues, redundancies
- `accessMatrix`: Role-resource matrix with access levels

### 3. YAML Format
Structured YAML output for configuration management tools.

```bash
./compiler policy.pol --format yaml
```

Output includes compilation status and summary statistics.

## Analysis Types

You can run specific analyses or all at once:

```bash
# Conflict detection only
./compiler policy.pol --analysis conflict

# Privilege escalation detection only
./compiler policy.pol --analysis priv-esc

# All analysis types (default)
./compiler policy.pol --analysis all
```

## Examples

### Example 1: Basic RBAC Policy

```
policy basic_rbac {
    allow action: read resource: file role: user;
    allow action: write resource: file role: editor;
    allow action: delete resource: file role: admin;
    deny action: delete resource: system role: user;
}
```

Compile:
```bash
./compiler policies/basic_rbac.pol --json
```

### Example 2: E-commerce Access Control

```
policy ecommerce_access {
    allow action: read resource: catalog role: customer;
    allow action: write resource: cart role: customer;
    allow action: read resource: orders role: customer;
    allow action: write resource: order role: customer;
    allow action: read resource: orders role: admin;
    allow action: write resource: orders role: admin;
    deny action: delete resource: customer role: customer;
}
```

### Example 3: Hospital Data Access

```
policy hospital_access {
    allow action: read resource: patient_records role: doctor;
    allow action: write resource: patient_records role: doctor;
    allow action: read resource: patient_records role: nurse;
    deny action: read resource: financial role: patient;
}
```

## Testing

Run the test suite:

```bash
cd build
ctest
```

Individual tests:

```bash
./test_lexer
./test_parser
```

## Command Line Options

| Option | Description |
|--------|-------------|
| `--json` | Output results in JSON format |
| `--verbose` | Enable verbose logging |
| `--output <file>` | Write output to file |
| `--help` | Show help message |

## Architecture

### Lexer
- Handles tokenization of source code
- Supports comments (single-line `//` and multi-line `/* */`)
- Recognizes keywords, operators, literals, and delimiters
- Provides detailed source location tracking

### Parser
- Builds Abstract Syntax Tree (AST)
- Implements recursive descent parsing
- Error recovery and reporting

### Semantic Analyzer
- Symbol table management
- Type checking
- Semantic validation

### Analyzers
- **ConflictDetector**: Identifies conflicting allow/deny rules
- **PrivilegeEscalation**: Detects potential privilege escalation
- **RedundancyDetector**: Finds redundant statements

### Code Generator
- **AccessMatrix**: Generates role-based access control matrix
- **MatrixGenerator**: Builds matrix from AST

## Error Handling

The compiler provides comprehensive error handling with:
- Line and column information
- Detailed error messages
- Error categorization (lexical, syntax, semantic, type, conflict)
- Graceful error recovery

## Performance

- Efficient single-pass lexical analysis
- Linear-time parsing
- Polynomial-time conflict detection
- Suitable for small to medium-sized policy documents

## Future Enhancements

- [ ] Support for policy inheritance
- [ ] Constraint solving for conflict resolution
- [ ] XACML/JSON Policy export
- [ ] IDE integration
- [ ] Policy optimization
- [ ] Formal verification support

## License

Educational Project

## Author

Policy DSL Compiler Development Team

## Support

For issues, bugs, or feature requests, please refer to the project documentation.
