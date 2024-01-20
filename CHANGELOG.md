# Changelog

## [0.3.0] - 2024-01-20

### Added

 - Add ability to update packages.
 - Add package uninstallation.
 - Add color output.
 - Add convienient install script.

### Fixed

 - Fix segmentation fault when `packages.list` file exists but we don't have permission to write to it.
 - Fix segmentation fault when the package doesn't have a valid signature.

### Changed

 - Refactor `main.c` to use early returns.
 - Move package signature, source archive and source tree to `/tmp/npkg/src`.
 - Disable debug messages by default. They can be enabled at compile time by passing the `--enable-debug` argument to `configure`.

## [0.2.0] - 2024-01-14

### Added

 - Add ability to search for packages in repository.
 - **Breaking:** add command-line option parsing.
 - **Breaking:** add build-system-agnostic installation.

## [0.1.1] - 2023-12-31

### Fixed

 - Fix segmentation fault when no configuration file is present.
 - Fix invalid pointer free() when no configuration file is present.

## [0.1.0] - 2023-12-31

_Initial release._


[0.1.0]: https://github.com/yanderemine54/npkg/releases/tag/v0.1.0
[0.1.1]: https://github.com/yanderemine54/npkg/releases/tag/v0.1.1
[0.2.0]: https://github.com/yanderemine54/npkg/releases/tag/v0.2.0
[0.3.0]: https://github.com/yanderemine54/npkg/releases/tag/v0.3.0
