#pragma once

#include <concepts>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <utility>
#include <variant>

#include <esc/event.hpp>
#include <esc/key.hpp>
#include <esc/mouse.hpp>

#include <ox/core/common.hpp>

namespace ox {

using ::esc::Key;
using ::esc::Mod;
using ::esc::Mouse;

// Thread Safe Queue
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Thread safe queue for inter-thread communication.
 *
 * @details This is built specifically for Events where multiple threads enqueue and one
 * thread consumes.
 * @tparam T The type of element to store in the queue.
 */
template <typename T>
class ConcurrentQueue {
   public:
    using value_type = T;

   public:
    ConcurrentQueue() = default;

    ConcurrentQueue(ConcurrentQueue const&) = delete;
    ConcurrentQueue(ConcurrentQueue&&) = delete;

    auto operator=(ConcurrentQueue const&) -> ConcurrentQueue& = delete;
    auto operator=(ConcurrentQueue&&) -> ConcurrentQueue& = delete;

   public:
    /**
     * Adds an element at the back of the queue.
     *
     * @param value The element to enqueue.
     */
    void enqueue(value_type value)
    {
        auto tmp = std::list<value_type>{};
        tmp.push_back(std::move(value));
        {
            auto const lock = std::scoped_lock{mutex_};
            queue_.splice(std::end(queue_), tmp);
        }
        cond_.notify_one();
    }

    /**
     * Removes and retrieves the element at the front of the queue.
     *
     * @details This blocks until an element is available. Uses a condition variable.
     * @return value_type The element at the front of the queue.
     */
    [[nodiscard]] auto pop() -> value_type
    {
        auto lock = std::unique_lock{mutex_};
        cond_.wait(lock, [this] { return !queue_.empty(); });
        // After the wait, the lock is re-acquired.
        auto value = std::move(queue_.front());
        queue_.pop_front();
        return value;
    }

   private:
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::list<value_type> queue_;
};

// Event Handler Response
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Indicates an Event handler wants the application to shut down.
 *
 * @param return_code The return code that will be returned by Application::run().
 */
struct QuitRequest {
    int return_code = 0;
};

/**
 * The response from an Event handler.
 *
 * @details This is so the Event handler can communicate with the Application class.
 * std::nullopt is a request to do nothing special.
 */
using EventResponse = std::optional<QuitRequest>;

// Events
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace event {

struct Timer {
    int id;
};

struct Custom {
    std::function<EventResponse()> action;
};

struct Interrupt {};

}  // namespace event

using Event = std::variant<esc::MousePress,
                           esc::MouseRelease,
                           esc::MouseWheel,
                           esc::MouseMove,
                           esc::KeyPress,
                           esc::KeyRelease,
                           esc::Resize,
                           event::Timer,
                           event::Custom,
                           event::Interrupt>;

/**
 * A thread-safe queue of Events.
 */
using EventQueue = ConcurrentQueue<Event>;

// Event Handler Concepts
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Checks if a type can handle a MousePress event.
 */
template <typename T>
concept HandlesMousePress = requires(T t, Mouse m) {
    { t.handle_mouse_press(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseRelease event.
 */
template <typename T>
concept HandlesMouseRelease = requires(T t, Mouse m) {
    { t.handle_mouse_release(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseWheel event.
 */
template <typename T>
concept HandlesMouseWheel = requires(T t, Mouse m) {
    { t.handle_mouse_wheel(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseMove event.
 */
template <typename T>
concept HandlesMouseMove = requires(T t, Mouse m) {
    { t.handle_mouse_move(m) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a KeyPress event.
 */
template <typename T>
concept HandlesKeyPress = requires(T t, Key k) {
    { t.handle_key_press(k) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a KeyRelease event.
 */
template <typename T>
concept HandlesKeyRelease = requires(T t, Key k) {
    { t.handle_key_release(k) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Resize event.
 */
template <typename T>
concept HandlesResize = requires(T t, esc::Area a) {
    { t.handle_resize(a) } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Timer event.
 */
template <typename T>
concept HandlesTimer = requires(T t, int id) {
    { t.handle_timer(id) } -> std::same_as<EventResponse>;
};

}  // namespace ox