#!/usr/bin/env python3
"""
Policy DSL Compiler - Backend Server
Bridges the web frontend with the C++ compiler binary.
"""

import os
import subprocess
import tempfile
import json
import re
from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS

app = Flask(__name__, static_folder='.', static_url_path='')
CORS(app)

# Path to the compiled binary
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMPILER_PATH = os.path.join(BASE_DIR, 'build', 'compiler')


def run_compiler(source_code: str, fmt: str = 'text', analysis: str = 'all') -> dict:
    """Run the C++ compiler on source code and return structured output."""
    with tempfile.NamedTemporaryFile(mode='w', suffix='.pol', delete=False) as f:
        f.write(source_code)
        tmp_path = f.name

    try:
        args = [COMPILER_PATH, tmp_path, '--format', fmt, '--analysis', analysis]
        if fmt != 'json':
            args.append('--verbose')
            
        result = subprocess.run(
            args,
            capture_output=True,
            text=True,
            timeout=15
        )
        stdout = result.stdout
        stderr = result.stderr
        returncode = result.returncode

        return {
            'success': returncode == 0,
            'stdout': stdout,
            'stderr': stderr,
            'returncode': returncode
        }
    except subprocess.TimeoutExpired:
        return {'success': False, 'stdout': '', 'stderr': 'Compiler timed out after 15 seconds.', 'returncode': -1}
    except FileNotFoundError:
        return {'success': False, 'stdout': '', 'stderr': f'Compiler not found at {COMPILER_PATH}. Please build the project first.', 'returncode': -1}
    finally:
        os.unlink(tmp_path)


def parse_text_output(raw: str) -> dict:
    """Parse the text-format compiler output into structured sections."""
    sections = {
        'phases': [],
        'conflicts': [],
        'privilege_escalations': [],
        'redundancies': [],
        'access_matrix': '',
        'warnings': [],
        'errors': [],
        'success': False,
    }

    lines = raw.split('\n')
    matrix_lines = []
    in_matrix = False

    for line in lines:
        stripped = line.strip()

        if stripped.startswith('Phase'):
            sections['phases'].append(stripped)
        elif '[ERROR]' in stripped or 'Compilation failed' in stripped:
            sections['errors'].append(stripped)
        elif '[WARNING]' in stripped:
            sections['warnings'].append(stripped)
        elif '[CONFLICT]' in stripped:
            sections['conflicts'].append(stripped)
        elif '[PRIV_ESC]' in stripped:
            sections['privilege_escalations'].append(stripped)
        elif '[REDUNDANT]' in stripped:
            sections['redundancies'].append(stripped)
        elif '=== Access Matrix ===' in stripped:
            in_matrix = True
        elif stripped == 'Compilation successful!' or 'Compilation successful' in stripped:
            sections['success'] = True
            in_matrix = False

        if in_matrix:
            matrix_lines.append(line)

    sections['access_matrix'] = '\n'.join(matrix_lines)
    return sections


def pol_to_graph(source_code: str) -> dict:
    """Parse .pol source to extract roles, inheritance, and rules for graph rendering."""
    nodes = []
    edges = []
    node_ids = set()

    # Extract roles and inheritance
    role_pattern = re.compile(r'^\s*role\s+(\w+)(?:\s*<\s*(\w+))?\s*;', re.MULTILINE)
    for m in role_pattern.finditer(source_code):
        role_name = m.group(1)
        parent = m.group(2)
        if role_name not in node_ids:
            nodes.append({'id': role_name, 'label': role_name, 'type': 'role'})
            node_ids.add(role_name)
        if parent:
            if parent not in node_ids:
                nodes.append({'id': parent, 'label': parent, 'type': 'role'})
                node_ids.add(parent)
            edges.append({'from': role_name, 'to': parent, 'label': 'inherits', 'type': 'inheritance'})

    # Extract users
    user_pattern = re.compile(r'^\s*user\s+(\w+)\s*;', re.MULTILINE)
    for m in user_pattern.finditer(source_code):
        uid = m.group(1)
        if uid not in node_ids:
            nodes.append({'id': uid, 'label': uid, 'type': 'user'})
            node_ids.add(uid)

    # Extract resources
    resource_pattern = re.compile(r'^\s*resource\s+(\w+)\s*;', re.MULTILINE)
    for m in resource_pattern.finditer(source_code):
        rid = m.group(1)
        if rid not in node_ids:
            nodes.append({'id': rid, 'label': rid, 'type': 'resource'})
            node_ids.add(rid)

    # Extract rules (block-style)
    rule_pattern = re.compile(
        r'rule\s+(\w+)\s*\{[^}]*?effect\s*:\s*(allow|deny)[^}]*?subject\s*:\s*(?:role\s+)?(\w+)[^}]*?action\s*:\s*(\w+)[^}]*?resource\s*:\s*(\w+)',
        re.DOTALL | re.MULTILINE
    )
    for m in rule_pattern.finditer(source_code):
        rule_name = m.group(1)
        effect = m.group(2)
        subject = m.group(3)
        action = m.group(4)
        resource = m.group(5)

        rule_id = f'rule_{rule_name}'
        if rule_id not in node_ids:
            nodes.append({'id': rule_id, 'label': f'{rule_name}\n({effect})', 'type': 'rule', 'effect': effect})
            node_ids.add(rule_id)

        if subject in node_ids:
            edges.append({'from': subject, 'to': rule_id, 'label': 'subject_of', 'type': 'subject'})
        if resource in node_ids:
            edges.append({'from': rule_id, 'to': resource, 'label': action, 'type': 'permission', 'effect': effect})

    # Simple cycle detection via DFS
    adj = {}
    for e in edges:
        adj.setdefault(e['from'], []).append(e['to'])

    cycles = []
    visited = set()
    rec_stack = set()
    path = []

    def dfs(v):
        visited.add(v)
        rec_stack.add(v)
        path.append(v)
        for nb in adj.get(v, []):
            if nb not in visited:
                if dfs(nb):
                    return True
            elif nb in rec_stack:
                idx = path.index(nb)
                cycles.append(list(path[idx:]) + [nb])
        path.pop()
        rec_stack.discard(v)
        return False

    for node in [n['id'] for n in nodes]:
        if node not in visited:
            dfs(node)

    return {'nodes': nodes, 'edges': edges, 'cycles': cycles}


@app.route('/')
def index():
    return send_from_directory('.', 'index.html')


@app.route('/api/compile', methods=['POST'])
def compile_policy():
    data = request.get_json()
    source = data.get('source', '')
    fmt = data.get('format', 'text')
    analysis = data.get('analysis', 'all')

    if not source.strip():
        return jsonify({'success': False, 'error': 'No source code provided.'}), 400

    result = run_compiler(source, 'text', analysis)
    parsed = parse_text_output(result['stdout'] + '\n' + result['stderr'])
    
    # Also run compiler with format=json to get the raw AST or JSON structure
    json_result = run_compiler(source, 'json', analysis)
    native_json = json_result['stdout']

    # success is determined by return code
    parsed['success'] = result['success']

    return jsonify({
        'success': result['success'],
        'stdout': result['stdout'],
        'stderr': result['stderr'],
        'parsed': parsed,
        'native_json': native_json
    })


@app.route('/api/graph', methods=['POST'])
def get_graph():
    data = request.get_json()
    source = data.get('source', '')
    graph = pol_to_graph(source)
    return jsonify(graph)


@app.route('/api/generate', methods=['POST'])
def generate_policy():
    """Generate .pol source from the no-code builder form data."""
    data = request.get_json()
    policy_name = data.get('name', 'my_policy').replace(' ', '_')
    users = data.get('users', [])
    roles = data.get('roles', [])
    resources = data.get('resources', [])
    actions = data.get('actions', [])
    rules = data.get('rules', [])

    lines = [f'policy {policy_name} {{']

    if users:
        lines.append('')
        lines.append('    // Users')
        for u in users:
            lines.append(f'    user {u["name"]};')

    if roles:
        lines.append('')
        lines.append('    // Roles')
        for r in roles:
            if r.get('parent'):
                lines.append(f'    role {r["name"]} < {r["parent"]};')
            else:
                lines.append(f'    role {r["name"]};')

    if resources:
        lines.append('')
        lines.append('    // Resources')
        for res in resources:
            lines.append(f'    resource {res["name"]};')

    if actions:
        lines.append('')
        lines.append('    // Actions')
        for a in actions:
            lines.append(f'    action {a["name"]};')

    if rules:
        lines.append('')
        lines.append('    // Rules')
        for rule in rules:
            lines.append(f'    rule {rule["name"]} {{')
            lines.append(f'        effect: {rule["effect"]}')
            lines.append(f'        subject: role {rule["subject"]}')
            lines.append(f'        action: {rule["action"]}')
            lines.append(f'        resource: {rule["resource"]}')
            if rule.get('condition'):
                lines.append(f'        condition: "{rule["condition"]}"')
            lines.append('    }')
            lines.append('')

    lines.append('}')
    return jsonify({'source': '\n'.join(lines)})


@app.route('/api/samples', methods=['GET'])
def get_samples():
    samples_dir = os.path.join(BASE_DIR, 'examples', 'sample_policies')
    samples = []
    try:
        for fname in sorted(os.listdir(samples_dir)):
            if fname.endswith('.pol'):
                fpath = os.path.join(samples_dir, fname)
                with open(fpath) as f:
                    content = f.read()
                samples.append({'name': fname.replace('.pol', ''), 'content': content})
    except Exception as e:
        pass
    return jsonify(samples)


if __name__ == '__main__':
    print(f"Policy DSL Compiler Frontend")
    print(f"   Compiler: {COMPILER_PATH}")
    print(f"   Server:   http://localhost:5050")
    app.run(debug=True, port=5050, host='0.0.0.0')
