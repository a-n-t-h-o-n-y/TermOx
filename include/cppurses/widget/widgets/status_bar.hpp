#ifndef CPPURSES_WIDGET_WIDGETS_STATUS_HPP
#define CPPURSES_WIDGET_WIDGETS_STATUS_HPP
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {

class Status_bar : public Label {
   public:
    sig::Signal<void(Glyph_string const&)> status_updated;

   public:
    explicit Status_bar(Glyph_string initial_message = "")
        : Label{std::move(initial_message)}
    {}

    void update_status(Glyph_string message)
    {
        this->set_contents(std::move(message));
        status_updated(this->contents());
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto status_bar(Args&&... args) -> std::unique_ptr<Status_bar>
{
    return std::make_unique<Status_bar>(std::forward<Args>(args)...);
}

namespace slot {

inline auto update_status(Status_bar& sb) -> sig::Slot<void(Glyph)>
{
    auto slot = sig::Slot<void(Glyph)>{[&sb](Glyph message) {
        sb.update_status(Glyph_string{std::move(message)});
    }};
    slot.track(sb.destroyed);
    return slot;
}

inline auto update_status(Status_bar& sb, Glyph_string const& message)
    -> sig::Slot<void()>
{
    auto slot =
        sig::Slot<void()>{[&sb, message] { sb.update_status(message); }};
    slot.track(sb.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_STATUS_HPP
