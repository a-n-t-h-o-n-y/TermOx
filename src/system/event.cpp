#include "system/event.hpp"
class Widget;

namespace cppurses {

Event::Event(Type type, Widget* reciever) : type_{type}, reciever_{reciever} {}

Event::Type Event::type() const {
    return type_;
}

}  // namespace cppurses
