#include <cppurses/system/events/delete_event.hpp>

#include <utility>
#include <vector>

#include <cppurses/system/event.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Delete_event::Delete_event(Event_handler* receiver,
                           std::unique_ptr<Widget> removed)
    : Event{Event::Delete, receiver}, removed_{std::move(removed)} {}

bool Delete_event::send() const {
    bool result{receiver_->delete_event()};
    if (removed_ == nullptr) {
        return result;
    }
    for (Widget* w : removed_->children.get_descendants()) {
        w->delete_event();
    }
    removed_.reset();
    return result;
}

bool Delete_event::filter_send(Event_handler* filter) const {
    return filter->delete_event_filter(receiver_);
}

}  // namespace cppurses
