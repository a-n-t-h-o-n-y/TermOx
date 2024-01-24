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
#include <termox/terminal.hpp>

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

struct Timer {};

}  // namespace event

using Event = std::variant<esc::MousePress,
                           esc::MouseRelease,
                           esc::MouseWheel,
                           esc::MouseMove,
                           esc::KeyPress,
                           esc::KeyRelease,
                           esc::Resize,
                           event::Timer>;

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
concept HandlesTimer = requires(T t) {
    {
        t.handle_timer()
    } -> std::same_as<EventResponse>;
};

// Apply Fn
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief Calls the appropriate handler function on \p handler for the given
 * Event.
 *
 * This will only call the handler if the handler has the appropriate function
 * for the given Event type.
 *
 * @param ev The Event to handle.
 * @param handler The event handler to process the event.
 * @return EventResponse The response from the handler.
 */
template <typename T>
[[nodiscard]] auto apply_event(Event const& ev, T& handler) -> EventResponse
{
    // TODO can this return an optional event response? so if the first layer is
    // null then you do nothing, if the second layer is null you commit changes
    // and if the second is not null then you quit
    return std::visit(
        Overload{[&](esc::KeyPress e) -> EventResponse {
                     if constexpr (HandlesKeyPress<T>) {
                         return handler.handle_key_press(e.key);
                     }
                     else {
                         return std::nullopt;
                     }
                 },
                 [&](esc::KeyRelease e) -> EventResponse {
                     if constexpr (HandlesKeyRelease<T>) {
                         return handler.handle_key_release(e.key);
                     }
                     else {
                         return std::nullopt;
                     }
                 },
                 [&](esc::MousePress e) -> EventResponse {
                     if constexpr (HandlesMousePress<T>) {
                         return handler.handle_mouse_press(e.mouse);
                     }
                     else {
                         return std::nullopt;
                     }
                 },
                 [&](esc::MouseRelease e) -> EventResponse {
                     if constexpr (HandlesMouseRelease<T>) {
                         return handler.handle_mouse_release(e.mouse);
                     }
                     else {
                         return std::nullopt;
                     }
                 },
                 [&](esc::MouseWheel e) -> EventResponse {
                     if constexpr (HandlesMouseWheel<T>) {
                         return handler.handle_mouse_wheel(e.mouse);
                     }
                     else {
                         return std::nullopt;
                     }
                 },
                 [&](esc::MouseMove e) -> EventResponse {
                     if constexpr (HandlesMouseMove<T>) {
                         return handler.handle_mouse_move(e.mouse);
                     }
                     else {
                         return std::nullopt;
                     }
                 },
                 [&](esc::Resize const& e) -> EventResponse {
                     Terminal::current_screen.reset(e.area);
                     Terminal::changes.reset(e.area);
                     if constexpr (HandlesResize<T>) {
                         return handler.handle_resize(e.area);
                     }
                     else {
                         return std::nullopt;
                     }
                 },
                 [&](event::Timer) -> EventResponse {
                     if constexpr (HandlesTimer<T>) {
                         return handler.handle_timer();
                     }
                     else {
                         return std::nullopt;
                     }
                 }},
        ev);
}

}  // namespace ox