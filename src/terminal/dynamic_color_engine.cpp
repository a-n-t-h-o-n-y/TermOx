#include <termox/terminal/dynamic_color_engine.hpp>

#include <utility>
#include <vector>

#include <termox/painter/color.hpp>
#include <termox/painter/detail/screen.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>

namespace ox {

void Dynamic_color_engine::loop_function()
{
    // The first call to this returns immediately.
    Interval_event_loop::loop_function();
    this->post_dynamic_color_events();
}

void Dynamic_color_engine::post_dynamic_color_events()
{
    if (data_.empty()) {
        this->set_interval(default_interval);
        return;
    }
    auto processed = Dynamic_color_event::Processed_colors{};
    {
        auto const lock    = this->Lockable::lock();
        auto next_interval = [&, this] {
            return std::min_element(std::cbegin(data_), std::cend(data_),
                                    [](auto const& a, auto const& b) {
                                        return a.dynamic.interval <
                                               b.dynamic.interval;
                                    })
                ->dynamic.interval;
        }();
        auto const now = Clock_t::now();
        for (auto& data : data_) {
            auto const time_left = std::chrono::duration_cast<Interval_t>(
                data.dynamic.interval - (now - data.last_event_time));
            if (time_left <= Interval_t{0}) {
                data.last_event_time = now;
                processed.push_back({data.color, data.dynamic.get_value()});
            }
            else {
                next_interval = std::min(next_interval, time_left);
            }
        }
        this->Interval_event_loop::set_interval(next_interval);
    }
    System::post_event(Dynamic_color_event{std::move(processed)});
}

}  // namespace ox
