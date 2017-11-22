#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/deferred_delete_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Deferred_delete_event::Deferred_delete_event(Event_handler* receiver)
    : Event{Event::DeferredDelete, nullptr}, to_delete_{receiver} {
    Widget* parent = static_cast<Widget*>(receiver)->parent();
    this->set_receiver(parent);
}

Widget* Deferred_delete_event::to_delete() const {
    return static_cast<Widget*>(to_delete_);
}

bool Deferred_delete_event::send() const {
    if (receiver_ == nullptr) {
        System::exit();
        return true;
    }
    return receiver_->deferred_delete_event(to_delete_);
}

bool Deferred_delete_event::filter_send(Event_handler* filter) const {
    return filter->deferred_delete_event_filter(receiver_, to_delete_);
}

}  // namespace cppurses
