#ifndef SYSTEM_DETAIL_POSTED_EVENT_QUEUE_HPP
#define SYSTEM_DETAIL_POSTED_EVENT_QUEUE_HPP
#include "system/detail/posted_event.hpp"
#include <functional>
#include <set>

namespace cppurses {
namespace detail {

class Posted_event_queue : public std::multiset<Posted_event, std::greater<>> {
   public:
    void add_event(Posted_event pe);

   private:
    using std::multiset<Posted_event, std::greater<>>::emplace;
    using std::multiset<Posted_event, std::greater<>>::emplace_hint;
    using std::multiset<Posted_event, std::greater<>>::insert;
};

}  // namespace detail
}  // namespace cppurses
#endif  // SYSTEM_DETAIL_POSTED_EVENT_QUEUE_HPP
