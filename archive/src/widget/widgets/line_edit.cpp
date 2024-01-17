#include <termox/widget/widgets/line_edit.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/align.hpp>

namespace ox {

Line_edit::Line_edit(Glyph_string initial_text,
                     Align alignment,
                     Action focus_in_action,
                     std::optional<Validator_t> validator)
    : Textline_base{std::move(initial_text), alignment, focus_in_action},
      validator_{std::move(validator)}
{}

Line_edit::Line_edit(Parameters p)
    : Line_edit{std::move(p.initial_text), p.alignment, p.focus_in_action,
                std::move(p.validator)}
{}

void Line_edit::set_validator(std::optional<Validator_t> x)
{
    validator_ = std::move(x);
}

/// Turn off the validator so all text input is valid.
void Line_edit::disable_validator() { validator_ = std::nullopt; }

/// Return the current validator function object, or nullopt if not set.
auto Line_edit::validator() const noexcept -> std::optional<Validator_t> const&
{
    return validator_;
}

auto Line_edit::key_press_event(ox::Key k) -> bool
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

auto Line_edit::focus_out_event() -> bool
{
    submitted.emit(this->text().str());
    return Textline_base::focus_out_event();
}

auto line_edit(Glyph_string initial_text,
               Align alignment,
               Line_edit::Action focus_in_action,
               std::optional<Line_edit::Validator_t> validator)
    -> std::unique_ptr<Line_edit>
{
    return std::make_unique<Line_edit>(std::move(initial_text), alignment,
                                       focus_in_action, std::move(validator));
}

/// Helper function to create a Line_edit instance.
auto line_edit(Line_edit::Parameters p) -> std::unique_ptr<Line_edit>
{
    return std::make_unique<Line_edit>(std::move(p));
}

}  // namespace ox
