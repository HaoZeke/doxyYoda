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
    assert "$mermaidjs" in text
    assert "HTML_DYNAMIC_SECTIONS" in text
    assert "HTML_CODE_FOLDING" in text
    assert "codefolding.js" in text
    assert 'name="doxygen-page-layout"' in text
    assert "<!--END TITLEAREA-->" in text
    assert "<!--BEGIN PROJECT_BRIEF-->" in text
    assert "clipboard.js" not in text


def test_header_code_folding_does_not_force_close_fragments():
    """Contract: do not wrap every .fragment in a forced-closed details.

    Page-sized *_source.html listings and INLINE_SOURCES use .fragment; a
    closed <details> hides them after first paint. Folding stays optional.
    """
    text = HEADER.read_text(encoding="utf-8")
    fold_idx = text.find('querySelectorAll(".fragment")')
    copy_idx = text.find('querySelectorAll("div.fragment")')
    assert fold_idx != -1, "folding querySelectorAll(.fragment) missing"
    assert copy_idx != -1, "copy querySelectorAll(div.fragment) missing"
    assert fold_idx < copy_idx, "folding loop must precede copy-button loop"
    block = text[fold_idx:copy_idx]

    # Skip a fragment already inside details.code-details
    assert "details.code-details" in block
    assert "closest" in block

    # Skip native Doxygen listings: .foldopen sits inside the fragment
    assert ".foldopen" in block

    # Wrapped details start open so listings stay visible; user can collapse
    assert re.search(r"details\.open\s*=\s*true", block), block
    assert not re.search(r"details\.open\s*=\s*false", block)
    assert 'createElement("details")' in block
    assert "summary" in block
    assert "Code" in block


def test_header_drops_doxygen_clipboard_js():
    """Stock clipboard.js injects an unstyled div.clipboard beside .copy-btn.

    The theme owns one copy control. Copied text is source only: clone the
    fragment and drop .copy-btn so the button label is not included.
    """
    text = HEADER.read_text(encoding="utf-8")
    assert "COPY_CLIPBOARD" not in text
    assert "clipboard.js" not in text
    assert "copy-btn" in text

    copy_idx = text.find('querySelectorAll("div.fragment")')
    assert copy_idx != -1, "copy querySelectorAll(div.fragment) missing"
    block = text[copy_idx:]
    assert re.search(r"cloneNode\s*\(\s*true\s*\)", block), block
    assert re.search(r'querySelector\(\s*["\']\.copy-btn["\']\s*\)', block)
    assert re.search(r"\.remove\s*\(", block)


def test_footer_closes_grid_and_has_attrib_class():
    text = FOOTER.read_text(encoding="utf-8")
    assert "grid-contents" in text
    assert "doxyyoda-attrib" in text
    # Doxygen 1.17+ already closed #doc-content / #container before the footer.
    assert "close #doc-content" not in text
    assert re.search(r'id="doc-content"', text) is None
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


def test_scss_dark_tokens_key_html_dark_and_dark_mode():
    """Doxygen TOGGLE JS sets html.dark-mode; theme toggle sets html.dark.

    Both class names must apply the same dark token block. Keep the
    prefers-color-scheme path so AUTO / OS preference still works.
    """
    variables = (ROOT / "src" / "styles" / "scss" / "_variables.scss").read_text(
        encoding="utf-8"
    )
    assert re.search(r"@media\s*\(\s*prefers-color-scheme\s*:\s*dark\s*\)", variables)
    assert re.search(r":root:not\(\s*\.light\s*\)", variables)
    assert re.search(
        r"html\.dark\s*,\s*html\.dark-mode\s*\{|html\.dark-mode\s*,\s*html\.dark\s*\{",
        variables,
    ), "dark tokens must key both html.dark and html.dark-mode in one rule"
    # Bare html.dark without -mode still present as a class key, not only a comment
    assert re.search(r"html\.dark(?!-mode)", variables)
    assert "html.dark-mode" in variables


def test_header_toggle_keeps_html_dark_and_dark_mode_in_sync():
    """Header boot and toggle keep html.dark and html.dark-mode together."""
    text = HEADER.read_text(encoding="utf-8")
    assert "$darkmode" in text
    assert re.search(r'classList\.contains\(\s*["\']dark-mode["\']\s*\)', text)
    assert re.search(
        r'classList\.(?:add|toggle)\(\s*["\']dark-mode["\']',
        text,
    ), "boot or toggle must apply html.dark-mode"
    assert re.search(
        r'classList\.(?:add|toggle)\(\s*["\']dark["\']',
        text,
    ), "boot or toggle must apply html.dark"
    # Toggle click sets both class names from the same nextDark flag
    toggle_idx = text.find('getElementById("darkModeToggle")')
    assert toggle_idx != -1
    toggle_block = text[toggle_idx:]
    assert re.search(
        r'classList\.toggle\(\s*["\']dark["\']\s*,\s*nextDark\s*\)',
        toggle_block,
    )
    assert re.search(
        r'classList\.toggle\(\s*["\']dark-mode["\']\s*,\s*nextDark\s*\)',
        toggle_block,
    )


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
        "#MSearchSelect",
        "div.mermaid",
        ".project_brief",
    ):
        assert sel in blob, f"missing SCSS coverage for {sel}"
    # Doxygen 1.13-1.18 tabs/navtree/search sheets; doxygen.css is not linked
    for token in (
        "--nav-text-active-color",
        "--search-magnification-select-image",
        "--search-background-color",
        "--font-family-nav",
        "--nav-border-color",
        "--nav-menu-active-bg",
        "--sync-icon-color",
        "--scrollbar-thumb-color",
        "--search-box-border-color",
    ):
        assert token in blob, f"missing Doxygen 1.18 token {token}"
        assert blob.count(token) >= 3, (
            f"{token} must be defined in light and both dark token blocks"
        )


def test_scss_attrib_strip_has_named_full_width_grid_area():
    """footer.html emits .doxyyoda-attrib as a sibling of .footer; it needs its own row."""
    scss_root = ROOT / "src" / "styles" / "scss"
    blob = "\n".join(p.read_text(encoding="utf-8") for p in scss_root.rglob("*.scss"))
    assert "grid-area: attrib" in blob
    assert '"attrib attrib attrib"' in blob
    assert re.search(r'^\s*"attrib"\s*;?\s*$', blob, re.M)


def test_scss_names_sidebar_grid_area_for_treeview():
    """GENERATE_TREEVIEW #side-nav must land on an explicit sidebar track."""
    layout = (ROOT / "src" / "styles" / "scss" / "_layout.scss").read_text(encoding="utf-8")
    variables = (ROOT / "src" / "styles" / "scss" / "_variables.scss").read_text(
        encoding="utf-8"
    )
    areas = re.findall(
        r"grid-template-areas\s*:\s*([^;]+);",
        layout,
        flags=re.DOTALL,
    )
    assert areas, "grid-template-areas missing in _layout.scss"
    assert any("sidebar" in block for block in areas), (
        "desktop grid-template-areas must name a sidebar track"
    )
    assert re.search(
        r"#side-nav\s*\{[^}]*grid-area\s*:\s*sidebar\s*;",
        layout,
        flags=re.DOTALL,
    ), "#side-nav { grid-area: sidebar } must remain"
    # Left 1fr gutter collapses at <=75rem; treeview needs a length column.
    assert re.search(
        r"--sidebar-width\s*:\s*\d+(?:\.\d+)?rem\s*;",
        variables,
    ), "--sidebar-width must be a rem length, not 1fr"
    assert ":has(> #side-nav)" in layout or ":has(>#side-nav)" in layout
    assert "var(--sidebar-width)" in layout


def test_header_treeview_sidenav_is_grid_child():
    """FULL_SIDEBAR must open #side-nav inside .grid-contents, not before it."""
    text = HEADER.read_text(encoding="utf-8")
    grid = text.find('class="grid-contents"')
    side = text.find('id="side-nav"')
    assert grid != -1 and side != -1
    assert grid < side, "open #side-nav inside .grid-contents so grid-area: sidebar applies"


def test_demo_enables_generate_treeview():
    dox = (ROOT / "demo" / "Doxyfile").read_text(encoding="utf-8")
    assert re.search(r"^GENERATE_TREEVIEW\s*=\s*YES\s*$", dox, re.M)
    assert re.search(r"^HTML_DYNAMIC_SECTIONS\s*=\s*YES\s*$", dox, re.M)


def test_logo_png_is_the_shipped_mark():
    """Demo PROJECT_LOGO is the 2021 turtle raster, not a generated substitute."""
    png = ROOT / "src" / "images" / "doxyYoda_logo.png"
    dox = (ROOT / "demo" / "Doxyfile").read_text(encoding="utf-8")
    assert png.is_file()
    assert png.read_bytes()[:8] == b"\x89PNG\r\n\x1a\n"
    assert re.search(
        r"^PROJECT_LOGO\s*=\s*src/images/doxyYoda_logo\.png\s*$",
        dox,
        re.M,
    )


def test_header_uses_antics_not_umami():
    header = HEADER.read_text(encoding="utf-8")
    footer = (ROOT / "src" / "html" / "footer.html").read_text(encoding="utf-8")
    pages = (ROOT / ".github" / "workflows" / "pages.yml").read_text(encoding="utf-8")
    assert 'src="https://antics-api.turtletech.us/antics.js"' in header
    assert "antics.event" in header
    assert "umami" not in header.lower()
    assert "analytics.turtletech.us" not in header
    assert "antics.turtletech.us" in footer
    assert "umami.is" not in footer
    assert "umami" not in pages.lower()
    assert "antics" not in pages.lower()


def test_tablet_grid_overrides_treeview_has_selector():
    """Phone/tablet must collapse the sidebar track; :has is more specific."""
    nav = (ROOT / "src" / "styles" / "scss" / "_doxynav.scss").read_text(
        encoding="utf-8"
    )
    assert ".grid-contents:has(> #side-nav)" in nav
    assert re.search(
        r"@media\s+#\{\$media-size-tablet\}.*?\.grid-contents:has\(>\s*#side-nav\).*?grid-template-columns:\s*1fr",
        nav,
        flags=re.DOTALL,
    )


def test_layout_unwraps_doxygen_container():
    """Doxygen 1.18 wraps #doc-content in #container; unwrap for the grid."""
    layout = (ROOT / "src" / "styles" / "scss" / "_layout.scss").read_text(
        encoding="utf-8"
    )
    assert re.search(
        r"#container\s*\{\s*display:\s*contents\s*!important\s*;",
        layout,
    )
    assert re.search(
        r"#page-nav,\s*#page-nav-resize-handle\s*\{[^}]*display\s*:\s*none\s*!important",
        layout,
        flags=re.DOTALL,
    )


def test_fonts_are_fog_over_fen():
    fonts = (ROOT / "src" / "styles" / "scss" / "_fonts.scss").read_text(encoding="utf-8")
    assert "Fraunces" in fonts
    assert "Source+Serif+4" in fonts
    assert "Source+Sans+3" in fonts
    vars_scss = (ROOT / "src" / "styles" / "scss" / "_variables.scss").read_text(encoding="utf-8")
    assert "Fraunces" in vars_scss
    assert "--accent-ember" in vars_scss


def test_pixi_doxygen_floor_is_1_17():
    pixi = (ROOT / "pixi.toml").read_text(encoding="utf-8")
    assert re.search(r'^doxygen\s*=\s*">=1\.17"', pixi, re.M), pixi


def test_ci_runs_theme_contract_suite():
    """Push/PR CI must invoke this file so a broken header fails the job."""
    ci = (ROOT / ".github" / "workflows" / "ci.yml").read_text(encoding="utf-8")
    assert re.search(r"(?m)^\s+push:", ci)
    assert re.search(r"(?m)^\s+pull_request:", ci)
    assert re.search(
        r"(?m)^\s+run:\s+python(?:3)?\s+tests/test_theme_contracts\.py\s*$",
        ci,
    ), "ci.yml must run python tests/test_theme_contracts.py"


def test_pixi_has_test_task():
    """pixi run test must execute the same contract suite as CI."""
    pixi = (ROOT / "pixi.toml").read_text(encoding="utf-8")
    assert re.search(
        r'(?m)^test\s*=\s*\{[^}\n]*cmd\s*=\s*"python(?:3)? tests/test_theme_contracts\.py"',
        pixi,
    ), "pixi.toml must define a test task that runs the contract suite"


def test_release_fails_closed_without_version_matched_tarball():
    """A tag that does not match version.txt must fail; unmatched files too.

    pixi run release names the archive from version.txt. The upload path
    uses GITHUB_REF_NAME#v. Without an equality check and
    fail_on_unmatched_files, a mismatch still publishes a green release
    with no asset.
    """
    text = (ROOT / ".github" / "workflows" / "release.yml").read_text(encoding="utf-8")
    assert re.search(r"fail_on_unmatched_files:\s*true", text), (
        "softprops/action-gh-release must set fail_on_unmatched_files: true"
    )
    assert re.search(
        r"doxyYoda_\$\{\{\s*steps\.version\.outputs\.version\s*\}\}\.tar\.gz",
        text,
    ), "release must upload doxyYoda_<tag-version>.tar.gz"
    assert "version.txt" in text
    assert "GITHUB_REF_NAME" in text
    assert re.search(
        r"(?:cat\s+version\.txt|<\s*version\.txt)",
        text,
    ), "release job must read version.txt to compare against the tag"


def test_action_curl_fails_closed_on_http_errors():
    """Missing release assets must fail the action, not extract an error page."""
    action = (ROOT / "action.yml").read_text(encoding="utf-8")
    curl = re.search(r"curl\s+(\S+)\s+\"\$\{url\}\"", action)
    assert curl, "tarball download must curl ${url}"
    flags = curl.group(1)
    assert "f" in flags, f"curl must fail on HTTP errors (-f): {flags}"
    assert "S" in flags, f"curl must show errors (-S): {flags}"
    assert "L" in flags, f"curl must follow redirects (-L): {flags}"


def test_grid_cancels_doxygen_resize_offsets():
    """CSS Grid owns the sidebar column; navtree.js inline offsets must lose."""
    layout = (ROOT / "src" / "styles" / "scss" / "_layout.scss").read_text(
        encoding="utf-8"
    )
    nav = (ROOT / "src" / "styles" / "scss" / "_doxynav.scss").read_text(
        encoding="utf-8"
    )
    assert re.search(
        r"#doc-content\s*\{[^}]*margin-left\s*:\s*0\s*!important",
        layout,
        flags=re.DOTALL,
    )
    assert re.search(
        r"#container\s*\{[^}]*display\s*:\s*contents\s*!important",
        layout,
        flags=re.DOTALL,
    )
    assert re.search(
        r"#side-nav\s*\{[^}]*width\s*:\s*auto\s*!important",
        nav,
        flags=re.DOTALL,
    )
    assert re.search(
        r"#splitbar,\s*\.ui-resizable-handle,\s*#nav-sync\s*\{[^}]*display\s*:\s*none\s*!important",
        nav,
        flags=re.DOTALL,
    )


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
