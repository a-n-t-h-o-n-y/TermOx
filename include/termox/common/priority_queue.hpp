#ifndef TERMOX_COMMON_PRIORITY_QUEUE_HPP
#define TERMOX_COMMON_PRIORITY_QUEUE_HPP
#include <cstdint>
#include <queue>
#include <vector>

#include <termox/common/lockable.hpp>

namespace ox {

/// Wraps std::priority_queue to attach an int priority type to stored objects.
/** Uses std::uint8_t as priority type, 0 is lowest priority, 255 is highest. */
template <typename T, typename Priority = std::uint8_t>
class Priority_queue {
   public:
    using Priority_t = Priority;

   public:
    /// Returns a const reference to the element with the highest priority.
    auto top() const -> T const& { return queue_.top().second; }

    /// Returns true if no elements in queue.
    auto empty() const -> bool { return queue_.empty(); }

    /// Returns the number of elements currently in the queue.
    auto size() const -> std::size_t { return queue_.size(); }

    /// Copy an element into the queue with the given priority.
    void push(Priority p, T const& x) { queue_.push(std::pair{p, x}); }

    /// Move an element into the queue with the given priority.
    void push(Priority p, T&& x) { queue_.push(std::pair{p, std::move(x)}); }

    /// Moves top element from the queue and returns it, shrinking the queue.
    auto pop() -> T
    {
        auto result = std::move(const_cast<T&>(this->top()));
        queue_.pop();
        return std::move(result);
    }

   private:
    using Value_t = std::pair<Priority, T>;

   private:
    struct Compare {
        auto operator()(Value_t const& x, Value_t const& y) const -> bool
        {
            return x.first < y.first;
        }
    };

   private:
    using Queue_t = std::priority_queue<Value_t, std::vector<Value_t>, Compare>;
    Queue_t queue_;
};
}  // namespace ox
#endif  // TERMOX_COMMON_PRIORITY_QUEUE_HPP
