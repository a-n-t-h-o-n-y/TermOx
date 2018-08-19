#include <cppurses/system/event.hpp>

#include <cstddef>
#include <vector>

#include <cppurses/system/event_handler.hpp>

namespace cppurses {

Event::Event(Type type, Event_handler* receiver)
    : type_{type}, receiver_{receiver} {}

Event::Type Event::type() const {
    return type_;
}

Event_handler* Event::receiver() const {
    return receiver_;
}

void Event::set_receiver(Event_handler* receiver) {
    receiver_ = receiver;
}

bool Event::send_to_all_filters() const {
    const auto& event_filters = receiver_->get_event_filters();
    bool handled = false;
    // Index iteration: event_filters might change size and reallocate.
    for (std::size_t i{0}; i < event_filters.size() && !handled; ++i) {
        if (event_filters[i]->enabled()) {
            handled = this->filter_send(event_filters[i]);
        }
    }
    return handled;
}

bool Event::operator==(const Event& other) const {
    return (this->type_ == other.type_) && (this->receiver_ == other.receiver_);
}

}  // namespace cppurses
