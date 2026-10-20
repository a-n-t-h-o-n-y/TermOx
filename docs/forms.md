# Validate and submit a form

Use `Form` to line up labels, editors, help, and errors. Field widgets remain ordinary
controls; the form commits their drafts before running cross-field validation.

## Compose the fields

This complete program requires a name and an integral voice count. Press Ctrl+Enter
inside the form or activate Save. Invalid fields show an error; successful submission
updates the status line.

```cpp
#include <ox/ox.hpp>

int main()
{
    auto form = ox::Form{
        std::tuple{ox::LineEdit{}, ox::IntegerEdit{{.value = 8}}},
        std::vector<ox::FormField>{
            {.label = "Name", .help = "Required",
             .validate = [](ox::Widget const& field) -> std::optional<std::string> {
                 auto const& name = static_cast<ox::LineEdit const&>(field);
                 if (name.text().empty()) return "Enter a name";
                 return std::nullopt;
             }},
            {.label = "Voices", .help = "Use a whole number"},
        },
    };
    auto save = ox::Button{{.label = {.text = "Save"}}};
    auto status = ox::Label{"Edit the fields, then save."};
    auto root = ox::Column{std::tie(form, save, status)};
    ox::Connection{.signal = save.on_activate,
                   .slot = [](auto& form) { form.submit(); }}(form);
    ox::Connection{.signal = form.on_submit,
                   .slot = [](ox::Label& status) { status.set_text("Saved"); }}(status);
    return ox::Application{root}.run();
}
```

Validators receive the actual field widget. The cast above matches the concrete
widget in that field's position. Read typed numeric values with `value()` after
commit; use the editor's text only when inspecting a draft.

## Submission order and errors

Validators are functions taking the actual field widget and returning an error message
or nullopt. `submit()` validates every field, stores `errors()`, focuses the first invalid
field, and emits `on_invalid(errors const&)` or `on_submit()`. Ctrl-Enter submits from
within the form. Before validation, Form calls `commit()` on fields whose concrete types expose a
boolean commit operation, including every BasicNumericEdit specialization. Invalid drafts
produce an `"Invalid value"` field error and prevent submission. Validation then sees the
committed typed value. Each `FormField` also accepts an optional `commit(Widget&) -> bool`
callback, overriding automatic commit; use it for custom composites, fields stored through
base-class references, or an explicit no-op when preserving drafts is desired. Submission
is not atomic across fields: successful commits remain if another field is invalid.
`set_fields` validates labels/count and resets errors. Validate functions must not alter
field configuration while running.

A cross-field validator may capture tracked views of other fields and inspect their
committed values. It should return a message without editing widgets or submitting
the form again. Save durable application data from `on_submit`, after validation.

## Layout and field metadata

Form retains a tuple or sized forward range of widget values or references, plus a
same-length vector of `FormField{label, help, validate, commit}` metadata. Fields remain directly accessible in `children`.
Labels occupy a shared column; `label_width` optionally fixes it. Help/errors appear
beneath each field; `gap` separates rows. Field width policies limit controls within
the shared field column and also constrain measurement of wrapped content. Put large
forms inside ScrollView.

For numeric drafts and formatting, see [Numeric input](values.md#commit-numeric-drafts).
For a larger form and a modal dialog around it, see the [Showcase](../examples/showcase.md).
