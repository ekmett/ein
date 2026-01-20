# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project adheres to
Semantic Versioning.

## [Unreleased]

### Fixed
- Make AVX512 detection use a compiler check instead of `lscpu`.
- Handle `__cxa_demangle` failures by falling back to `typeid` names.
- Make `profiling.hpp` self-contained and fix inverted JSON emission checks.
- Correct `static_string` usage of `string_view::data()` and boolean conversion.
