policy-dsl-compiler/
├── CMakeLists.txt                 # Build configuration
├── include/
│   ├── lexer/
│   │   ├── Token.h
│   │   ├── Lexer.h
│   │   └── TokenType.h
│   ├── parser/
│   │   ├── AST.h
│   │   ├── Parser.h
│   │   └── ASTNodes.h
│   ├── semantic/
│   │   ├── SymbolTable.h
│   │   ├── SemanticAnalyzer.h
│   │   └── TypeChecker.h
│   ├── analyzer/
│   │   ├── ConflictDetector.h
│   │   ├── PrivilegeEscalation.h
│   │   └── RedundancyDetector.h
│   ├── codegen/
│   │   ├── AccessMatrix.h
│   │   └── MatrixGenerator.h
│   └── utils/
│       ├── Error.h
│       └── SourceLocation.h
├── src/
│   ├── lexer/
│   ├── parser/
│   ├── semantic/
│   ├── analyzer/
│   ├── codegen/
│   └── main.cpp
├── tests/
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   └── policies/
│       ├── basic_rbac.pol
│       ├── privilege_escalation.pol
│       └── conflicts.pol
└── examples/
    └── sample_policies/