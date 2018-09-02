#ifndef CPPURSES_SYSTEM_SYSTEM_HPP
#define CPPURSES_SYSTEM_SYSTEM_HPP
#include <cstddef>
#include <memory>
#include <mutex>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/system/terminal_properties.hpp>

namespace cppurses {
namespace detail {
class User_input_event_loop;
}  // namespace detail
class Widget;
class Event;
class Animation_engine;
class Event_loop;

class System {
   public:
    System();
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&) noexcept = default;
    System& operator=(System&&) noexcept = default;
    ~System();

    int run();

    static bool send_event(const Event& event);
    static void post_event(std::unique_ptr<Event> event);

    template <typename T, typename... Args>
    static void post_event(Args&&... args);

    static Event_loop& find_event_loop();
    static void register_event_loop(Event_loop* loop);
    static void deregister_event_loop(Event_loop* loop);

    static void exit(int return_code = 0);

    static Widget* head();
    static void set_head(Widget* head_widget);

    static Animation_engine& animation_engine();

    static bool exit_requested();

    // Slots
    static sig::Slot<void()> quit;

   private:
    // TODO move below two into own struct
    static std::vector<Event_loop*> running_event_loops_;
    static std::mutex running_loops_mtx_;

    static Widget* head_;
    static bool exit_requested_;
    static detail::User_input_event_loop main_loop_;
    static Animation_engine animation_engine_;

   public:
    /// Provides access to and modification of global terminal properties.
    ///
    /// Most members are unsafe to use if System object has not been initialized
    static Terminal_properties terminal;
};

template <typename T, typename... Args>
void System::post_event(Args&&... args) {
    auto event = std::make_unique<T>(std::forward<Args>(args)...);
    System::post_event(std::move(event));
}

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_SYSTEM_HPP
