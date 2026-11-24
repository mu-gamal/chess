#!/usr/bin/env python3
"""Stamp every local CSS/JS reference in log/index.html with ?v=<content hash>.
The hash is computed from the referenced files, so it changes only when they do.
Run manually or via the pre-commit hook (.git/hooks/pre-commit)."""
import hashlib, os, re, sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "log")
INDEX = os.path.join(ROOT, "index.html")
REF = re.compile(r'''(<(?:script|link)\b[^>]*?\b(?:src|href)=")([^"?#]+\.(?:js|css))(\?v=[^"]*)?(")''')

html = open(INDEX, encoding="utf-8").read()
files = sorted({m.group(2) for m in REF.finditer(html)
                if not re.match(r"^(https?:)?//", m.group(2)) and not m.group(2).startswith("lib/") and os.path.isfile(os.path.join(ROOT, m.group(2)))})
h = hashlib.sha1()
for f in files:
    h.update(f.encode())
    h.update(open(os.path.join(ROOT, f), "rb").read())
ver = h.hexdigest()[:8]

def sub(m):
    if m.group(2) not in files:
        return m.group(0).replace("?v=" + (m.group(3) or "")[3:], "") if m.group(3) else m.group(0)
    return f"{m.group(1)}{m.group(2)}?v={ver}{m.group(4)}"

new = REF.sub(sub, html)
if new != html:
    open(INDEX, "w", encoding="utf-8").write(new)
print(f"version {ver} ({len(files)} files)", "updated" if new != html else "unchanged")
