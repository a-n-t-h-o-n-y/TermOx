# Building the API reference

The reference is generated from `include/ox/` with Doxygen 1.18.0 and the vendored
Doxygen Awesome 2.5.0 theme. Generated HTML stays under `build/`.

## Build locally

Install [Doxygen 1.18.0](https://github.com/doxygen/doxygen/releases/tag/Release_1_18_0),
then run from the repository root:

```sh
cmake -S docs/doxygen -B build/docs
cmake --build build/docs --target TermOx.docs
python3 tests/check_reference.py build/docs/html
```

Open `build/docs/html/index.html` to read the reference. If Doxygen is not on PATH,
pass `-DDOXYGEN_EXECUTABLE=/absolute/path/to/doxygen` when configuring.
The regular library build can also enable this target with `-DTERMOX_BUILD_DOCUMENTATION=ON`.

## Publishing

[The reference workflow](../../.github/workflows/reference.yml) checks pull requests
and uploads their HTML as an artifact. Pushes to the default branch publish the
[API reference](https://a-n-t-h-o-n-y.github.io/TermOx/) through GitHub Pages.

## Inline documentation

Describe public declarations and enum values. Document behavior that callers need
to know, such as ownership, thread restrictions, exceptions, and callback effects.
Avoid restating the signature. Guides and app walkthroughs stay in Markdown.

Missing descriptions and malformed comments fail the build; see
`build/docs/warnings.log`. Inherited members appear on each class page. Overrides
can inherit comments when their behavior is unchanged.

## Updating Doxygen or the theme

Update the CMake version requirement, CI download and SHA-256 checksum, and this
guide together. Check light/dark pages, search, and classes with templates and overloads.

`termox.css` supplies the project colors. The vendored theme's version, source commit,
and license are in `vendor/doxygen-awesome/`. CMake adds its dark-mode initialization
to Doxygen's generated header. Static menus avoid incorrect alphabetical jump links
in Doxygen 1.18.0's dynamic menus.
