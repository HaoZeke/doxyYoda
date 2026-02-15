# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

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

[Unreleased]: https://github.com/HaoZeke/doxyYoda/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/HaoZeke/doxyYoda/compare/0.0.2...v0.1.0
[0.0.2]: https://github.com/HaoZeke/doxyYoda/compare/v0.0.1...0.0.2
[0.0.1]: https://github.com/HaoZeke/doxyYoda/releases/tag/v0.0.1
