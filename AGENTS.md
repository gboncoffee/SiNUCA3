# SiNUCA3 — Agent / Contributor Guidelines

This document collects coding and project conventions discovered across the
repository headers and sources. It is intended to help contributors and
automated agents produce consistent, maintainable code for SiNUCA3.

---

## High-level principles

- The project is cross-platform C++ (Linux and Windows portability
  considerations are present).
- All files carry the GNU GPL license header. New files should include the same
  license block.
- Public interfaces use Doxygen-style comments (`@file`, `@brief`, `@details`)
  and short descriptive comments for functions and classes.
- Keep functions small and focused. Prefer clarity and explicit behavior over
  clever constructs.

---

## File headers & include guards

- Source and header files start with the GPL copyright/license comment block
  (year & group).
- Use Doxygen headers at top of each header/source file:
     - `@file` to identify the file
     - `@brief` for short description
     - `@details` for longer explanations when needed
- Include guards follow the pattern:
     - Uppercase, `SINUCA3_` prefix, then the path-like name. Examples:
          - `SINUCA3_UTILS_LOGGER_HPP_`
          - `SINUCA3_CONFIG_HPP_`
     - Use a trailing underscore for `.hpp` guards (`_HPP_`).

---

## Logging

There are logging utilities and macros used across the codebase.

- A contextual logger singleton exists providing richer output and metadata
  (file, line, function) in `utils/logger.hpp` / `utils/logger.cpp`.
     - Use `logger::Logger::instance()->log(...)` via the provided convenience
       macros:
          - `SINUCA3_ERROR_PRINTF(...)`
          - `SINUCA3_WARNING_PRINTF(...)`
          - `SINUCA3_LOG_PRINTF(...)`
          - `SINUCA3_DEBUG_PRINTF(...)` (compiles to nothing when `NDEBUG` is
            set)
     - The contextual logger prints colored, annotated messages when the output
       is a TTY.
     - The logger exposes runtime filtering (minimum log level) via:
          - `logger::Logger::instance()->set_level(logger::LEVEL_WARNING);`
          - Query current level with `logger::Logger::instance()->level()`.
     - Levels are ordered (by numeric value): `LEVEL_ERROR`, `LEVEL_WARNING`,
       `LEVEL_INFO`, `LEVEL_DEBUG`.
          - Lower numeric value = more severe. Messages with a numeric level
            greater than the configured minimum are suppressed.

Guidelines:

- Prefer the contextual logger when you want file/line/function metadata.
- Use `SINUCA3_DEBUG_PRINTF` for verbose internal debugging that should be
  eliminated in release builds (`NDEBUG`).
- When adding new logging calls, use the macros for consistency. Do not sprinkle
  `printf` directly.

---

## Coding style & API design

- Public APIs are well-documented with Doxygen comments.
- Use `inline` for trivial one-line functions in headers when appropriate
  (constructors, small accessors).
- Use `assert(...)` for unreachable branches or internal invariants.
- Keep state encapsulated: many classes expose only the minimum public API
  required by the engine.

Memory and performance:

- Messages passed between components are passed by value — avoid very large
  types as the `MessageType`.
- Templates used where type safety is desired, but heavy logic is centralized in
  non-template implementations for better compile and runtime performance.

---

## Components and engine model

- Components inherit from `Component<MessageType>` (wrapper around `Linkable`).
     - If a component does not exchange messages, use `int` as a placeholder
       `MessageType`.
     - Avoid large `MessageType` types because messages are copied/passed by
       value.
- Typical component lifecycle and API:
     - `virtual void Clock()` — called by the engine each cycle.
     - `virtual int Configure(Config config)` — read configuration parameters.
     - `virtual void PrintStatistics()` — print stats at simulation end.
- Messaging API exposed by `Component`:
     - `Connect(bufferSize)` — create a connection and return connection id.
     - `IsComponentAvailable(connectionID)` — check if a peer can receive a
       request.
     - `SendRequest(connectionID, MessageType* message)` /
       `SendResponseToConnection(...)`
     - `ReceiveRequestFromConnection(...)` / `ReceiveResponse(...)`
- `Linkable` provides the underlying connection semantics and buffer management.
  Only use the `Linkable` lower-level API when necessary (essentially never).

Guidelines:

- Prefer using `Component<T>` public helpers instead of calling `Linkable`
  internals.
- Implement `Configure` to read all parameters using the `Config` helpers and
  return non-zero on error.
- `PrintStatistics` should produce useful, human-readable output.

---

## Configuration and YAML

- The project uses a YAML-based configuration (`yaml_parser.hpp` / `yaml`
  namespace).
- `Config` is the public configuration API:
     - Use `Config::Bool`, `Config::Integer`, `Config::Floating`,
       `Config::String` to read typed parameters.
     - Use `Config::ComponentReference<T>` to obtain references to other
       components.
     - `Config::Error(parameter, reason)` reports configuration
       parsing/validation errors.
     - `Config::RawYaml()` and `Config::Fork(...)` exist but are marked _only
       use if you know what you're doing_.
- There are helper factory functions used by the engine to instantiate
  components by class name:
     - `CreateDefaultComponentByClass(...)`,
       `CreateCustomComponentByClass(...)`, `CreateComponentByClass(...)`.
- For tests, use `CreateFakeConfig(yaml::Parser*, content, aliases)` to
  instantiate configurations.

Guidelines:

- Keep config parsing defensive: use `required` flags appropriately and return
  informative errors.

---

## Macros & build-time behavior

- `NDEBUG` controls debug macro expansion:
     - When `NDEBUG` is defined, `SINUCA3_DEBUG_PRINTF(...)` compiles to
       nothing.
     - Some code paths in `main.cpp` and elsewhere are conditional on `NDEBUG`.
- Command-line switches in `main.cpp` are conditionally compiled depending on
  `NDEBUG` (see `SINUCA3_SWITCHES` definition).
- Avoid heavy runtime side-effects in macros; macros should be thin wrappers
  around consistent calls.

---

## Tests, debugging, and development

- There is a `tests.hpp`/`tests.cpp` area compiled into debug builds (guarded by
  `#ifndef NDEBUG`) and accessible via `main` when not `NDEBUG`.
- Use `SINUCA3_DEBUG_PRINTF` for internal debug messages that you do not want in
  release.
- Prefer deterministic small-unit tests that use `CreateFakeConfig(...)` and
  component stubs.

---

## Style & formatting

- Use 4 spaces consistently (follow existing project spacing; keep style
  consistent with surrounding file). `private`, `public` and `protected`
  keywords are only indented with 2 spaces.
- Keep line lengths reasonable to keep license header and comment blocks
  readable.
- Prefer clear, descriptive names for functions and parameters.
- Use `const` for read-only parameters where appropriate (e.g., `const char*`,
  `const Level`).
- `clang-format` will automatically format the code, but does not catch all
  guidelines. Always format everything before commiting.
- Always use `this`.

Naming conventions:

- Classes, structs: `PascalCase` (e.g. `Config`, `Component`).
- Methods: `PascalCase` (e.g. `Configure`, `PrintStatistics`, `SendRequest`).
- Enums: `PascalCase` for values (e.g. `LevelError`).
- Macros: `SINUCA3_*` naming and upper-case identifiers.
- Variables: `camelCase` (e.g. `engine`, `logLevel`).
- Members: exactly the same as variables, without any suffix or prefix.

---

## Error handling

- Return `int` status codes for many Configure/Component methods: `0` for
  success, non-zero for failure.
- Use `SINUCA3_ERROR_PRINTF` for reporting fatal/important errors during
  configuration, initialization, and runtime faults.
- Use `assert` for detecting programmer errors (invariants that should never
  fail in production).

---

## Contribution tips

- Add or update Doxygen comments when changing public APIs.
- Keep license header at top of all new files and copy existing header format.
- Run the project compile in both debug and release modes to catch `NDEBUG`
  differences.
- If adding new global macros, prefix with `SINUCA3_` to avoid clashes.

---

## Files & locations of interest

- `src/utils/logger.hpp`, `src/utils/logger.cpp` — contextual logger singleton
  and macros.
- `src/engine/component.hpp` — `Component<T>` base wrapper.
- `src/engine/linkable.hpp` / `src/engine/linkable.cpp` — underlying messaging &
  buffers.
- `src/config/config.hpp`, `src/config/config.cpp` — configuration API and
  helpers.
- `src/main.cpp` — program entry, command-line handling, license/usage printing.
- `src/std_components` — directory with the components distributed with the
  simulator.
