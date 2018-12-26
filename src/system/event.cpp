#include <cppurses/system/event.hpp>

#include <cstddef>
#include <vector>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

bool Event::send_to_all_filters() const {
    const auto& event_filters = receiver_.get_event_filters();
    auto handled = false;
    // Index iteration: event_filters might change size and reallocate.
    for (auto i = std::size_t{0}; i < event_filters.size() && !handled; ++i) {
        if (event_filters[i]->enabled()) {
            handled = this->filter_send(*event_filters[i]);
        }
    }
    return handled;
}

}  // namespace cppurses
