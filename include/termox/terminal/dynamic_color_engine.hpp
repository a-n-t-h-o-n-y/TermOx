#ifndef TERMOX_TERMINAL_DYNAMIC_COLOR_ENGINE_HPP
#define TERMOX_TERMINAL_DYNAMIC_COLOR_ENGINE_HPP
#include <algorithm>
#include <vector>

#include <termox/common/lockable.hpp>
#include <termox/painter/color.hpp>
#include <termox/system/detail/interval_event_loop.hpp>

namespace ox {

/// Event loop that manages posting of Dynamic_color_events.
class Dynamic_color_engine : public detail::Interval_event_loop,
                             private Lockable<std::mutex> {
   public:
    using Interval_event_loop::Clock_t;
    using Interval_event_loop::Interval_t;
    using Interval_event_loop::Time_point;

    struct Registered_data {
        Color color;
        Dynamic_color dynamic;
        Time_point last_event_time;
    };

    static auto constexpr default_interval = Interval_t{100};

   public:
    Dynamic_color_engine() : Interval_event_loop{default_interval} {}

    ~Dynamic_color_engine()
    {
        this->Interval_event_loop::exit(0);
        this->Interval_event_loop::wait();
    }

   public:
    /// Add a dynamic color linked to \p color.
    /** Does not check for duplicates. */
    void register_color(Color color, Dynamic_color const& dynamic)
    {
        auto const lock = this->Lockable::lock();
        data_.push_back({color, dynamic, Clock_t::now()});
    }

    /// Removes the Dynamic_color linked to \p color.
    /** Function does nothing if Color is not found. */
    void unregister_color(Color color)
    {
        auto const lock = this->Lockable::lock();
        auto const iter = std::find_if(
            std::cbegin(data_), std::cend(data_),
            [color](auto const& data) { return data.color == color; });
        if (iter != std::cend(data_))
            data_.erase(iter);
    }

    /// Remove all registered colors.
    void clear()
    {
        auto const lock = this->Lockable::lock();
        data_.clear();
    }

    /// Return true if there are no registered widgets
    [[nodiscard]] auto is_empty() const -> bool;

   private:
    std::vector<Registered_data> data_;

   private:
    void loop_function() override;

    void post_dynamic_color_events();
};

}  // namespace ox
#endif  // TERMOX_TERMINAL_DYNAMIC_COLOR_ENGINE_HPP
