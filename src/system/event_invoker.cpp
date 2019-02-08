#include <cppurses/system/detail/event_invoker.hpp>

#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>

// #define DEBUG_EVENT_INVOKER

#if defined(DEBUG_EVENT_INVOKER)
#include <fstream>
#include <string>
#include <unordered_map>

#include <cppurses/system/detail/event_as_string.hpp>
#include <cppurses/widget/widget.hpp>
#endif

namespace {
using namespace cppurses;

/// Check whether you can ignore an Event, based on the filter type.
bool is_ignorable(Event::Type event, Event::Type filter) {
    const bool ignore_paint = (event == Event::Paint && filter != Event::Paint);
    const bool ignore_delete =
        (event == Event::Delete && filter != Event::Delete);
    return ignore_paint || ignore_delete;
}

bool is_ignorable(const Widget& object, const Widget* filter) {
    return (filter != nullptr && (&object) != filter);
}

}  // namespace

namespace cppurses {
class Widget;
namespace detail {

void Event_invoker::invoke(Event_queue& queue,
                           Event::Type type_filter,
                           Widget* object_filter) {
#if defined(DEBUG_EVENT_INVOKER)
    std::unordered_map<Widget*, std::vector<Event::Type>> widgs;
#endif
    auto event_iter = std::begin(queue.queue_);
    while (event_iter != std::end(queue.queue_)) {
        auto& receiver = (*event_iter)->receiver();
        auto event_type = (*event_iter)->type();
        if (is_ignorable(event_type, type_filter) ||
            is_ignorable(receiver, object_filter)) {
            ++event_iter;
        } else if (type_filter == Event::None || type_filter == event_type) {
#if defined(DEBUG_EVENT_INVOKER)
            widgs[&receiver].push_back(event_type);
#endif
            auto event = std::move(*event_iter);
            queue.queue_.erase(event_iter);
            System::send_event(*event);
            event_iter = std::begin(queue.queue_);
        }
    }
#if defined(DEBUG_EVENT_INVOKER)
    auto l = std::ofstream{"events_log.txt", std::ios::app};
    for (const auto& widg_event : widgs) {
        l << widg_event.first;
        l << ' ' << widg_event.first->name();
        l << ":\n\t";
        auto seperator = std::string{""};
        for (Event::Type event_t : widg_event.second) {
            l << seperator << event_type_as_string(event_t);
            seperator = ", ";
        }
        l << '\n';
    }
    l << "\n--->End Queue Invokation. type_filter: "
      << event_type_as_string(type_filter);
    l << std::endl;
#endif
}

}  // namespace detail
}  // namespace cppurses
