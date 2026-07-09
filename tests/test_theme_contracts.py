"""Contract tests for doxyYoda shipped templates, docs, and theme preference logic.

These tests drive real repo files (not reimplemented fixtures). Run from repo root:
  python -m pytest tests/ -q
  # or: python tests/test_theme_contracts.py
"""
from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "src" / "html" / "header.html"
FOOTER = ROOT / "src" / "html" / "footer.html"
VERSION = (ROOT / "version.txt").read_text(encoding="utf-8").strip()
THEME_PREF_JS = ROOT / "src" / "js" / "theme_pref.js"


def test_version_txt_semver():
    assert re.fullmatch(r"\d+\.\d+\.\d+", VERSION), VERSION


def test_header_is_html5_not_xhtml_transitional():
    text = HEADER.read_text(encoding="utf-8")
    assert "<!DOCTYPE html>" in text
    assert "xhtml1-transitional" not in text.lower()
    assert 'http-equiv="X-UA-Compatible"' not in text
    assert "charset=" in text or 'charset="' in text or "charset=" in text.replace(" ", "")


def test_header_has_theme_behaviors():
    text = HEADER.read_text(encoding="utf-8")
    assert "doxyYoda-theme" in text
    assert "localStorage" in text
    assert "prefers-color-scheme" in text
    assert ".fragment" in text
    assert "code-details" in text
    assert "copy-btn" in text
    assert "darkModeToggle" in text
    assert "$extrastylesheet" in text
    assert "$projectname" in text


def test_footer_closes_grid_and_has_attrib_class():
    text = FOOTER.read_text(encoding="utf-8")
    assert "grid-contents" in text
    assert "doxyyoda-attrib" in text
    assert "doxyYoda theme" in text or "doxyYoda" in text


def test_action_default_version_matches_version_txt():
    action = (ROOT / "action.yml").read_text(encoding="utf-8")
    m = re.search(r'default:\s*"([^"]+)"', action)
    assert m, "action.yml version default missing"
    assert m.group(1) == VERSION, f"action default {m.group(1)} != version.txt {VERSION}"


def test_docs_pin_current_version():
    for rel in ("readme.org", "MIGRATION.org"):
        body = (ROOT / rel).read_text(encoding="utf-8")
        # release/action examples should mention current version, not only ancient 0.1.0
        assert f"v{VERSION}" in body or f"@{VERSION}" in body or f"={VERSION}=" in body, rel
        # outdated exclusive pin of 0.1.0 in action uses line should be gone when version != 0.1.0
        if VERSION != "0.1.0":
            assert f"HaoZeke/doxyYoda@v0.1.0" not in body, f"{rel} still pins @v0.1.0"
            assert f"doxyYoda_0.1.0.tar.gz" not in body, f"{rel} still ships 0.1.0 tarball path"


def test_theme_pref_js_resolve_and_toggle():
    """Execute shipped theme_pref.js via node (the real module, not a reimplementation)."""
    assert THEME_PREF_JS.is_file(), THEME_PREF_JS
    script = f"""
const m = require({THEME_PREF_JS.as_posix()!r});
const assert = (c, msg) => {{ if (!c) {{ console.error(msg); process.exit(1); }} }};
assert(m.resolveThemePreference('dark', false) === 'dark', 'stored dark');
assert(m.resolveThemePreference('light', true) === 'light', 'stored light wins');
assert(m.resolveThemePreference(null, true) === 'dark', 'OS dark');
assert(m.resolveThemePreference(undefined, false) === 'light', 'OS light');
assert(m.nextThemeAfterToggle(true) === 'light', 'toggle to light');
assert(m.nextThemeAfterToggle(false) === 'dark', 'toggle to dark');
console.log('theme_pref ok');
"""
    r = subprocess.run(["node", "-e", script], capture_output=True, text=True)
    assert r.returncode == 0, r.stdout + r.stderr
    assert "theme_pref ok" in r.stdout


def test_scss_covers_modern_doxygen_selectors():
    scss_root = ROOT / "src" / "styles" / "scss"
    blob = "\n".join(p.read_text(encoding="utf-8") for p in scss_root.rglob("*.scss"))
    for sel in (
        ".memitem",
        ".fragment",
        ".code-details",
        ".copy-btn",
        ".dark-mode-toggle",
        ".textblock",
        ".compoundTemplParams",
        "table.doxtable",
        ".doxyyoda-attrib",
        "div.dynheader",
    ):
        assert sel in blob, f"missing SCSS coverage for {sel}"


if __name__ == "__main__":
    # Minimal runner without pytest dependency
    failed = 0
    for name, fn in list(globals().items()):
        if name.startswith("test_") and callable(fn):
            try:
                fn()
                print(f"PASS {name}")
            except Exception as exc:
                failed += 1
                print(f"FAIL {name}: {exc}")
    sys.exit(1 if failed else 0)
