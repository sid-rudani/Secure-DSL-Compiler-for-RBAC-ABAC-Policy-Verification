document.addEventListener('DOMContentLoaded', () => {
    // 1. Initialize CodeMirror
    const editor = CodeMirror.fromTextArea(document.getElementById('code-editor'), {
        mode: "text/x-csrc",
        theme: "dracula",
        lineNumbers: true,
        matchBrackets: true,
        autoCloseBrackets: true,
        indentUnit: 4
    });

    // Handle Tabs
    const initTabs = (selector) => {
        const panes = document.querySelectorAll(selector);
        
        panes.forEach(mainNode => {
            mainNode.addEventListener('click', (e) => {
                if (e.target.classList.contains('tab-btn')) {
                    const targetId = 'content-' + e.target.getAttribute('data-tab');
                    const tabsContainer = e.target.closest('.tabs-container');
                    
                    if (!tabsContainer) return;
                    
                    // Update active tab button
                    tabsContainer.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
                    e.target.classList.add('active');

                    // Update content visibility inside this pane exactly
                    const contents = mainNode.querySelectorAll(':scope > .tab-content');
                    contents.forEach(c => {
                        if(c.id === targetId) {
                            c.classList.add('active');
                            if (c.id === 'content-code') {
                                setTimeout(() => editor.refresh(), 10);
                            } else if (c.id === 'content-graph' && window.network) {
                                setTimeout(() => window.network.fit(), 50);
                            }
                        } else {
                            c.classList.remove('active');
                        }
                    });
                }
            });
        });
    };

    initTabs('.left-pane');
    initTabs('.right-pane');

    // Builder Logic (Same)
    const initBuilder = () => {
        document.querySelectorAll('.btn-add').forEach(btn => {
            btn.addEventListener('click', (e) => {
                const targetId = e.target.getAttribute('data-target');
                const list = document.getElementById(targetId);
                const type = targetId.split('-')[0];
                const tplId = 'tpl-' + type.replace(/s$/, ''); // plural to singular trick
                let tmpl = document.getElementById(tplId);
                if (!tmpl && type === 'rules') tmpl = document.getElementById('tpl-rule');
                
                if (tmpl) {
                    const clone = tmpl.content.cloneNode(true);
                    list.appendChild(clone);
                }
            });
        });

        document.querySelector('.builder-content').addEventListener('click', (e) => {
            if (e.target.classList.contains('btn-remove')) {
                e.target.closest('.builder-item').remove();
            }
        });

        document.getElementById('btn-generate').addEventListener('click', () => {
            const data = {
                name: document.getElementById('builder-policy-name').value || 'generated_policy',
                users: [],
                roles: [],
                resources: [],
                actions: [],
                rules: []
            };

            document.getElementById('users-list').querySelectorAll('.builder-item').forEach(el => {
                const name = el.querySelector('.item-name').value;
                if (name) data.users.push({name});
            });

            document.getElementById('roles-list').querySelectorAll('.builder-item').forEach(el => {
                const name = el.querySelector('.item-name').value;
                const parent = el.querySelector('.item-parent').value;
                if (name) data.roles.push({name, parent});
            });

            document.getElementById('resources-list').querySelectorAll('.builder-item').forEach(el => {
                const name = el.querySelector('.item-name').value;
                if (name) data.resources.push({name});
            });

            document.getElementById('actions-list').querySelectorAll('.builder-item').forEach(el => {
                const name = el.querySelector('.item-name').value;
                if (name) data.actions.push({name});
            });

            document.getElementById('rules-list').querySelectorAll('.builder-item').forEach(el => {
                const rule = {
                    name: el.querySelector('.rule-name').value,
                    effect: el.querySelector('.rule-effect').value,
                    subject: el.querySelector('.rule-subject').value,
                    action: el.querySelector('.rule-action').value,
                    resource: el.querySelector('.rule-resource').value,
                    condition: el.querySelector('.rule-condition').value
                };
                if (rule.name && rule.subject && rule.action && rule.resource) {
                    data.rules.push(rule);
                }
            });

            fetch('/api/generate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            })
            .then(res => res.json())
            .then(res => {
                editor.setValue(res.source);
                document.querySelector('.left-pane .tab-btn[data-tab="code"]').click();
            })
            .catch(err => console.error(err));
        });
    };
    initBuilder();

    // Load Samples
    fetch('/api/samples')
        .then(res => res.json())
        .then(samples => {
            const select = document.getElementById('sample-select');
            window.loadedSamples = samples;
            samples.forEach((s, idx) => {
                const opt = document.createElement('option');
                opt.value = idx;
                opt.textContent = s.name;
                select.appendChild(opt);
            });
            
            select.addEventListener('change', (e) => {
                if (e.target.value !== "") {
                    const idx = parseInt(e.target.value);
                    editor.setValue(samples[idx].content);
                }
            });

            // Load first sample initially
            if(samples.length > 0) {
                editor.setValue(samples[0].content);
            }
        });

    // Compile & Analyze
    document.getElementById('btn-compile').addEventListener('click', () => {
        const source = editor.getValue();
        const btn = document.getElementById('btn-compile');
        btn.innerHTML = 'Compiling... <span style="font-size: 1.2rem">⏳</span>';
        btn.disabled = true;

        fetch('/api/compile', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ source: source, format: 'text', analysis: 'all' })
        })
        .then(res => res.json())
        .then(res => {
            const status = document.getElementById('compiler-status');
            const data = res.parsed;

            if (res.success) {
                status.innerHTML = '✨ Compilation Successful';
                status.className = 'status-banner status-success';
            } else {
                status.innerHTML = '🚨 Compilation Failed';
                status.className = 'status-banner status-error';
            }

            // Update UI
            updateAnalysisSection('errors', data.errors.concat(data.warnings));
            updateAnalysisSection('conflicts', data.conflicts);
            updateAnalysisSection('priv-esc', data.privilege_escalations);
            updateAnalysisSection('redundancies', data.redundancies);

            document.querySelector('#section-raw .insight-content').textContent = res.stdout + '\n' + res.stderr;
            
            if (data.access_matrix) {
                document.getElementById('matrix-display').textContent = data.access_matrix;
            } else {
                document.getElementById('matrix-display').textContent = 'No matrix generated due to errors.';
            }

            // Populate JSON tab
            try {
                if (res.native_json) {
                    const parsedJson = JSON.parse(res.native_json);
                    document.getElementById('json-display').textContent = JSON.stringify(parsedJson, null, 4);
                } else {
                    document.getElementById('json-display').textContent = 'No JSON generated.';
                }
            } catch (err) {
                document.getElementById('json-display').textContent = res.native_json || 'Failed to parse JSON output';
            }

            // Auto switch to output tab
            document.querySelector('.right-pane .tab-btn[data-tab="output"]').click();
            
            // Build Graph
            updateGraph(source);
        })
        .catch(err => {
            console.error(err);
            const status = document.getElementById('compiler-status');
            status.innerHTML = '✖ System/Connection Error';
            status.className = 'status-banner status-error';
            document.querySelector('#section-raw .insight-content').textContent = String(err);
        })
        .finally(() => {
            btn.innerHTML = 'Compile & Analyze ✨';
            btn.disabled = false;
        });
    });

    function updateAnalysisSection(id, items) {
        const sec = document.getElementById(`section-${id}`);
        if(!sec) return;
        const content = sec.querySelector('.insight-content');
        const badge = sec.querySelector('.badge');
        
        if (!items || items.length === 0) {
            content.textContent = 'No issues found.';
            if (badge) {
                badge.textContent = '0';
                badge.className = 'badge count-0';
            }
        } else {
            content.textContent = items.join('\n\n');
            if (badge) {
                badge.textContent = items.length;
                badge.className = items.length > 0 ? 'badge count-danger' : 'badge count-0';
            }
        }
    }

    // Vis.js Graph Update
    function updateGraph(source) {
        fetch('/api/graph', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ source })
        })
        .then(res => res.json())
        .then(graphData => {
            const container = document.getElementById('graph-container');
            const nodesArr = graphData.nodes.map(n => {
                let color, shape = 'box';
                if (n.type === 'user') { color = { background: '#1e1b4b', border: '#fca5a5' }; shape = 'ellipse'; }
                else if (n.type === 'role') { color = { background: '#1e3a8a', border: '#93c5fd' }; shape = 'box'; }
                else if (n.type === 'resource') { color = { background: '#064e3b', border: '#86efac' }; shape = 'box'; }
                else if (n.type === 'rule') { 
                    shape = 'hexagon';
                    if (n.effect === 'allow') color = { background: '#064e3b', border: '#10b981' };
                    else color = { background: '#7f1d1d', border: '#ef4444' };
                }
                
                return {
                    id: n.id,
                    label: n.label,
                    shape: shape,
                    color: color,
                    borderWidth: 2,
                    font: { color: '#ffffff', face: 'Outfit', size: 14 }
                };
            });

            const edgesArr = graphData.edges.map(e => {
                let color = '#94a3b8';
                let dashes = false;
                if (e.type === 'inheritance') { color = '#c084fc'; dashes = true; }
                if (e.type === 'permission') {
                    color = e.effect === 'allow' ? '#10b981' : '#ef4444';
                }
                
                return {
                    from: e.from,
                    to: e.to,
                    label: e.label,
                    color: { color: color },
                    dashes: dashes,
                    arrows: 'to',
                    font: { color: '#f8fafc', align: 'middle', size: 11, face: 'Outfit', background: 'rgba(0,0,0,0.5)' }
                };
            });

            if (graphData.cycles && graphData.cycles.length > 0) {
                graphData.cycles.forEach(cyclePath => {
                    for (let i = 0; i < cyclePath.length - 1; i++) {
                        const from = cyclePath[i];
                        const to = cyclePath[i+1];
                        const edgeIdx = edgesArr.findIndex(ed => ed.from === from && ed.to === to);
                        if (edgeIdx >= 0) {
                            edgesArr[edgeIdx].color = { color: '#f59e0b' }; 
                            edgesArr[edgeIdx].width = 3;
                            edgesArr[edgeIdx].label = 'CYCLE: ' + edgesArr[edgeIdx].label;
                        }
                    }
                });
            }

            const data = {
                nodes: new vis.DataSet(nodesArr),
                edges: new vis.DataSet(edgesArr)
            };
            
            const options = {
                physics: {
                    barnesHut: {
                        gravitationalConstant: -2000,
                        centralGravity: 0.3,
                        springLength: 150,
                        springConstant: 0.04,
                        avoidOverlap: 0.1
                    }
                },
                layout: {
                    hierarchical: false
                },
                interaction: { hover: true, tooltipDelay: 200 }
            };

            if (window.network) window.network.destroy();
            window.network = new vis.Network(container, data, options);
        })
        .catch(err => console.error("Graph Error:", err));
    }
});
