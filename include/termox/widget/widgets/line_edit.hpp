#ifndef TERMOX_WIDGET_WIDGETS_LINE_EDIT_HPP
#define TERMOX_WIDGET_WIDGETS_LINE_EDIT_HPP
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {

// TODO input_mask option, which shows outline of acceptable text in ghost color
// ex) phone #| (___)___-____
// ip address| ___.___.___.___
// ref: webtoolkit.eu/widgets/forms/line_text-editor

/// Single line Textbox with input validator and various options.
/** Initial text is in ghost color and cleared on initial focus. */
class Line_edit : public Textbox {
   public:
    using Validator_t = std::function<bool(char)>;

    enum class Action { None, Clear };

    enum class Style { None, Underline };

    struct Parameters {
        Glyph_string placeholder_text        = U"";
        std::optional<Validator_t> validator = std::nullopt;
        std::optional<Glyph> veil            = std::nullopt;
        Action on_enter                      = Action::None;
        Color ghost_color                    = Color::Light_gray;
        Style style                          = Style::None;
    };

   public:
    /// Emitted on Enter Key press, sends along the current contents.
    sl::Signal<void(std::string const&)> enter_pressed;

   public:
    /// Construct a Line_edit object with \p placeholder_text in ghost color.
    explicit Line_edit(Glyph_string placeholder_text        = U"",
                       std::optional<Validator_t> validator = std::nullopt,
                       std::optional<Glyph> veil            = std::nullopt,
                       Action on_enter                      = Action::None,
                       Color ghost_color                    = Color::Light_gray,
                       Style style                          = Style::None)
        : Textbox{std::move(placeholder_text)},
          validator_{validator},
          veil_{veil},
          on_enter_{on_enter},
          ghost_color_{ghost_color},
          style_{style}
    {
        using namespace ox::pipe;
        *this | fixed_height(1) | ghost(ghost_color_) | word_wrap(false);
    }

    /// Construct a Line_edit object with \p parameters.
    explicit Line_edit(Parameters parameters)
        : Line_edit{std::move(parameters.placeholder_text),
                    std::move(parameters.validator),
                    parameters.veil,
                    parameters.on_enter,
                    parameters.ghost_color,
                    parameters.style}
    {}

   public:
    /// Reset the Line to the initial state, with ghosted \p placeholder_text.
    void reset(Glyph_string placeholder_text)
    {
        on_initial_ = true;
        this->set_contents(std::move(placeholder_text | fg(ghost_color_)));
    }

    /// Return the current text set in the Line_edit.
    [[nodiscard]] auto text() const -> Glyph_string const&
    {
        return this->Textbox::contents();
    }

    /// Set the input validator, allowing or disallowing specific char types.
    /** Invalid character input will result in no input. */
    void set_validator(Validator_t validator)
    {
        validator_ = std::move(validator);
    }

    /// Turn off the validator so all text input is valid.
    void disable_validator() { validator_ = std::nullopt; }

    /// Return the current validator function object, or nullopt if not set.
    [[nodiscard]] auto validator() const -> std::optional<Validator_t> const&
    {
        return validator_;
    }

    /// Enable veilded display, and set the Glyph used to obscure each Glyph.
    /** Only alters the display, does not alter the text contents. */
    void enable_veil(Glyph veil = U'*')
    {
        veil_ = veil;
        this->update();
    }

    /// Disable veiled display, will display each Glyph as is.
    void disable_veil()
    {
        veil_.reset();
        this->update();
    }

    /// Return the current veil, or nullopt if not set.
    [[nodiscard]] auto veil() const -> std::optional<Glyph> { return veil_; }

    /// Set what text display action takes place on enter key press.
    void on_enter(Action x) { on_enter_ = x; }

    /// Return the currently set text display Action for enter key presses.
    [[nodiscard]] auto on_enter_action() const -> Action { return on_enter_; }

    /// Set color of the placeholder text, before focus has been given to *this.
    void set_ghost_color(Color c)
    {
        ghost_color_ = c;
        if (on_initial_)
            this->Textbox::set_contents(this->text() | fg(ghost_color_));
    }

    /// Return the current ghost color for the placeholder text.
    [[nodiscard]] auto ghost_color() const -> Color { return ghost_color_; }

    /// Set whether the Line_edit has an underline for the entire Widget.
    void set_style(Style x)
    {
        using namespace ox::pipe;
        switch (x) {
            case Style::None: {
                auto const no_ul = discard(Trait::Underline);
                *this | wallpaper(this->get_wallpaper() | no_ul);
                *this | pipe::contents(this->text() | no_ul);
                this->insert_brush | no_ul;
            }
            case Style::Underline: {
                *this | wallpaper(U' ' | Trait::Underline);
                *this | pipe::contents(this->text() | Trait::Underline);
                this->insert_brush | Trait::Underline;
            }
        }
        style_ = x;
        this->update();
    }

    /// Return the currently set style of the Widget.
    [[nodiscard]] auto style() const -> Style { return style_; }

   protected:
    auto key_press_event(Key k) -> bool override;

    auto mouse_wheel_event(Mouse const&) -> bool override { return true; }

    auto focus_in_event() -> bool override
    {
        if (on_initial_)
            this->Textbox::clear();
        on_initial_ = false;
        return Textbox::focus_in_event();
    }

    auto paint_event(Painter& p) -> bool override
    {
        if (veil_.has_value())
            this->set_contents(Glyph_string{*veil_, this->contents().size()});
        return Textbox::paint_event(p);
    }

   private:
    bool on_initial_ = true;

    std::optional<Validator_t> validator_;
    std::optional<Glyph> veil_;
    Action on_enter_;
    Color ghost_color_;
    Style style_;
};

/// Helper function to create a Line_edit instance.
[[nodiscard]] inline auto line_edit(
    Glyph_string placeholder_text                   = U"",
    std::optional<Line_edit::Validator_t> validator = std::nullopt,
    std::optional<Glyph> veil                       = std::nullopt,
    Line_edit::Action on_enter                      = Line_edit::Action::None,
    Color ghost_color                               = Color::Light_gray,
    Line_edit::Style style                          = Line_edit::Style::None)
    -> std::unique_ptr<Line_edit>
{
    return std::make_unique<Line_edit>(std::move(placeholder_text),
                                       std::move(validator), veil, on_enter,
                                       ghost_color, style);
}

/// Helper function to create a Line_edit instance.
[[nodiscard]] inline auto line_edit(Line_edit::Parameters parameters)
    -> std::unique_ptr<Line_edit>
{
    return std::make_unique<Line_edit>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_LINE_EDIT_HPP
