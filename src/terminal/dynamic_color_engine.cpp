#include <termox/terminal/dynamic_color_engine.hpp>

#include <mutex>
#include <utility>
#include <vector>

#include <termox/painter/detail/screen.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/output.hpp>

namespace ox::detail {

namespace {

using Processed_colors = std::vector<std::pair<ANSI, True_color>>;

/// Create a Custom_event to update color definitions.
auto dynamic_color_event(Processed_colors colors) -> Custom_event
{
    return {[=] {
        {
            for (auto& [ansi, true_color] : colors)
                System::terminal.term_set_color(ansi, true_color);
        }
        ox::output::refresh();
    }};
}

}  // namespace

void Dynamic_color_event_loop::loop_function()
{
    {
        auto processed   = Processed_colors{};
        auto const guard = std::scoped_lock{colors_mtx_};
        for (auto& [ansi, dynamic] : colors_)
            processed.push_back({ansi, dynamic.get_value()});

        System::post_event(dynamic_color_event(processed));
    }
    Interval_event_loop::loop_function();
}

}  // namespace ox::detail
