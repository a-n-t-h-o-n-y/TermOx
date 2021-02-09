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

// TODO can you create a new Event type instead of having to rely on
// std::function?
/// Create a Custom_event to update color definitions.
auto dynamic_color_event(Processed_colors colors) -> Custom_event
{
    return {[=] {
        {
            // linking a Color with a True_color instead, requires more work,
            // look up each glyph on the screen that has this color, and rewrite
            // it. Maybe a function that tells the screen buffer to 'touch' and
            // repaint every glyph with a particular color, doesn't matter if
            // fore or back, then you change the color definition value and when
            // it goes to paint it pulls out the new rgb value to use. There is
            // a bit of difference here to how it used to work. You have to
            // store the returned get_color() value somewhere. Before, the
            // terminal held this info for you and you just told it up change
            // the definition for a particular color. Maybe there is a better
            // way at a different level.
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
