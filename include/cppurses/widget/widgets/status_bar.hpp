#ifndef CPPURSES_WIDGET_WIDGETS_STATUS_HPP
#define CPPURSES_WIDGET_WIDGETS_STATUS_HPP
#include <signals/signals.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {

class Status_bar : public Label {
   public:
    explicit Status_bar(Glyph_string initial_message = "");
    void update_status(Glyph_string message);

    // Signals
    sig::Signal<void(const Glyph_string&)> status_updated;
};

namespace slot {

sig::Slot<void(Glyph_string)> update_status(Status_bar& sb);
sig::Slot<void()> update_status(Status_bar& sb, const Glyph_string& message);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_STATUS_HPP
