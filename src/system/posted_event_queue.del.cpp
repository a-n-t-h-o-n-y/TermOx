#include "system/detail/posted_event_queue.hpp"
#include "system/detail/posted_event.hpp"
#include "system/event.hpp"
#include <algorithm>
#include <utility>

namespace cppurses {
namespace detail {

// Sorts by priority when adding Posted_event.
void Posted_event_queue::add_event(Posted_event pe) {
    // Remove multiple events to the same object for resource heavy events.
    if (pe.event().type() == Event::Paint ||
        pe.event().type() == Event::ClearScreen ||
        pe.event().type() == Event::Resize ||
        pe.event().type() == Event::Move) {
        auto at = std::find(this->begin(), this->end(), pe);
        if (at != this->end()) {
            this->erase(at);
        }
    }
    this->insert(std::move(pe));
}

}  // namespace detail
}  // namespace cppurses
