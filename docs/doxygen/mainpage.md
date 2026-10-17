# TermOx API reference

TermOx is a C++20 library for terminal user interfaces. Build applications from
widgets, compose them with layouts, and connect their signals.

Browse **Classes**, **Namespaces**, and **Files** in the sidebar, or search for an
interface or method by name. Each class page lists its constructors, methods,
options, and signals; header links show the declarations in context.

| Start with | Interfaces |
| --- | --- |
| Application and lifetime | ox::Application, ox::Widget, ox::Connection, ox::Timer |
| Layout | ox::Row, ox::Column, ox::Padding, ox::Stack, ox::ScrollView |
| Controls and data | ox::Button, ox::ListView, ox::DataTable, ox::TreeView, ox::Form |
| Text | ox::Label, ox::LineEdit, ox::TextBox |
| Appearance | ox::Theme, ox::Palette, ox::Animation, ox::Command |
| Drawing | ox::Canvas, ox::Frame, ox::PixelGrid |

## Guides and apps

- [Getting started](https://github.com/a-n-t-h-o-n-y/TermOx/blob/library-refactor/docs/getting_started.md)
- [Topic guides](https://github.com/a-n-t-h-o-n-y/TermOx/blob/library-refactor/docs/README.md)
- [Apps and development tools](https://github.com/a-n-t-h-o-n-y/TermOx/blob/library-refactor/examples/README.md)
- [Source repository](https://github.com/a-n-t-h-o-n-y/TermOx)

This reference is generated from inline comments in the public headers. Methods,
options, signals, enum values, and inherited interfaces are documented beside their
declarations. Class pages describe ownership, notifications, errors, and coordinate
units where they affect use. Escape input and drawing types are summarized on the
[events](events_8hpp.html) and [glyph](glyph_8hpp.html) header pages, with links to
the upstream declarations.
