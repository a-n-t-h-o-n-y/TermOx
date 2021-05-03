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
#include <termox/widget/widgets/detail/textline_base.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {

class Line_edit : public detail::Textline_base {
   public:
    using Validator_t = std::function<bool(char)>;

    struct Parameters {
        Glyph_string initial_text            = U"";
        Align alignment                      = Align::Left;
        Action focus_in_action               = Action::None;
        std::optional<Validator_t> validator = std::nullopt;
    };

   public:
    /// Emitted on Enter key press, sends along the current contents.
    sl::Signal<void(std::string const&)> submitted;

    /// Emitted on every printable key press, sends along the current contents.
    sl::Signal<void(std::string const&)> text_modified;

   public:
    explicit Line_edit(Glyph_string initial_text            = U"",
                       Align alignment                      = Align::Left,
                       Action focus_in_action               = Action::None,
                       std::optional<Validator_t> validator = std::nullopt)
        : Textline_base{std::move(initial_text), alignment, focus_in_action},
          validator_{std::move(validator)}
    {}

    explicit Line_edit(Parameters p)
        : Line_edit{std::move(p.initial_text), p.alignment, p.focus_in_action,
                    std::move(p.validator)}
    {}

   public:
    /// Set the input validator, allowing or disallowing specific char values.
    /** Invalid character input will result in no input. */
    void set_validator(std::optional<Validator_t> x)
    {
        validator_ = std::move(x);
    }

    /// Turn off the validator so all text input is valid.
    void disable_validator() { validator_ = std::nullopt; }

    /// Return the current validator function object, or nullopt if not set.
    [[nodiscard]] auto validator() const noexcept
        -> std::optional<Validator_t> const&
    {
        return validator_;
    }

   protected:
    auto key_press_event(ox::Key k) -> bool override
    {
        if (k == Key::Enter)
            submitted.emit(this->text().str());

        auto const c = key_to_char32(k);
        if (c != U'\0' && (!validator_ || (*validator_)(c))) {
            core_.insert_at_cursor(c);
            this->update();
            text_modified.emit(this->text().str());
        }
        return Textline_base::key_press_event(k);
    }

    auto focus_out_event() -> bool override
    {
        submitted.emit(this->text().str());
        return Textline_base::focus_out_event();
    }

   private:
    std::optional<Validator_t> validator_;
};

/// Helper function to create a Line_edit instance.
[[nodiscard]] inline auto line_edit(
    Glyph_string initial_text                       = U"",
    Align alignment                                 = Align::Left,
    Line_edit::Action focus_in_action               = Line_edit::Action::None,
    std::optional<Line_edit::Validator_t> validator = std::nullopt)
    -> std::unique_ptr<Line_edit>
{
    return std::make_unique<Line_edit>(std::move(initial_text), alignment,
                                       focus_in_action, std::move(validator));
}

/// Helper function to create a Line_edit instance.
[[nodiscard]] inline auto line_edit(Line_edit::Parameters p)
    -> std::unique_ptr<Line_edit>
{
    return std::make_unique<Line_edit>(std::move(p));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_LINE_EDIT_HPP
