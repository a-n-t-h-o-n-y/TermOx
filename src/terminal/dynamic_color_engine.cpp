#include <termox/terminal/dynamic_color_engine.hpp>

#include <utility>
#include <vector>

#include <termox/painter/color.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

void Dynamic_color_engine::loop_function(Event_queue& queue)
{
    // The first call to this returns immediately.
    timer_.wait();
    timer_.begin();
    // This resets the Timer interval.
    queue.append(this->get_dynamic_color_event());
}

auto Dynamic_color_engine::get_dynamic_color_event() -> Dynamic_color_event
{
    auto processed = Dynamic_color_event::Processed_colors{};
    if (data_.empty()) {
        timer_.set_interval(default_interval);
        return Dynamic_color_event{processed};
    }
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
        timer_.set_interval(next_interval);
    }
    return Dynamic_color_event{std::move(processed)};
}

}  // namespace ox
