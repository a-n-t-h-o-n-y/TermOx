#include <cppurses/terminal/dynamic_color_engine.hpp>

#include <mutex>
#include <vector>

#include <cppurses/painter/detail/screen.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/output.hpp>

namespace cppurses::detail {

class Dynamic_color_event : public cppurses::Event {
   public:
    Dynamic_color_event(std::vector<Dynamic_color_event_loop::Def>& colors,
                        std::mutex& colors_mtx)
        : Event{Event::Custom, *System::head()},
          colors_{colors},
          colors_mtx_{colors_mtx}
    {}

   public:
    auto send() const -> bool override
    {
        {
            auto const guard = std::lock_guard{colors_mtx_};
            for (auto& c : colors_)
                System::terminal.term_set_color(c.ansi, c.dynamic.get_value());
        }
        cppurses::output::refresh();
        return true;
    }

    auto filter_send(Widget&) const -> bool override { return true; }

   private:
    std::vector<Dynamic_color_event_loop::Def>& colors_;
    std::mutex& colors_mtx_;
};

void Dynamic_color_event_loop::loop_function()
{
    System::post_event<Dynamic_color_event>(colors_, colors_mtx_);
    Interval_event_loop::loop_function();
}

}  // namespace cppurses::detail
