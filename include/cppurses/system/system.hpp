#ifndef SYSTEM_SYSTEM_HPP
#define SYSTEM_SYSTEM_HPP

#include "system/event.hpp"
#include "system/object.hpp"
#include "painter/detail/ncurses_paint_engine.hpp"
#include "painter/paint_engine.hpp"
#include "painter/palette.hpp"
#include <signals/slot.hpp>
#include <memory>

namespace cppurses {

class Widget;

class System : public Object {
   public:
    explicit System(std::unique_ptr<Paint_engine> engine =
                        std::make_unique<detail::NCurses_paint_engine>());
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    System(System&&) noexcept = default;             // NOLINT
    System& operator=(System&&) noexcept = default;  // NOLINT
    ~System() override;

    static void post_event(Object* obj,
                           std::unique_ptr<Event> event,
                           int priority = 0);
    static void remove_posted_event(Event* event);
    static bool send_event(Object* obj, const Event& event);
    static void send_posted_events(Object* obj_filter = nullptr,
                                   Event::Type etype_filter = Event::None);
    static bool notify(Object* obj, const Event& event);
    static void exit(int return_code = 0);
    static Object* head();
    static unsigned max_width();
    static unsigned max_height();
    static Paint_engine* paint_engine();
    static void set_paint_engine(std::unique_ptr<Paint_engine> engine);
    static Widget* focus_widget();
    static void set_focus_widget(Widget* widg, bool clear_focus = true);
    static void cycle_tab_focus();
    static void set_palette(std::unique_ptr<Palette> palette);
    static Palette* palette();
    static void set_head(Object* obj);
    void handle_ctrl_characters(bool enable) {
        this->paint_engine()->set_ctrl_char(enable);
    }

    int run();

    // Slots
    static sig::Slot<void()> quit;

    friend class Abstract_event_dispatcher;

   private:
    static Object* head_;
    static std::unique_ptr<Paint_engine> engine_;
    static Widget* focus_widg_;
    static std::unique_ptr<Palette> system_palette_;
    static bool notify_helper(Object* obj, const Event& event);
};

}  // namespace cppurses
#endif  // SYSTEM_SYSTEM_HPP
