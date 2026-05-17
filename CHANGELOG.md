# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project follows Semantic Versioning with the `-alpha` suffix while pre-release.

## [v1.0.3-alpha] - 2026-05-17

### Fixed
- `xre_use_string(ctx, NULL)` and `xre_state_set_string(state, NULL)` no longer crash; NULL is normalized to an empty string.
- String state length tracking is now consistent with the current value.
- `xre_use_effect` cleanup now guards null cleanup callbacks and no longer crashes when a cleanup object has `fn == NULL`.

### Added
- Comprehensive unit test scaffolding under `tests/src/xre-core/`.
- Crash-isolated test utilities in `tests/src/shared/` using subprocess execution.
- Regression tests for historical crash cases (`NULL` string input and `NULL` effect cleanup function).

## [v1.0.2-alpha] - 2024-05-17
### Notes
- Previous release tag.

## [v1.0.1-alpha] - 2024-05-17
### Notes
- Previous release tag.

## [v1.0.0-alpha] - 2024-05-17
### Notes
- Initial alpha release tag.
