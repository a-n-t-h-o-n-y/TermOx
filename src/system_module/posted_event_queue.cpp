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
    auto priority = pe.priority();
    if (this->empty() || this->back().priority() >= priority) {
        this->emplace_back(std::move(pe));
        return;
    }
    // by priority
    auto gt = [](const Posted_event& e, const int& i) {
        return e > i;
    };  // replace with std::greater
    auto at = std::lower_bound(
        this->begin(), this->end(), priority,
        gt);  // use a priority queue and it will automatically sort
    this->emplace(at, std::move(pe));  // no need for the emplace call then
}

}  // namespace detail
}  // namespace twf
