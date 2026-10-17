# API reference

Look up an interface below. Each entry links to its declaration and usage details.
For a first program, see [Getting Started](getting_started.md); for explanations
of related interfaces, use the [guides index](README.md).

## Topics

| Reference page | Contents |
| --- | --- |
| [Application and widget core](reference/application.md) | Application ownership, widget contracts, callbacks, focus, and scheduling. |
| [Layouts and decorations](reference/layout.md) | Size policies, container storage, borders, and dividers. |
| [Controls and data views](reference/controls.md) | Buttons, choices, tables, scrolling, and composite widgets. |
| [Labels and text editing](reference/text.md) | Labels, numeric labels, text widgets, and the editing core. |
| [Drawing and rendering](reference/drawing.md) | Shapes, drawing functions, pixel images, frames, canvases, and rendering. |
| [Geometry, text, and style types](reference/types.md) | Coordinates, alignment, colors, styles, traits, and glyphs. |
| [Terminal and input](reference/terminal.md) | Terminal sessions, key and mouse events, and the event variant. |

## Interfaces

### Application and widget core

- <a id="-oxapplication"></a>[ox::Application](reference/application.md#oxapplication)
- <a id="-oxwidget"></a>[ox::Widget](reference/application.md#oxwidget)
- <a id="-oxconnection"></a>[ox::Connection](reference/application.md#oxconnection)
- <a id="-oxlifetimeview"></a>[ox::LifetimeView](reference/application.md#oxlifetimeview)
- <a id="-oxfocuspolicy"></a>[ox::FocusPolicy](reference/application.md#oxfocuspolicy)
- <a id="-oxfocus"></a>[ox::Focus](reference/application.md#oxfocus)
- <a id="-routing-and-focus"></a>[Routing and focus](reference/application.md#routing-and-focus)
- <a id="-oxtimer"></a>[ox::Timer](reference/application.md#oxtimer)
- <a id="-oxeventloop"></a>[ox::EventLoop](reference/application.md#oxeventloop)

### Layouts and decorations

- <a id="-oxsizepolicy"></a>[ox::SizePolicy](reference/layout.md#oxsizepolicy)
- <a id="layout-concepts"></a>[Layout Concepts](reference/layout.md#layout-concepts)
- <a id="-oxrow"></a>[ox::Row](reference/layout.md#oxrow)
- <a id="-oxcolumn"></a>[ox::Column](reference/layout.md#oxcolumn)
- <a id="-oxsuspended"></a>[ox::Suspended](reference/layout.md#oxsuspended)
- <a id="-oxpadding-oxstack-oxscrollview"></a>[ox::Padding, ox::Stack, ox::ScrollView](reference/layout.md#oxpadding-oxstack-oxscrollview)
- <a id="-layout-calculation"></a>[Layout calculation](reference/layout.md#layout-calculation)
- <a id="-oxborder"></a>[ox::Border](reference/layout.md#oxborder)
- <a id="-oxbordered"></a>[ox::Bordered](reference/layout.md#oxbordered)
- <a id="-oxlines"></a>[ox::Lines](reference/layout.md#oxlines)
- <a id="-oxdivider"></a>[ox::Divider](reference/layout.md#oxdivider)

### Controls and data views

- <a id="-oxbutton"></a>[ox::Button](reference/controls.md#oxbutton)
- <a id="-oxcheckbox"></a>[ox::CheckBox](reference/controls.md#oxcheckbox)
- <a id="-oxradiogroup"></a>[ox::RadioGroup](reference/controls.md#oxradiogroup)
- <a id="-oxdatatable"></a>[ox::DataTable](reference/controls.md#oxdatatable)
- <a id="-oxscrollbar"></a>[ox::ScrollBar](reference/controls.md#oxscrollbar)
- <a id="selection-and-application-widgets"></a>[Selection and application widgets](reference/controls.md#selection-and-application-widgets)

### Labels and text editing

- <a id="-oxlabel"></a>[ox::Label](reference/text.md#oxlabel)
- <a id="-oxbasicnumberlabel-oxintegerlabel-oxfloatlabel"></a>[ox::BasicNumberLabel ox::IntegerLabel ox::FloatLabel](reference/text.md#oxbasicnumberlabel-oxintegerlabel-oxfloatlabel)
- <a id="-oxlineedit"></a>[ox::LineEdit](reference/text.md#oxlineedit)
- <a id="-oxtextbox"></a>[ox::TextBox](reference/text.md#oxtextbox)
- <a id="-text-editing"></a>[Text editing](reference/text.md#text-editing)

### Drawing and rendering

- <a id="-oxshapehline"></a>[ox::shape::HLine](reference/drawing.md#oxshapehline)
- <a id="-oxshapevline"></a>[ox::shape::VLine](reference/drawing.md#oxshapevline)
- <a id="-oxshapebox"></a>[ox::shape::Box](reference/drawing.md#oxshapebox)
- <a id="-oxput"></a>[ox::put(...)](reference/drawing.md#oxput)
- <a id="-oxfill"></a>[ox::fill(...)](reference/drawing.md#oxfill)
- <a id="-oxclear"></a>[ox::clear(...)](reference/drawing.md#oxclear)
- <a id="-oxpixelgrid"></a>[ox::PixelGrid](reference/drawing.md#oxpixelgrid)
- <a id="-oxframe-and-oxcanvas"></a>[ox::Frame and ox::Canvas](reference/drawing.md#oxframe-and-oxcanvas)
- <a id="-oxrenderer"></a>[ox::Renderer](reference/drawing.md#oxrenderer)

### Geometry, text, and style types

- <a id="-oxpoint"></a>[ox::Point](reference/types.md#oxpoint)
- <a id="-oxarea"></a>[ox::Area](reference/types.md#oxarea)
- <a id="-oxalign"></a>[ox::Align](reference/types.md#oxalign)
- <a id="oxtrait-and-oxtraits"></a>[ox::Trait and ox::Traits](reference/types.md#oxtrait-and-oxtraits)
- <a id="-oxcolor-and-oxstyle"></a>[ox::Color and ox::Style](reference/types.md#oxcolor-and-oxstyle)
- <a id="-oxglyph-and-oxglyphstring"></a>[ox::Glyph and ox::GlyphString](reference/types.md#oxglyph-and-oxglyphstring)

### Terminal and input

- <a id="-oxterminal"></a>[ox::Terminal](reference/terminal.md#oxterminal)
- <a id="-mouse-events"></a>[Mouse Events](reference/terminal.md#mouse-events)
- <a id="-oxkeyevent"></a>[ox::KeyEvent](reference/terminal.md#oxkeyevent)
- <a id="-oxevent"></a>[ox::Event](reference/terminal.md#oxevent)

## Related guides

| Interfaces | Guide |
| --- | --- |
| ListView, Select, ComboBox, Tabs, Popup, Dialog, Menu, MenuBar | [Selection widgets](selection-and-application.md) |
| NumericEdit, IntegerEdit, Slider, ProgressBar, Spinner, Switch | [Values and status](value-and-composite-widgets.md#values-and-status) |
| TreeView, LogView, SplitPane, Form, FilePicker, CommandPalette | [Composite widgets](value-and-composite-widgets.md) |
| Command | [Shared commands](commands.md) |
| Theme, MotionPolicy, Tween, Animation, KeyHelp | [Theme and motion](theme-and-motion.md) |
| Gutters, density, indicators, and decoration options | [Widget presentation](presentation.md) |

Coverage varies by interface. The linked headers contain the complete declarations
and defaults.
