# Checkbox Widget

A Checkbox is a Widget that can be left-clicked to change its state between
being checked and unchecked, emitting a cooresponding Signal when the state
changes. A Checkbox may also be locked, so that it cannot be manipulated until
it is unlocked.

There are a variety of Checkbox designs implemented by the library, as well as a
set of pre-defined Labeled Checkboxes for common combinations.

## Checkbox Interface

```cpp
/// Checkbox Widget that is either checked or not checked.
/** Uses mouse left button click to toggle between states. */
template <template <typename> typename Layout_t>
class Checkbox : public Label<Layout_t> {
   public:
    enum class State : bool { Unchecked, Checked };

    struct Display {
        Glyph_string checked;
        Glyph_string unchecked;
    };

    struct Parameters {
        State initial_state;
        Display display;
        bool locked;
    };

   public:
    /// Emitted when box becomes checked.
    sl::Signal<void()> checked;

    /// Emitted when box becomes unchecked.
    sl::Signal<void()> unchecked;

    /// Emitted every time the box changes state.
    sl::Signal<void()> toggled;

   public:
    explicit Checkbox(State initial_state, Display display, bool locked);
    explicit Checkbox(Parameters p);

   public:
    /// Set the state to be checked.
    void check();

    /// Set the state to be unchecked.
    void uncheck();

    /// Change state to be unchecked if currently checked, checked otherwise.
    void toggle();

    /// Return the current state of the Checkbox as Checkbox::State enum value.
    auto get_state() const -> State;

    /// Lock the Checkbox, it can not be toggled when locked.
    void lock();

    /// Unlock the Checkbox, allowing it to be toggled.
    void unlock();

    /// Return true if the Checkbox is locked.
    auto is_locked() const -> bool;

    /// Set the look of each Checkbox State.
    void set_display(Display d);

    /// Return the look of each Checkbox State.
    auto get_display() -> Display;
};
```

### Checkbox Library Types

| Widget Type | Unchecked Display   | Checked Display     |
|:------------|:-------------------:|:-------------------:|
|`Checkbox1`  | `☐`                 | `☒`                 |
|`Checkbox2`  | `[ ]`               | `[x]`               |
|`Checkbox3`  | `┌`<br/>` `<br/>`└` | `┌`<br/>`x`<br/>`└` |
|`Checkbox4`  | `┐`<br/>` `<br/>`┘` | `┐`<br/>`x`<br/>`┘` |
|`Checkbox5`  | `┌ ┐`               | `┌x┐`               |
|`Checkbox6`  | `└ ┘`               | `└x┘`               |
|`Checkbox7`  | `╭`<br/>` `<br/>`╰` | `╭`<br/>`x`<br/>`╰` |
|`Checkbox8`  | `╮`<br/>` `<br/>`╯` | `╮`<br/>`x`<br/>`╯` |
|`Checkbox9`  | `╭ ╮`               | `╭x╮`               |
|`Checkbox10` | `╰ ╯`               | `╰x╯`               |
|`Checkbox11` | `┘`<br/>` `<br/>`┐` | `┘`<br/>`╴`<br/>`┐` |
|`Checkbox12` | `└`<br/>` `<br/>`┌` | `└`<br/>`╶`<br/>`┌` |
|`Checkbox13` | `┘ └`               | `┘╵└`               |
|`Checkbox14` | `┐ ┌`               | `┐╷┌`               |
|`Checkbox15` | `╯`<br/>` `<br/>`╮` | `╯`<br/>`╴`<br/>`╮` |
|`Checkbox16` | `╰`<br/>` `<br/>`╭` | `╰`<br/>`╶`<br/>`╭` |
|`Checkbox17` | `╯ ╰`               | `╯╵╰`               |
|`Checkbox18` | `╮ ╭`               | `╮╷╭`               |
|`Checkbox19` | `├ ┤`               | `├x┤`               |
|`Checkbox20` | `┤ ├`               | `┤x├`               |
|`HCheckbox`  | ` `                 | ` `                 |
|`VCheckbox`  | ` `                 | ` `                 |

Each of these types has an identically named lower-case function that returns an
initialized `std::unique_ptr` object of the corresponding type. They each have a
signature that looks like the below:

```cpp
auto checkbox_type(Checkbox::Parameters) -> std::unique_ptr<Checkbox_type>;
```

## Labeled Checkbox Interface

When using any of the below Labeled Checkboxes, the following interface will be
available.

```cpp
/// Pseudo-code, there is no class named Labeled_checkbox
class Labeled_checkbox {
   public:
    using Checkbox_t;
    using Label_t;

   public:
    Checkbox_t& checkbox;
    Label_t& label;

   public:
    explicit Labeled_checkbox(
        typename Label_t::Parameters    label_parameters    = {},
        typename Checkbox_t::Parameters checkbox_parameters = {});
};
```

### Labeled Checkbox Library Types

| Widget Type       | Unchecked Display   | Checked Display     |
|:------------------|:-------------------:|:-------------------:|
|`HCheckbox1_label`<br/>`HCheckbox_label` | `☐ Label`           | `☒ Label`           |
|`HLabel_checkbox1`<br/>`HLabel_checkbox` | `Label ☐`           | `Label ☒`           |
|`VCheckbox1_label`<br/>`VCheckbox_label` | `☐`<br/>`L`<br/>`a`<br/>`b`<br/>`e`<br/>`l`|`☒`<br/>`L`<br/>`a`<br/>`b`<br/>`e`<br/>`l`|
|`VLabel_checkbox1`<br/>`VLabel_checkbox` | `L`<br/>`a`<br/>`b`<br/>`e`<br/>`l`<br/>`☐`|`L`<br/>`a`<br/>`b`<br/>`e`<br/>`l`<br/>`☒`|
|`HCheckbox2_label`  | `[ ] Label`        | `[x] Label`          |
|`HLabel_checkbox2`  | `Label [ ]`        | `Label [x]`
|`VCheckbox2_label`  | `[ ]`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|`[x]`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|
|`VLabel_checkbox2`  | ` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`[ ]`|` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`[x]`|
|`HCheckbox3_label`  | `┌⠀⠀⠀⠀⠀`<br/>`⠀⠀Label`<br/>`└⠀⠀⠀⠀⠀` | `┌⠀⠀⠀⠀⠀`<br/>`x Label`<br/>`└⠀⠀⠀⠀⠀` |
|`HLabel_checkbox4`  | `⠀⠀⠀⠀⠀┐`<br/>`Label⠀⠀`<br/>`⠀⠀⠀⠀⠀┘` | `⠀⠀⠀⠀⠀┐`<br/>`Label x`<br/>`⠀⠀⠀⠀⠀┘` |
|`VCheckbox5_label`  | `┌ ┐`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|`┌x┐`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|
|`VLabel_checkbox6`  | ` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`└ ┘`|` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`└x┘`|
|`HCheckbox7_label`  | `╭⠀⠀⠀⠀⠀`<br/>`⠀⠀Label`<br/>`╰⠀⠀⠀⠀⠀` | `╭⠀⠀⠀⠀⠀`<br/>`x Label`<br/>`╰⠀⠀⠀⠀⠀` |
|`HLabel_checkbox8`  | `⠀⠀⠀⠀⠀╮`<br/>`Label⠀⠀`<br/>`⠀⠀⠀⠀⠀╯` | `⠀⠀⠀⠀⠀╮`<br/>`Label x`<br/>`⠀⠀⠀⠀⠀╯` |
|`VCheckbox9_label`  | `╭ ╮`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|`╭x╮`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|
|`VLabel_checkbox10` | ` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`╰ ╯`|` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`╰x╯`|
|`HCheckbox11_label` | `┘⠀⠀⠀⠀⠀`<br/>`⠀⠀Label`<br/>`┐⠀⠀⠀⠀⠀` | `┘⠀⠀⠀⠀⠀`<br/>`╴ Label`<br/>`┐⠀⠀⠀⠀⠀` |
|`HLabel_checkbox12` | `⠀⠀⠀⠀⠀└`<br/>`Label⠀⠀`<br/>`⠀⠀⠀⠀⠀┌` | `⠀⠀⠀⠀⠀└`<br/>`Label ╶`<br/>`⠀⠀⠀⠀⠀┌` |
|`VCheckbox13_label` | `┘ └`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|`┘╵└`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|
|`VLabel_checkbox14` | ` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`┐ ┌`|` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`┐╷┌`|
|`HCheckbox15_label` | `╯⠀⠀⠀⠀⠀`<br/>`⠀⠀Label`<br/>`╮⠀⠀⠀⠀⠀` | `╯⠀⠀⠀⠀⠀`<br/>`╴ Label`<br/>`╮⠀⠀⠀⠀⠀` |
|`HLabel_checkbox16` | `⠀⠀⠀⠀⠀╰`<br/>`Label⠀⠀`<br/>`⠀⠀⠀⠀⠀╭` | `⠀⠀⠀⠀⠀╰`<br/>`Label ╶`<br/>`⠀⠀⠀⠀⠀╭` |
|`VCheckbox17_label` | `╯ ╰`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|`╯╵╰`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|
|`VLabel_checkbox18` | ` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`╮ ╭`|` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`╮╷╭`|
|`HCheckbox19_label` | `├ ┤ Label`        | `├x┤ Label`          |
|`HLabel_checkbox19` | `Label ├ ┤`        | `Label ├x┤`
|`VCheckbox19_label` | `├ ┤`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|`├x┤`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|
|`VLabel_checkbox19` | ` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`├ ┤`|` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`├x┤`|
|`HCheckbox20_label` | `┤ ├ Label`        | `┤x├ Label`          |
|`HLabel_checkbox20` | `Label ┤ ├`        | `Label ┤x├`
|`VCheckbox20_label` | `┤ ├`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|`┤x├`<br/>` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `|
|`VLabel_checkbox20` | ` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`┤ ├`|` L `<br/>` a `<br/>` b `<br/>` e `<br/>` l `<br/>`┤x├`|

Each of these types has an identically named lower-case function that returns an
initialized `std::unique_ptr` object of the corresponding type. They each have a
signature that looks like the below:

```cpp
auto labeled_checkbox_type(Label::Parameters, Checkbox::Parameters) -> std::unique_ptr<Labeled_checkbox_type>;
```
