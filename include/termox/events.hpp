#pragma once

#include <concepts>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>
#include <variant>

#include <esc/area.hpp>
#include <esc/key.hpp>
#include <esc/mouse.hpp>
#include <esc/point.hpp>

#include <termox/common.hpp>
#include <termox/terminal.hpp>
#include <termox/widget.hpp>

namespace ox {

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
    auto append(T const& value) -> void
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
     * @return T The element at the front of the queue.
     */
    [[nodiscard]] auto pop() -> T
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
    std::queue<T> queue_;
};

// Events
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace event {

struct KeyPress {
    esc::Key key;
};

struct KeyRelease {
    esc::Key key;
};

struct MousePress {
    esc::Mouse mouse;
};

struct MouseRelease {
    esc::Mouse mouse;
};

struct MouseWheel {
    esc::Mouse mouse;
};

struct MouseMove {
    esc::Mouse mouse;
};

struct Move {
    esc::Point coordinates;
};

struct Resize {
    esc::Area size;
};

struct Timer {};

struct Quit {};

}  // namespace event

/**
 * @brief A variant of all possible Events.
 */
using Event = std::variant<event::KeyPress,
                           event::KeyRelease,
                           event::MousePress,
                           event::MouseRelease,
                           event::MouseWheel,
                           event::MouseMove,
                           event::Move,
                           event::Resize,
                           event::Timer,
                           event::Quit>;

/**
 * @brief A thread-safe queue of Events.
 */
using EventQueue = ConcurrentQueue<Event>;

// Event Handler Response
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * @brief Indicates an Event handler wants the application to shut down.
 */
struct QuitRequest {};

/**
 * @brief The response from an Event handler.
 *
 * This is so the Event handler can communicate with the Application class.
 */
using EventResponse = std::optional<std::variant<Canvas, QuitRequest>>;

// Event Handler Concepts
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Checks if a type can handle a KeyPress event.
 */
template <typename T>
concept HandlesKeyPress = Widget<T> && requires(T t, esc::Key k) {
    {
        t.handle_key_press(k)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a KeyRelease event.
 */
template <typename T>
concept HandlesKeyRelease = Widget<T> && requires(T t, esc::Key k) {
    {
        t.handle_key_release(k)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MousePress event.
 */
template <typename T>
concept HandlesMousePress = Widget<T> && requires(T t, esc::Mouse m) {
    {
        t.handle_mouse_press(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseRelease event.
 */
template <typename T>
concept HandlesMouseRelease = Widget<T> && requires(T t, esc::Mouse m) {
    {
        t.handle_mouse_release(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseWheel event.
 */
template <typename T>
concept HandlesMouseWheel = Widget<T> && requires(T t, esc::Mouse m) {
    {
        t.handle_mouse_wheel(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a MouseMove event.
 */
template <typename T>
concept HandlesMouseMove = Widget<T> && requires(T t, esc::Mouse m) {
    {
        t.handle_mouse_move(m)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Move event.
 */
template <typename T>
concept HandlesMove = Widget<T> && requires(T t, esc::Point p) {
    {
        t.handle_move(p)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Resize event.
 */
template <typename T>
concept HandlesResize = Widget<T> && requires(T t, esc::Area a) {
    {
        t.handle_resize(a)
    } -> std::same_as<EventResponse>;
};

/**
 * Checks if a type can handle a Timer event.
 */
template <typename T>
concept HandlesTimer = Widget<T> && requires(T t) {
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
 * @param handler The Widget to handle the Event.
 * @return EventResponse The response from the handler.
 */
template <Widget T>
[[nodiscard]] auto apply_event(Event const& ev, T& handler) -> EventResponse
{
    return std::visit(
        Overload{
            [&](event::KeyPress e) -> EventResponse {
                if constexpr (HandlesKeyPress<T>) {
                    return handler.handle_key_press(e.key);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](event::KeyRelease e) -> EventResponse {
                if constexpr (HandlesKeyRelease<T>) {
                    return handler.handle_key_release(e.key);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](event::MousePress e) -> EventResponse {
                if constexpr (HandlesMousePress<T>) {
                    return handler.handle_mouse_press(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](event::MouseRelease e) -> EventResponse {
                if constexpr (HandlesMouseRelease<T>) {
                    return handler.handle_mouse_release(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](event::MouseWheel e) -> EventResponse {
                if constexpr (HandlesMouseWheel<T>) {
                    return handler.handle_mouse_wheel(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](event::MouseMove e) -> EventResponse {
                if constexpr (HandlesMouseMove<T>) {
                    return handler.handle_mouse_move(e.mouse);
                }
                else {
                    return std::nullopt;
                }
            },
            [&](event::Move e) -> EventResponse {
                // TODO this doesn't work, you need to directly
                // send it, so instead maybe move isn't an event
                // and instead the layout just sets it directly
                // but you do run into what you thought of
                // earlier where an app is drawing its coords but
                // it doesn't know to update them because layout
                // didn't bother with it. Layout could call the
                // handler directly.. But that is just moving
                // responsibility?
                //  handler.set_coordinates(x.coordinates);
                return std::nullopt;
            },
            [&](event::Resize const& e) -> EventResponse {
                // head widget has no layout parent, so set it here.
                handler.size = e.size;
                Terminal::current_screen.reset(e.size);
                Terminal::changes.reset(e.size);
                if constexpr (HandlesResize<T>) {
                    return handler.handle_resize(e.size);
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
            },
            [](event::Quit) -> EventResponse { return QuitRequest{}; },
        },
        ev);
}

}  // namespace ox