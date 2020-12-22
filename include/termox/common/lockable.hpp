#ifndef TERMOX_COMMON_LOCKABLE_HPP
#define TERMOX_COMMON_LOCKABLE_HPP
#include <mutex>

namespace ox {

template <typename Mutex = std::mutex>
class Lockable {
   public:
    auto lock() -> std::scoped_lock<Mutex> { return std::scoped_lock{mtx_}; }

    auto mutex() -> Mutex& { return mtx_; }

   private:
    Mutex mtx_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_LOCKABLE_HPP
