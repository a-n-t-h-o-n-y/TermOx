#include <system_module/detail/posted_event_queue.hpp>
#include <system_module/detail/posted_event.hpp>
#include <system_module/event.hpp>

#include <algorithm>
#include <functional>
#include <utility>

namespace twf {
namespace detail {

// sorts by priority when adding Posted_event
void Posted_event_queue::add_event(Posted_event pe) {
    // Remove multiple paint events to the same object
    if (pe.event().type() == Event::Paint) {
        auto at = std::find(this->begin(), this->end(), pe);
        if (at != this->end()) {
            this->erase(at);
        }
    }
    this->insert(std::move(pe));
}

}  // namespace detail
}  // namespace twf
