#ifndef SYSTEM_MODULE_DETAIL_POSTED_EVENT_QUEUE_HPP
#define SYSTEM_MODULE_DETAIL_POSTED_EVENT_QUEUE_HPP

#include "system_module/detail/posted_event.hpp"
#include <functional>
#include <set>

namespace twf {
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
}  // namespace twf
#endif  // SYSTEM_MODULE_DETAIL_POSTED_EVENT_QUEUE_HPP
