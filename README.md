# 🔐 Secure DSL Compiler for RBAC/ABAC Policy Verification

<div align="center">

![C++17](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.10+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)
![Tests](https://img.shields.io/badge/Tests-5%2F5%20Passing-brightgreen?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-1.0.0-blue?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)

**A production-ready compiler for a security policy Domain-Specific Language (DSL), enabling automatic verification of Role-Based Access Control (RBAC) and Attribute-Based Access Control (ABAC) policies.**

[Getting Started](#-getting-started) · [DSL Syntax](#-dsl-syntax) · [CLI Reference](#-cli-reference) · [Output Formats](#-output-formats) · [Architecture](#-architecture)

</div>

---

## 📖 Overview

The **Policy DSL Compiler** transforms human-readable access control policies into verifiable, structured representations. It implements a full compilation pipeline — from lexical analysis to code generation — and automatically surfaces common security issues such as conflicting rules, privilege escalation paths, and redundant statements.

### Why This Exists

Access control policies in real systems are often scattered across configuration files, codebases, and documentation, making them hard to reason about holistically. This compiler gives you a **single, auditable source of truth** for your permissions model, with automated analysis built in.

### Key Capabilities

| Capability | Description |
|---|---|
| 🔍 **Conflict Detection** | Identifies allow/deny rules that contradict each other on the same action + resource |
| 🚨 **Privilege Escalation** | Flags dangerous permission patterns (e.g., `execute` on `system` resources) |
| ♻️ **Redundancy Detection** | Finds duplicate statements that bloat and obscure policies |
| 📊 **RBAC Matrix Generation** | Produces a role × resource access matrix for visualization |
| 🔄 **Multi-Format Output** | Exports results as matrix, JSON, or YAML for downstream tooling |
| 🔗 **CI/CD Ready** | Clean exit codes and `--warnings-as-errors` flag for pipeline integration |

---

## ✨ Features

- **Complete 6-stage compilation pipeline**: Lexing → Parsing → Semantic Analysis → Policy Analysis → Matrix Generation → Output
- **40+ token types** including keywords, operators, literals, and delimiters
- **Three output formats**: human-readable matrix, JSON (for APIs), YAML (for config pipelines)
- **Flexible analysis modes**: run all checks or target a specific analysis type
- **Verbose diagnostics**: per-phase execution reporting for debugging
- **Warning control**: suppress warnings or promote them to hard errors
- **Robust error handling**: line/column tracking, detailed error messages, graceful recovery
- **Modern C++17**: smart pointers, modular architecture, zero-warning clean build

---

## 🚀 Getting Started

### Prerequisites

- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.10+**
- **Make** or **Ninja**

### Quick Start

```bash
# Clone the repository
git clone https://github.com/sid-rudani/Secure-DSL-Compiler-for-RBAC-ABAC-Policy-Verification.git
cd Secure-DSL-Compiler-for-RBAC-ABAC-Policy-Verification

# Use the quick start script (recommended)
chmod +x QUICKSTART.sh && ./QUICKSTART.sh
```

### Manual Build

```bash
cd policy-dsl-compiler
mkdir build && cd build
cmake ..
make
```

This produces three executables in `build/`:

| Executable | Purpose |
|---|---|
| `compiler` | Main policy compiler (533 KB) |
| `test_lexer` | Lexer unit test suite (162 KB) |
| `test_parser` | Parser unit test suite (276 KB) |

### Run Tests

```bash
cd build
./test_lexer   # Lexer tokenization tests
./test_parser  # AST generation tests
```

---

## 📝 DSL Syntax

Policies are written in `.pol` files using a clean, human-readable syntax.

### Basic Structure

```
policy <policy_name> {
    <effect> action: <action_name> resource: <resource_name> role: <role_name>;
    <effect> action: <action_name> resource: <resource_name> role: <role_name> when: <condition>;
}
```

- **`<effect>`** — `allow` or `deny`
- **`<action_name>`** — e.g., `read`, `write`, `delete`, `execute`, `admin`
- **`<resource_name>`** — e.g., `file`, `database`, `system`
- **`<role_name>`** — e.g., `admin`, `user`, `guest`, `customer`
- **`when: <condition>`** — optional ABAC-style attribute condition

### Example Policies

**E-Commerce Policy**
```
policy ecommerce {
    allow action: read   resource: catalog  role: customer;
    allow action: write  resource: cart     role: customer;
    deny  action: delete resource: customer role: customer;
    allow action: admin  resource: catalog  role: admin;
    allow action: admin  resource: orders   role: admin;
}
```

**Hospital Data Policy**
```
policy hospital {
    allow action: read   resource: patient_records role: doctor;
    allow action: write  resource: patient_records role: doctor;
    deny  action: delete resource: patient_records role: nurse;
    allow action: read   resource: patient_records role: nurse;
}
```

**Cloud Infrastructure Policy (with ABAC condition)**
```
policy cloud_infra {
    allow action: execute resource: pipeline role: devops;
    deny  action: admin   resource: system   role: developer;
    allow action: read    resource: logs     role: developer when: environment == "staging";
}
```

### Supported Token Types

<details>
<summary>View all 40+ token types</summary>

```
KEYWORDS         — policy, user, role, resource, action, attribute,
                   rule, effect, allow, deny, subject, when
TYPE KEYWORDS    — int, string, bool, time
OPERATORS        — == != < > <= >=
LOGICAL          — and, or, not
DELIMITERS       — { } : ; . ( )
LITERALS         — identifiers, integers, strings, true, false
SPECIAL          — EOF, INVALID
```

</details>

---

## 💻 CLI Reference

```
Usage: ./compiler <input_file.pol> [options]
```

### Options

| Option | Description | Default |
|---|---|---|
| `-o <file>` | Write output to file instead of stdout | stdout |
| `--format <fmt>` | Output format: `matrix`, `json`, `yaml` | `matrix` |
| `--analysis <type>` | Analysis to run: `conflict`, `priv-esc`, `all` | `all` |
| `--warnings-as-errors` | Exit with code 1 on any warning | `false` |
| `--no-warnings` | Suppress all warning messages | `false` |
| `--verbose` | Print per-phase diagnostic output | `false` |
| `--help` | Show help message | — |
| `--version` | Show version information | — |

### Exit Codes

| Code | Meaning |
|---|---|
| `0` | Compilation successful, no errors |
| `1` | Compilation error, or warning triggered with `--warnings-as-errors` |

---

## 📤 Output Formats

### Matrix (Default)

Human-readable role × resource access matrix.

```
=== Access Control Matrix ===

Roles:     admin  user  guest
Resources: file   database  system

        file        database    system
admin   ALLOW       ALLOW       ALLOW
user    ALLOW       NONE        DENY
guest   NONE        NONE        DENY

=== Analysis Report ===
Conflicts Found:        0
Privilege Issues Found: 1
Redundancies Found:     0
```

### JSON

Structured output including the full AST, analysis results, and matrix — ideal for API consumption or programmatic processing.

```json
{
  "status": "success",
  "ast": {
    "policies": [
      {
        "name": "ecommerce",
        "statements": [
          { "effect": "ALLOW", "action": "read", "resource": "catalog", "role": "customer" }
        ]
      }
    ]
  },
  "analysis": {
    "conflicts": [],
    "privilegeIssues": [],
    "redundancies": []
  },
  "accessMatrix": {
    "roles": ["customer"],
    "resources": ["catalog", "cart", "customer"],
    "matrix": [[1, 1, -1]]
  }
}
```

### YAML

Compact summary output for CI/CD configuration pipelines.

```yaml
---
format: yaml
status: success
analysis:
  conflicts: 0
  privilege_issues: 1
  redundancies: 0
```

---

## 🔬 Policy Analysis

### Conflict Detection

Triggered when the same `action` + `resource` pair has both `allow` and `deny` effects within or across policies.

```bash
./compiler policy.pol --analysis conflict
```

### Privilege Escalation Detection

Flags statements that grant high-risk permissions (e.g., `admin`, `execute`) on sensitive resources (e.g., `system`, `database`), or that enable role promotion.

```bash
./compiler policy.pol --analysis priv-esc
```

### Redundancy Detection

Identifies statements that are exact duplicates, contributing no additional access control logic.

```bash
./compiler policy.pol --analysis all  # Default: runs all three
```

---

## 🏗 Architecture

The compiler follows a classical multi-stage pipeline:

```
.pol file
    │
    ▼
┌─────────────────────────┐
│  1. Lexical Analysis    │  Tokenizes source code (40+ token types)
│     src/lexer/          │  Line/column tracking, comment handling
└──────────┬──────────────┘
           │
    ▼
┌─────────────────────────┐
│  2. Parsing             │  Recursive descent → Abstract Syntax Tree
│     src/parser/         │  Error recovery with detailed messages
└──────────┬──────────────┘
           │
    ▼
┌─────────────────────────┐
│  3. Semantic Analysis   │  Symbol table, type checking, scope mgmt
│     src/semantic/       │  Validates actions, roles, resources
└──────────┬──────────────┘
           │
    ▼
┌─────────────────────────┐
│  4. Policy Analysis     │  Conflict detection
│     src/analyzer/       │  Privilege escalation detection
│                         │  Redundancy detection
└──────────┬──────────────┘
           │
    ▼
┌─────────────────────────┐
│  5. Code Generation     │  RBAC access matrix generation
│     src/codegen/        │  Role × Resource × Effect mapping
└──────────┬──────────────┘
           │
    ▼
┌─────────────────────────┐
│  6. Output              │  Matrix / JSON / YAML serialization
│     src/utils/          │  File or stdout
└─────────────────────────┘
```

### Project Structure

```
policy-dsl-compiler/
├── CMakeLists.txt
├── include/
│   ├── lexer/           # TokenType.h, Token.h, Lexer.h
│   ├── parser/          # ASTNodes.h, AST.h, Parser.h
│   ├── semantic/        # SymbolTable.h, TypeChecker.h, SemanticAnalyzer.h
│   ├── analyzer/        # ConflictDetector.h, PrivilegeEscalation.h, RedundancyDetector.h
│   ├── codegen/         # AccessMatrix.h, MatrixGenerator.h
│   └── utils/           # Error.h, SourceLocation.h, JsonOutput.h
├── src/
│   ├── lexer/           # Lexer.cpp, Token.cpp, TokenTypeHelper.cpp
│   ├── parser/          # Parser.cpp, AST.cpp
│   ├── semantic/        # SymbolTable.cpp, TypeChecker.cpp, SemanticAnalyzer.cpp
│   ├── analyzer/        # ConflictDetector.cpp, PrivilegeEscalation.cpp, RedundancyDetector.cpp
│   ├── codegen/         # AccessMatrix.cpp, MatrixGenerator.cpp
│   ├── utils/           # Error.cpp, JsonOutput.cpp
│   └── main.cpp         # Compiler driver + CLI
├── tests/
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   └── policies/        # basic_rbac.pol, privilege_escalation.pol, conflicts.pol
└── examples/            # sample1.pol (e-commerce), sample2.pol (hospital), sample3.pol (cloud)
```

---

## 🔌 Integration Examples

### Shell Script

```bash
#!/bin/bash
./compiler policy.pol --format json > policy_report.json

if [ $? -eq 0 ]; then
    echo "✅ Policy compilation successful"
    cat policy_report.json | jq '.analysis'
else
    echo "❌ Policy compilation failed"
    exit 1
fi
```

### Python

```python
import subprocess
import json

result = subprocess.run(
    ['./compiler', 'policy.pol', '--format', 'json'],
    capture_output=True, text=True
)

if result.returncode == 0:
    data = json.loads(result.stdout)
    conflicts = data['analysis']['conflicts']
    print(f"Conflicts found: {len(conflicts)}")
else:
    print("Compilation failed:", result.stderr)
```

### Makefile

```makefile
POLICIES = policy1.pol policy2.pol policy3.pol

.PHONY: validate
validate: $(POLICIES)
	@for policy in $(POLICIES); do \
		./compiler $$policy --warnings-as-errors || exit 1; \
	done
	@echo "✅ All policies valid"
```

### GitHub Actions

```yaml
- name: Validate Access Policies
  run: |
    cd policy-dsl-compiler/build
    ./compiler ../../policies/production.pol \
      --format json \
      --warnings-as-errors \
      -o policy_report.json
```

---

## 📊 Project Stats

| Metric | Value |
|---|---|
| Language | C++17 |
| Header Files | 16 |
| Source Files | 17 |
| Test Files | 2 |
| Example Policies | 6 |
| Lines of Code | 3,500+ |
| Compilation Stages | 6 |
| Token Types | 40+ |
| Test Pass Rate | 100% (5/5) |

---

## 🗺 Roadmap

Potential future enhancements:

- [ ] Policy inheritance and composition
- [ ] XACML / OASIS standard export
- [ ] Constraint solving for automatic conflict resolution
- [ ] ABAC attribute expression evaluator
- [ ] IDE / LSP plugin for `.pol` files
- [ ] Formal verification via SMT solver integration
- [ ] Web-based policy editor and visualizer
- [ ] Parallel compilation for large policy sets

---

## 📚 Documentation

| Document | Description |
|---|---|
| [`PROJECT_REPORT.md`](PROJECT_REPORT.md) | Full implementation report with technical details |
| [`IMPLEMENTATION_SUMMARY.md`](IMPLEMENTATION_SUMMARY.md) | Developer-focused implementation notes |
| [`VERIFICATION_CHECKLIST.md`](VERIFICATION_CHECKLIST.md) | Feature-by-feature verification status |
| [`CUSTOM_TEST_FILES.md`](CUSTOM_TEST_FILES.md) | Guide to writing custom test policies |
| [`folder_structure.md`](folder_structure.md) | Detailed directory layout |
| [`QUICKSTART.sh`](QUICKSTART.sh) | Automated build and demo script |

---

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add my feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

<div align="center">
Made with ❤️ by <a href="https://github.com/sid-rudani">sid-rudani</a>
</div>
