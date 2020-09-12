#include <cppurses/terminal/dynamic_color_engine.hpp>

#include <mutex>
#include <vector>

#include <cppurses/painter/detail/screen.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/output.hpp>

namespace cppurses::detail {

namespace {

/// Create a Custom_event to update color definitions.
auto dynamic_color_event(std::vector<Dynamic_color_event_loop::Def>& colors,
                         std::mutex& colors_mtx) -> Custom_event
{
    return {[&] {
        {
            auto const guard = std::scoped_lock{colors_mtx};
            for (auto& c : colors)
                System::terminal.term_set_color(c.ansi, c.dynamic.get_value());
        }
        cppurses::output::refresh();
    }};
}

}  // namespace

void Dynamic_color_event_loop::loop_function()
{
    System::post_event(dynamic_color_event(colors_, colors_mtx_));
    Interval_event_loop::loop_function();
}

}  // namespace cppurses::detail
