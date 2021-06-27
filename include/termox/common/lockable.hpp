#ifndef TERMOX_COMMON_LOCKABLE_HPP
#define TERMOX_COMMON_LOCKABLE_HPP
#include <mutex>

namespace ox {

template <typename Mutex = std::mutex>
class Lockable {
   public:
    [[nodiscard]] auto lock() const -> std::scoped_lock<Mutex>
    {
        return std::scoped_lock{mtx_};
    }

    [[nodiscard]] auto mutex() const -> Mutex& { return mtx_; }

   private:
    mutable Mutex mtx_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_LOCKABLE_HPP
