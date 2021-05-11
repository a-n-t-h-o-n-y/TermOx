#include <termox/terminal/dynamic_color_engine.hpp>

#include <algorithm>
#include <chrono>
#include <iterator>
#include <utility>
#include <vector>

#include <termox/common/lockable.hpp>
#include <termox/painter/color.hpp>
#include <termox/system/event.hpp>

namespace ox {

void Dynamic_color_engine::register_color(Color color,
                                          Dynamic_color const& dynamic)
{
    auto const lock = this->Lockable::lock();
    data_.push_back({color, dynamic, Clock_t::now()});
}

void Dynamic_color_engine::unregister_color(Color color)
{
    auto const lock = this->Lockable::lock();
    auto const iter =
        std::find_if(std::cbegin(data_), std::cend(data_),
                     [color](auto const& data) { return data.color == color; });
    if (iter != std::cend(data_))
        data_.erase(iter);
}

void Dynamic_color_engine::clear()
{
    auto const lock = this->Lockable::lock();
    data_.clear();
}

auto Dynamic_color_engine::is_empty() const -> bool
{
    auto const lock = this->Lockable::lock();
    return data_.empty();
}

void Dynamic_color_engine::start()
{
    loop_.run_async([this](Event_queue& q) { this->loop_function(q); });
}

void Dynamic_color_engine::stop()
{
    loop_.exit(0);
    loop_.wait();
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

void Dynamic_color_engine::loop_function(Event_queue& queue)
{
    // The first call to this returns immediately.
    timer_.wait();
    timer_.begin();
    // This resets the Timer interval.
    queue.append(this->get_dynamic_color_event());
}

}  // namespace ox
