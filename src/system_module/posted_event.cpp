#include <system_module/detail/posted_event.hpp>

#include <memory>

namespace twf {
namespace detail {

Posted_event::Posted_event(Object* obj, std::unique_ptr<Event> ev, int priority)
    : reciever_{obj}, event_{std::move(ev)}, priority_{priority} {}

bool operator<(const Posted_event& x, const Posted_event& y) {
    return x.priority() < y.priority();
}

bool operator>(const Posted_event& x, const Posted_event& y) {
    return y < x;
}

bool operator<(int priority, const Posted_event& pe) {
    return priority < pe.priority();
}

bool operator<(const Posted_event& pe, int priority) {
    return pe.priority() < priority;
}

bool operator>(int priority, const Posted_event& pe) {
    return pe < priority;
}

bool operator>(const Posted_event& pe, int priority) {
    return priority < pe;
}

bool operator==(const Posted_event& x, const Posted_event& y) {
    return (x.reciever() == y.reciever()) && (x.event() == y.event());
}

}  // namespace detail
}  // namespace twf
