#ifndef TERMOX_COMMON_LISTENER_THREAD_HPP
#define TERMOX_COMMON_LISTENER_THREAD_HPP
#include <cassert>
#include <condition_variable>
#include <future>
#include <mutex>
#include <utility>

namespace ox {

/// Thread that waits to be notified, then performs some action.
/** Parameterized on the action that will be taken, with Function_t. This is
 *  basically a wrapped condition variable that helps you launch and exit a new
 *  thread. */
template <typename Function_t>
class Listener_thread {
   public:
    /// Launch the listener thread
    /** Function_t can be any (zero argument)invokable with a move or copy
     *  constructor. */
    Listener_thread(Function_t&& action)
        : action_{std::forward<Function_t>(action)}
    {}

    /// Shutdown the listener thread.
    ~Listener_thread()
    {
        this->exit();
        this->wait();
    }

   public:
    /// Start the thread that will wait for notification.
    void launch()
    {
        assert(!fut_.valid());
        fut_ = std::async(std::launch::async, [this] {
            exit_     = false;
            notify_   = false;
            auto lock = std::unique_lock{mtx_};
            while (true) {
                cv_.wait(lock, [this] { return notify_ || exit_; });
                if (notify_) {
                    action_();
                    notify_ = false;
                }
                if (exit_) {
                    exit_ = false;
                    break;
                }
            }
        });
    }

    /// Wakes up the thread and has it run the action once.
    void notify()
    {
        {
            auto const lock = std::lock_guard{mtx_};
            notify_         = true;
        }
        cv_.notify_one();
    }

    /// Notify the thread to exit.
    void exit()
    {
        auto const lock = std::lock_guard{mtx_};
        exit_           = true;
    }

    /// Blocks until the thread is dead.
    void wait()
    {
        fut_.wait();
        if (fut_.valid())
            fut_.get();
    }

   private:
    bool exit_   = false;
    bool notify_ = false;
    Function_t action_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::future<void> fut_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_LISTENER_THREAD_HPP
