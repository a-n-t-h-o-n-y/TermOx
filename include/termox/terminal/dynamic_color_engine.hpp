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
    /// Register a new ansi color to be dynamic, will replace if already exists.
    void register_color(ANSI ansi, Dynamic_color dynamic)
    {
        auto const guard = Guard_t{colors_mtx_};
        if (this->has_color(ansi))
            this->get_def_at(ansi).dynamic = dynamic;
        else
            colors_.emplace_back(Def{ansi, dynamic});
    }

    auto unregister_color(ANSI ansi) -> bool
    {
        auto const guard = Guard_t{colors_mtx_};
        if (auto iter = this->get_iter_for(ansi); iter != std::end(colors_)) {
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
        ANSI ansi;
        Dynamic_color dynamic;
    };

   private:
    std::vector<Def> colors_;  // Shared between multiple threads.
    mutable Mutex_t colors_mtx_;

   private:
    /// Return the iterator for \p ansi from colors_, end(colors_) if not found.
    auto get_iter_for(ANSI ansi) -> std::vector<Def>::iterator
    {
        // No mtx because only used internally from places with locked mtx
        return std::find_if(
            std::begin(colors_), std::end(colors_),
            [ansi](auto const& def) { return def.ansi == ansi; });
    }

    /// Check if colors_ contains a Def for \p ansi.
    auto has_color(ANSI ansi) -> bool
    {
        return this->get_iter_for(ansi) != std::end(colors_);
    }

    /// Return the Def object for \p ansi. Assumes it exists, otherwise UB.
    auto get_def_at(ANSI ansi) -> Def& { return *this->get_iter_for(ansi); }
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
    /// Add a dynamic color at \p interval.
    void register_color(ANSI ansi, Dynamic_color const& dynamic)
    {
        if (!this->has_loop_with(dynamic.interval)) {
            loops_.emplace_back(
                std::make_unique<detail::Dynamic_color_event_loop>(
                    dynamic.interval));
            loops_.back()->run_async();
        }
        this->get_loop_with(dynamic.interval).register_color(ansi, dynamic);
    }

    /// Removes a dynamic color from the system.
    void unregister_color(ANSI ansi)
    {
        // Unregister the color from the event loop it is contained in, and
        // return that event loop.
        auto const iter = std::find_if(
            std::begin(loops_), std::end(loops_),
            [ansi](auto const& loop) { return loop->unregister_color(ansi); });

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
