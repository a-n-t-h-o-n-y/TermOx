# Building and publishing the API reference

The reference is generated from `include/ox/` using **Doxygen 1.18.0** and the
vendored **Doxygen Awesome 2.5.0** sidebar theme. Generated HTML stays under
`build/` and is never committed.

## Build locally

Install Doxygen 1.18.0 from its [official release](https://github.com/doxygen/doxygen/releases/tag/Release_1_18_0),
then run from the repository root:

```sh
cmake -S docs/doxygen -B build/docs
cmake --build build/docs --target TermOx.docs
```

If Doxygen is not on PATH, pass `-DDOXYGEN_EXECUTABLE=/absolute/path/to/doxygen`
to the configure command. Open `build/docs/html/index.html` to read the reference.
For local browser testing, serve the output with
`python3 -m http.server 8000 --directory build/docs/html`.

This standalone CMake project needs no C++ compiler, Graphviz, library dependency
downloads, or JavaScript package manager. The regular library build can also add
`TermOx.docs` by configuring with `-DTERMOX_BUILD_DOCUMENTATION=ON`.

## Publishing

[The reference workflow](../../.github/workflows/reference.yml) builds on pushes
to `library-refactor` and on pull requests. Pull requests upload an HTML artifact
for inspection; pushes to `library-refactor` also publish through GitHub's official
Pages artifact and deployment actions. A manual run is available once the workflow
exists on the default branch.

The reference is hosted at <https://a-n-t-h-o-n-y.github.io/TermOx/>. Pages uses
**GitHub Actions** as its source. No generated-file branch or publishing token is
needed. Only the deployment job has Pages write permissions.

While the refactor is underway, `library-refactor` is the publishing branch. When
it is merged, change the workflow's push branch and both deployment conditions to
`main`, and update the branch links in `mainpage.md`.

## Inline documentation

The initial configuration extracts public declarations even when their comments
are incomplete, and excludes private members and `ox::detail`. It treats malformed
comments and parameter mismatches as build failures; diagnostics are written to
`build/docs/warnings.log`. Missing descriptions are not yet failures. Once public
API comments are complete, turn off `EXTRACT_ALL` and enable undocumented-member
and parameter checks.

Keep contracts beside their declarations: ownership, signal emission, exceptions,
thread restrictions, and layout/repaint effects. Guides and app walkthroughs remain
Markdown in the repository. The handwritten reference stays until those contracts
have been transferred and verified.

## Appearance and versions

`termox.css` supplies TermOx's copper accents and green surfaces. The upstream
sidebar theme and dark-mode toggle are unmodified; their version, source commit,
and license are recorded in `vendor/doxygen-awesome/`. CMake generates Doxygen's
own header template and adds the theme's documented dark-mode initialization.

CI downloads the pinned official Linux binary and verifies its SHA-256 checksum.
When updating Doxygen, update the exact CMake requirement, CI download and checksum,
and this guide together. Check theme compatibility and inspect both light and dark
pages, search results, and a class with templates and overloaded methods.
