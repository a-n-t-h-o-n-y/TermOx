#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/status_bar.hpp>

#include <signals/signals.hpp>

#include <utility>

namespace cppurses {

Status_bar::Status_bar(Glyph_string initial_message) : Label{initial_message} {}

void Status_bar::update_status(Glyph_string message) {
    this->set_text(std::move(message));
    status_updated(this->contents());
}

namespace slot {

sig::Slot<void(Glyph_string)> update_status(Status_bar& sb) {
    sig::Slot<void(Glyph_string)> slot{
        [&sb](Glyph_string message) { sb.update_status(std::move(message)); }};
    slot.track(sb.destroyed);
    return slot;
}

sig::Slot<void()> update_status(Status_bar& sb, const Glyph_string& message) {
    sig::Slot<void()> slot{[&sb, message] { sb.update_status(message); }};
    slot.track(sb.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
