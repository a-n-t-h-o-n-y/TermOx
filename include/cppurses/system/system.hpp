#ifndef SYSTEM_SYSTEM_HPP
#define SYSTEM_SYSTEM_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/detail/user_input_event_loop.hpp>
#include <cppurses/system/event_loop.hpp>

namespace cppurses {
namespace detail {
class Abstract_event_listener;
}  // namespace detail
class Widget;
class Event;
class Paint_buffer;
class Palette;

class System {
   public:
    System();
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&) noexcept = default;             // NOLINT
    System& operator=(System&&) noexcept = default;  // NOLINT
    ~System();

    int run();

    static void post_event(std::unique_ptr<Event> event);

    template <typename T, typename... Args>
    static void post_event(Args&&... args) {
        auto event = std::make_unique<T>(std::forward<Args>(args)...);
        System::post_event(std::move(event));
    }

    // TODO How do you handle mutliple event_loop types? virtual funcs?
    // It might actually work as is since you are only posting events to the
    // loop from this function and that is something the base class handles.
    // Look over it though.
    static Event_loop& find_event_loop();

    static bool send_event(const Event& event);

    static void exit(int return_code = 0);
    static Widget* head();
    static std::size_t max_width();
    static std::size_t max_height();
    static detail::Abstract_event_listener* event_listener();
    static Paint_buffer& paint_buffer();
    static void set_palette(std::unique_ptr<Palette> palette);
    static Palette* palette();
    static void set_head(Widget* head_widget);
    void enable_ctrl_characters();
    void disable_ctrl_characters();
    static Animation_engine& animation_engine();
    static detail::NCurses_paint_engine& paint_engine();

    // Slots
    static sig::Slot<void()> quit;

   private:
    static Widget* head_;
    static detail::User_input_event_loop main_loop_;
    static Animation_engine animation_engine_;
    static Paint_buffer paint_buffer_;
    static std::unique_ptr<detail::Abstract_event_listener> event_listener_;
    static std::unique_ptr<Palette> system_palette_;

    // TODO This is in paint_buffer now remove this.
    static detail::NCurses_paint_engine paint_engine_;
};

}  // namespace cppurses
#endif  // SYSTEM_SYSTEM_HPP
