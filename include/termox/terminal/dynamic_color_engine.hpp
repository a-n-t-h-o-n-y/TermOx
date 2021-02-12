#ifndef TERMOX_TERMINAL_DYNAMIC_COLOR_ENGINE_HPP
#define TERMOX_TERMINAL_DYNAMIC_COLOR_ENGINE_HPP
#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

#include <termox/painter/color.hpp>
#include <termox/system/detail/interval_event_loop.hpp>

namespace ox::detail {

class Dynamic_color_event_loop : public detail::Interval_event_loop {
   private:
    using Mutex_t = std::mutex;
    using Guard_t = std::scoped_lock<Mutex_t>;

   public:
    using Interval_event_loop::Interval_event_loop;

   public:
    /// Register a new Color to be dynamic, will replace if already exists.
    void register_color(Color color, Dynamic_color const& dynamic)
    {
        auto const guard = Guard_t{colors_mtx_};
        if (this->has_color(color))
            this->get_def_at(color).dynamic = dynamic;
        else
            colors_.emplace_back(Def{color, dynamic});
    }

    auto unregister_color(Color color) -> bool
    {
        auto const guard = Guard_t{colors_mtx_};
        if (auto iter = this->get_iter_for(color); iter != std::end(colors_)) {
            colors_.erase(iter);
            return true;
        }
        return false;
    }

    /// Remove all colors.
    void clear()
    {
        auto const guard = Guard_t{colors_mtx_};
        colors_.clear();
    }

    auto is_empty() const -> bool
    {
        auto const guard = Guard_t{colors_mtx_};
        return colors_.empty();
    }

   protected:
    void loop_function() override;

   public:
    struct Def {
        Color color;
        Dynamic_color dynamic;
    };

   private:
    std::vector<Def> colors_;  // Shared between multiple threads.
    mutable Mutex_t colors_mtx_;

   private:
    /// Return iterator for \p color from colors_; end(colors_) if not found.
    auto get_iter_for(Color color) const -> std::vector<Def>::const_iterator
    {
        // No mtx because only used internally from places with locked mtx
        return std::find_if(
            std::begin(colors_), std::end(colors_),
            [color](auto const& def) { return def.color == color; });
    }

    /// Check if colors_ contains a Def for \p color.
    auto has_color(Color color) const -> bool
    {
        return this->get_iter_for(color) != std::end(colors_);
    }

    /// Return the Def object for \p color. Assumes it exists, otherwise UB.
    auto get_def_at(Color color) -> Def&
    {
        return const_cast<Def&>(
            *static_cast<Dynamic_color_event_loop const&>(*this).get_iter_for(
                color));
    }
};

}  // namespace ox::detail

namespace ox {

class Dynamic_color_engine {
   public:
    using Period_t = detail::Dynamic_color_event_loop::Period_t;

   public:
    ~Dynamic_color_engine()
    {
        for (auto& loop : loops_) {
            loop->exit(0);
            loop->wait();
        }
    }

   public:
    /// Add a dynamic color linked to \p color.
    void register_color(Color color, Dynamic_color const& dynamic)
    {
        if (!this->has_loop_with(dynamic.interval)) {
            loops_.emplace_back(
                std::make_unique<detail::Dynamic_color_event_loop>(
                    dynamic.interval));
            loops_.back()->run_async();
        }
        this->get_loop_with(dynamic.interval).register_color(color, dynamic);
    }

    /// Removes the Dynamic_color linked to \p color.
    void unregister_color(Color color)
    {
        // Unregister the color from the event loop it is contained in, and
        // return that event loop. unregister_color returns bool if removed.
        auto const iter = std::find_if(std::begin(loops_), std::end(loops_),
                                       [color](auto const& loop) {
                                           return loop->unregister_color(color);
                                       });

        // If an event loop was found, and it is now empty, destroy that loop.
        if (iter != std::end(loops_)) {
            if (auto& loop = *iter; loop->is_empty()) {
                loop->exit(0);
                loop->wait();
                loops_.erase(iter);
            }
        }
    }

    /// Remove all registered colors.
    void clear()
    {
        this->shutdown();
        loops_.clear();
    }

    /// Start a thread for each framerate.
    void startup()
    {
        for (auto& loop : loops_)
            loop->run_async();
    }

    /// Stop all threads.
    void shutdown()
    {
        for (auto& loop : loops_) {
            loop->exit(0);
            loop->wait();
        }
    }

   private:
    using Loop_t = std::unique_ptr<detail::Dynamic_color_event_loop>;
    std::vector<Loop_t> loops_;

   private:
    /// Find and return iterator pointing to Event Loop with \p interval.
    /** Returns std::end(loops_) if no loop found with \p interval. */
    auto get_loop_iter_with(Period_t interval) const
    {
        return std::find_if(std::begin(loops_), std::end(loops_),
                            [interval](auto const& loop) {
                                return loop->get_interval() == interval;
                            });
    }

    /// Return true if there is already an Event_loop for \p interval.
    auto has_loop_with(Period_t interval) const -> bool
    {
        return this->get_loop_iter_with(interval) != std::end(loops_);
    }

    /// Assumes that the Event Loop does exist, otherwise undefined behavior.
    auto get_loop_with(Period_t interval) -> detail::Dynamic_color_event_loop&
    {
        return **this->get_loop_iter_with(interval);
    }
};

}  // namespace ox
#endif  // TERMOX_TERMINAL_DYNAMIC_COLOR_ENGINE_HPP
