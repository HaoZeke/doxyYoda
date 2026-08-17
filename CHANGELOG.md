# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Changed
- README Why/Demo/Dark Mode shots are the current fog-over-fen theme, not Solarized

## [0.2.2] - 2026-08-17

### Fixed
- Hide Doxygen `#page-nav` so `display: contents` on `#container` cannot open an empty footer row
- Stop clipping tree labels (stock `margin-left: -5px`) and fold the Generated-by credit into the attrib strip

## [0.2.1] - 2026-08-17

### Fixed
- Footer no longer emits an extra `</div>`: Doxygen 1.17+ already closed `#doc-content` and `#container`

## [0.2.0] - 2026-08-17

### Added
- Fog-over-fen visual identity: sage/moss/ember tokens, Fraunces + Source Serif 4 + Source Sans 3. Title bar uses the original lime-and-shell turtle.
- Doxygen 1.17 / 1.18 header tokens (`$mermaidjs`, `$darkmode`, `HTML_DYNAMIC_SECTIONS`, `HTML_CODE_FOLDING`, `doxygen-page-layout`)
- CSS Grid sidebar track for `GENERATE_TREEVIEW` (`:has(> #side-nav)`)
- Pure theme preference helpers in `src/js/theme_pref.js` with contract tests under `tests/`
- CI push/PR job and `pixi run test` run `python tests/test_theme_contracts.py`

### Changed
- Analytics is Antics (`antics-api.turtletech.us/antics.js` plus outbound-link helper); footer credits Antics
- Demo `PROJECT_LOGO` is the 2021 turtle PNG (`src/images/doxyYoda_logo.png`)
- Header/footer templates use HTML5 (`<!DOCTYPE html>`), drop XHTML transitional + IE meta
- Dark-mode boot respects `prefers-color-scheme` when no localStorage preference is set; toggle keeps `aria-pressed` in sync and keys `html.dark` plus `html.dark-mode`
- Logo link targets `$relpath^index.html`; title area marks up as a labelled `<nav>`
- Footer attribution uses themed `.doxyyoda-attrib` classes instead of inline styles
- SCSS coverage for modern Doxygen structures: `.textblock`, `.compoundTemplParams`, `div.dynheader` / `div.dyncontent`, `div.classindex`, `table.doxtable`, sticky-header `:target` scroll margin, `prefers-reduced-motion`
- pixi Doxygen floor is `>=1.17`
- Consumer docs and examples pin the current release from `version.txt` (action `@v…`, tarball paths)

### Fixed
- Drop Doxygen `clipboard.js` so each fragment has one themed `.copy-btn`; copy excludes the button label
- Unwrap Doxygen 1.18 `#container` (`display: contents`) so the leftover sage strip is gone
- Collapse the treeview sidebar track on tablet/phone (`:has(> #side-nav)` override)
- CSS Grid owns the treeview column; `navtree.js` / `resize.js` inline margin and height lose (`!important`)
- Tag release fails when the tag does not match `version.txt` or the tarball is missing; the setup action `curl`s with `-fS`

## [0.1.3] - 2026-02-16

### Added
- SVG project logo (`src/images/doxyYoda_logo.svg`)
- `PROJECT_LOGO` entry in demo Doxyfile

### Changed
- Title area uses 3-column CSS Grid for true center alignment
- Responsive layout: simplified single-column grid on small screens
- Navigation menu items evenly spaced
- Body base styles (margin reset, background/text color variables)
- Logo max-height reduced to 36 px for a tighter title bar
- GitHub Action default version updated to latest release

## [0.1.2] - 2026-02-16

### Fixed
- Logo image rendering with literal `$logosize` instead of proper height constraint
- Logo now sized via CSS (`max-height`) to fit within the title bar

## [0.1.0] - 2025-01-01

### Added
- CSS Grid layout for modern page structure
- Dark mode support with Solarized Dark palette
- Foldable/collapsible code blocks
- MathJax 3 integration
- Demo site with full-featured C++ showcase (`demo/`)
- SCSS build pipeline via dart-sass
- pixi-based task runner (`pixi run build`, `pixi run demo`, etc.)
- GitHub Actions CI and Pages deployment
- Release tarball task (`pixi run release`)
- Reusable GitHub Action (`uses: HaoZeke/doxyYoda@v0.1.0`) with optional cppreference tag download
- Version management via tbump (`pixi run bump`)
- CHANGELOG.md
- GitHub Actions release workflow (auto-creates GitHub Release on `v*` tags)

### Changed
- Complete SCSS rewrite: variables, colors, typography, layout, navigation
- Updated for Doxygen >= 1.12 compatibility
- Switched from jQuery inline-style hacks to proper CSS overrides
- CI and Pages workflows now use pixi for reproducible doxygen/sass versions

### Fixed
- Dark mode toggle not working for users with OS-level dark mode preference

### Removed
- jQuery-based header manipulation
- `mkRel.sh` (superseded by `pixi run release`)

## [0.0.2] - 2021-03-07

### Added
- CSS Grid page layout
- Dynamic code-snippet wrapping via JS
- Attribution and licensing in all source files

### Fixed
- White-space handling in code blocks
- Minor navigation bug

### Changed
- Dropped Fira Code, reorganized SCSS variables
- Updated release script and version to 0.0.2

## [0.0.1] - 2021-03-02

### Added
- Initial Doxygen theme with Solarized Light color scheme
- SCSS source: variables, typography, code highlighting, tooltips
- Custom header/footer HTML templates
- MathJax 3 support in header
- Basic project structure and README

[Unreleased]: https://github.com/HaoZeke/doxyYoda/compare/v0.2.2...HEAD
[0.2.2]: https://github.com/HaoZeke/doxyYoda/compare/v0.2.1...v0.2.2
[0.2.1]: https://github.com/HaoZeke/doxyYoda/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/HaoZeke/doxyYoda/compare/v0.1.3...v0.2.0
[0.1.3]: https://github.com/HaoZeke/doxyYoda/compare/v0.1.2...v0.1.3
[0.1.2]: https://github.com/HaoZeke/doxyYoda/compare/v0.1.0...v0.1.2
[0.1.0]: https://github.com/HaoZeke/doxyYoda/compare/0.0.2...v0.1.0
[0.0.2]: https://github.com/HaoZeke/doxyYoda/compare/v0.0.1...0.0.2
[0.0.1]: https://github.com/HaoZeke/doxyYoda/releases/tag/v0.0.1
