#pragma once

#include <concepts>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>
#include <variant>

#include <esc/event.hpp>
#include <esc/key.hpp>
#include <esc/mouse.hpp>

#include <termox/common.hpp>

namespace ox {

using ::esc::Key;
using ::esc::Mouse;

// Thread Safe Queue
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief Thread safe queue for inter-thread communication.
 *
 * This is built specifically for Events where multiple threads append and one
 * thread consumes.
 *
 * @tparam T The type of element to store in the queue.
 */
template <typename T>
class ConcurrentQueue {
   public:
    using value_type = T;

   public:
    ConcurrentQueue() = default;

    ConcurrentQueue(ConcurrentQueue const&) = delete;
    ConcurrentQueue(ConcurrentQueue&&)      = delete;

    auto operator=(ConcurrentQueue const&) -> ConcurrentQueue& = delete;
    auto operator=(ConcurrentQueue&&) -> ConcurrentQueue&      = delete;

   public:
    /**
     * @brief Appends an element to the back of the queue.
     *
     * @param value The element to append.
     */
    auto append(value_type const& value) -> void
    {
        {
            auto const lock = std::scoped_lock{mutex_};
            queue_.push(value);
        }
        cond_.notify_one();
    }

    /**
     * @brief Removes and retrieves the element at the front of the queue.
     *
     * This will block until an element is available. Uses a condition variable.
     *
     * @return value_type The element at the front of the queue.
     */
    [[nodiscard]] auto pop() -> value_type
    {
        auto lock = std::unique_lock{mutex_};
        cond_.wait(lock, [this] { return !queue_.empty(); });
        auto value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

   private:
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<value_type> queue_;
};

// Events
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace event {

struct Timer {
    int id;
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
                           event::Interrupt>;

/**
 * @brief A thread-safe queue of Events.
 */
using EventQueue = ConcurrentQueue<Event>;

// Event Handler Response
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief Indicates an Event handler wants the application to shut down.
 *
 * @param return_code The return code that will be returned by
 * Application::run().
 */
struct QuitRequest {
    int return_code = 0;
};

/**
 * @brief The response from an Event handler.
 *
 * This is so the Event handler can communicate with the Application class.
 * std::nullopt is a request to do nothing special.
 */
using EventResponse = std::optional<QuitRequest>;

// Event Handler Concepts
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Checks if a type can handle a KeyPress event.
 */
template <typename T>
concept HandlesKeyPress = requires(T t, Key k) {
    {
        t.handle_key_press(k)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a KeyRelease event.
 */
template <typename T>
concept HandlesKeyRelease = requires(T t, Key k) {
    {
        t.handle_key_release(k)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MousePress event.
 */
template <typename T>
concept HandlesMousePress = requires(T t, Mouse m) {
    {
        t.handle_mouse_press(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseRelease event.
 */
template <typename T>
concept HandlesMouseRelease = requires(T t, Mouse m) {
    {
        t.handle_mouse_release(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseWheel event.
 */
template <typename T>
concept HandlesMouseWheel = requires(T t, Mouse m) {
    {
        t.handle_mouse_wheel(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseMove event.
 */
template <typename T>
concept HandlesMouseMove = requires(T t, Mouse m) {
    {
        t.handle_mouse_move(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Resize event.
 */
template <typename T>
concept HandlesResize = requires(T t, esc::Area a) {
    {
        t.handle_resize(a)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Timer event.
 */
template <typename T>
concept HandlesTimer = requires(T t, int id) {
    {
        t.handle_timer(id)
    } -> std::same_as<EventResponse>;
};

}  // namespace ox