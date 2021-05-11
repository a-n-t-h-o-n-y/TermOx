#ifndef TERMOX_WIDGET_WIDGETS_BUTTON_HPP
#define TERMOX_WIDGET_WIDGETS_BUTTON_HPP
#include <memory>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widget.hpp>

namespace ox {
class Painter;

/// Button widget that emits Signal on a left mouse button press.
class Button : public Widget {
   public:
    struct Parameters {
        Glyph_string text;
    };

   public:
    /// Emitted on a left mouse button press.
    sl::Signal<void()> pressed;

    /// Emitted on a left mouse button release.
    sl::Signal<void()> released;

   public:
    /// Construct a Button with centered \p text.
    explicit Button(Glyph_string text = U"");

    /// Construct a Button with Parameters.
    explicit Button(Parameters p);

   public:
    /// Set the text and repaint.
    void set_text(Glyph_string text);

    /// Return the current text.
    [[nodiscard]] auto text() const -> Glyph_string const&;

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override;

    auto mouse_release_event(Mouse const& m) -> bool override;

    auto paint_event(Painter& p) -> bool override;

   private:
    Glyph_string text_;
};

/// Helper function to create a Button instance.
[[nodiscard]] auto button(Glyph_string text = U"") -> std::unique_ptr<Button>;

/// Helper function to create a Button instance.
[[nodiscard]] auto button(Button::Parameters p) -> std::unique_ptr<Button>;

/// Button that changes its background Color on press and release events.
class Push_button : public Button {
   public:
    struct Parameters {
        Glyph_string text;
        Color pressed  = Color::Foreground;
        Color released = Color::Background;
    };

   public:
    /// Construct a new Push_button
    explicit Push_button(Glyph_string text = U"",
                         Color pressed     = Color::Foreground,
                         Color released    = Color::Background);

    /// Construct a new Push_button
    explicit Push_button(Parameters p);

   public:
    /// Set the background Color of the Button when pressed Signal is emitted.
    void set_pressed_color(Color c);

    /// Return the current Color assigned to mouse press events.
    [[nodiscard]] auto get_pressed_color() const -> Color;

    /// Set the background Color of the Button when released Signal is emitted.
    void set_released_color(Color c);

    /// Return the current Color assigned to mouse release events.
    [[nodiscard]] auto get_released_color() const -> Color;

   private:
    Color pressed_color_;
    Color released_color_;
};

/// Helper function to create a Push_button instance.
[[nodiscard]] auto push_button(Glyph_string text = U"",
                               Color pressed     = Color::Foreground,
                               Color released    = Color::Background)
    -> std::unique_ptr<Push_button>;

/// Helper function to create a Push_button instance.
[[nodiscard]] auto push_button(Push_button::Parameters p)
    -> std::unique_ptr<Push_button>;

/// Width or Height 1 Button
template <template <typename> typename Layout_t>
class Thin_button : public Button {
   public:
    using Button::Parameters;

   public:
    explicit Thin_button(Glyph_string text = U"");

    explicit Thin_button(Parameters p);
};

/// Helper function to create a Thin_button instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto thin_button(Glyph_string text = U"")
    -> std::unique_ptr<Thin_button<Layout_t>>;

/// Helper function to create a Thin_button instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto thin_button(typename Thin_button<Layout_t>::Parameters p)
    -> std::unique_ptr<Thin_button<Layout_t>>;

using HThin_button = Thin_button<layout::Horizontal>;

/// Helper function to create an HThin_button instance.
[[nodiscard]] auto hthin_button(Glyph_string text = U"")
    -> std::unique_ptr<HThin_button>;

/// Helper function to create an HThin_button instance.
[[nodiscard]] auto hthin_button(HThin_button::Parameters p)
    -> std::unique_ptr<HThin_button>;

using VThin_button = Thin_button<layout::Vertical>;

/// Helper function to create a VThin_button instance.
[[nodiscard]] auto vthin_button(Glyph_string text = U"")
    -> std::unique_ptr<VThin_button>;

/// Helper function to create a VThin_button instance.
[[nodiscard]] auto vthin_button(VThin_button::Parameters p)
    -> std::unique_ptr<VThin_button>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_BUTTON_HPP
