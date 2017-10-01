#ifndef SYSTEM_SYSTEM_HPP
#define SYSTEM_SYSTEM_HPP
#include "system/event_loop.hpp"
#include <signals/slot.hpp>
#include <memory>
#include <utility>

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

    static bool send_event(const Event& event);

    static void exit(int return_code = 0);
    static Widget* head();
    static unsigned max_width();
    static unsigned max_height();
    static detail::Abstract_event_listener* event_listener();
    static Paint_buffer* paint_buffer();
    void set_paint_buffer(std::unique_ptr<Paint_buffer> buffer);
    static void set_palette(std::unique_ptr<Palette> palette);
    static Palette* palette();
    static void set_head(Widget* head_widget);
    void enable_ctrl_characters();
    void disable_ctrl_characters();

    // Slots
    static sig::Slot<void()> quit;

   private:
    static Widget* head_;
    static Event_loop event_loop_;
    static std::unique_ptr<Paint_buffer> paint_buffer_;
    static std::unique_ptr<detail::Abstract_event_listener> event_listener_;
    static std::unique_ptr<Palette> system_palette_;
};

}  // namespace cppurses
#endif  // SYSTEM_SYSTEM_HPP
