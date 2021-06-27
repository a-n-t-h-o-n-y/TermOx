#ifndef TERMOX_WIDGET_WIDGETS_LINE_EDIT_HPP
#define TERMOX_WIDGET_WIDGETS_LINE_EDIT_HPP
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/widgets/detail/textline_base.hpp>

namespace ox {

class Line_edit : public detail::Textline_base {
   public:
    using Validator_t = std::function<bool(char32_t)>;

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
                       std::optional<Validator_t> validator = std::nullopt);

    explicit Line_edit(Parameters p);

   public:
    /// Set the input validator, allowing or disallowing specific char values.
    /** Invalid character input will result in no input. */
    void set_validator(std::optional<Validator_t> x);

    /// Turn off the validator so all text input is valid.
    void disable_validator();

    /// Return the current validator function object, or nullopt if not set.
    [[nodiscard]] auto validator() const noexcept
        -> std::optional<Validator_t> const&;

   protected:
    auto key_press_event(ox::Key k) -> bool override;

    auto focus_out_event() -> bool override;

   private:
    std::optional<Validator_t> validator_;
};

/// Helper function to create a Line_edit instance.
[[nodiscard]] auto line_edit(
    Glyph_string initial_text                       = U"",
    Align alignment                                 = Align::Left,
    Line_edit::Action focus_in_action               = Line_edit::Action::None,
    std::optional<Line_edit::Validator_t> validator = std::nullopt)
    -> std::unique_ptr<Line_edit>;

/// Helper function to create a Line_edit instance.
[[nodiscard]] auto line_edit(Line_edit::Parameters p)
    -> std::unique_ptr<Line_edit>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_LINE_EDIT_HPP
