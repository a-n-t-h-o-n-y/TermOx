#ifndef POSTED_EVENT_QUEUE_HPP
#define POSTED_EVENT_QUEUE_HPP

#include "posted_event.hpp"

#include <functional>
#include <set>
#include <mutex>

namespace twf {
namespace detail {

class Posted_event_queue
    : public std::multiset<Posted_event, std::greater<Posted_event>> {
   public:
    void add_event(Posted_event ev);
    std::mutex add_mtx_;
    std::mutex send_mtx_;

   private:
    using std::multiset<Posted_event, std::greater<Posted_event>>::emplace;
    using std::multiset<Posted_event, std::greater<Posted_event>>::emplace_hint;
    using std::multiset<Posted_event, std::greater<Posted_event>>::insert;
};

}  // namespace detail
}  // namespace twf
#endif  // POSTED_EVENT_QUEUE_HPP
