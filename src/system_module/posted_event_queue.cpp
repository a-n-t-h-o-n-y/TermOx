#include <system_module/detail/posted_event_queue.hpp>
#include <system_module/detail/posted_event.hpp>
#include <system_module/event.hpp>

#include <algorithm>
#include <functional>
#include <utility>
#include <mutex>

namespace twf {
namespace detail {

// sorts by priority when adding Posted_event
void Posted_event_queue::add_event(Posted_event pe) {
    // Remove multiple paint events to the same object
    std::lock_guard<std::mutex> lock(add_mtx_);
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
}  // namespace twf
