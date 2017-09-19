#include "system/event.hpp"
#include "widget/widget.hpp"

namespace cppurses {

Event::Event(Type type, Widget* receiver) : type_{type}, receiver_{receiver} {}

Event::Type Event::type() const {
    return type_;
}

Widget* Event::receiver() const {
    return receiver_;
}

void Event::set_receiver(Widget* receiver) {
    receiver_ = receiver;
}

bool Event::send_to_all_filters() const {
    const auto& event_filters = receiver_->get_event_filters();
    bool handled = false;
    // Index iteration: event_filters might change size and reallocate.
    for (int i{0}; i < event_filters.size() && !handled; ++i) {
        handled = this->filter_send(static_cast<Widget*>(event_filters[i]));
    }
    return handled;
}

}  // namespace cppurses
