#ifndef TERMOX_WIDGET_WIDGETS_STATUS_HPP
#define TERMOX_WIDGET_WIDGETS_STATUS_HPP
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

class Status_bar : public HLabel {
   public:
    sl::Signal<void(Glyph_string const&)> status_updated;

   public:
    explicit Status_bar(Glyph_string initial_message = "")
        : HLabel{{std::move(initial_message)}}
    {}

    void update_status(Glyph_string message)
    {
        this->set_text(std::move(message));
        status_updated(this->get_text());
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto status_bar(Args&&... args) -> std::unique_ptr<Status_bar>
{
    return std::make_unique<Status_bar>(std::forward<Args>(args)...);
}

}  // namespace ox

namespace ox::slot {

inline auto update_status(Status_bar& sb) -> sl::Slot<void(Glyph)>
{
    return link_lifetimes(
        [&sb](Glyph message) {
            sb.update_status(Glyph_string{std::move(message)});
        },
        sb);
}

inline auto update_status(Status_bar& sb, Glyph_string const& message)
    -> sl::Slot<void()>
{
    return link_lifetimes([&sb, message] { sb.update_status(message); }, sb);
}

}  // namespace ox::slot
#endif  // TERMOX_WIDGET_WIDGETS_STATUS_HPP
