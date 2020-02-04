#ifndef CPPURSES_WIDGET_WIDGET_SPINNER_HPP
#define CPPURSES_WIDGET_WIDGET_SPINNER_HPP
#include <cstddef>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Single cell animated spinner.
class Spinner : public Widget {
   public:
    using Period_t = Animation_engine::Period_t;

   public:
    /// Each glyph in frames is a frame, offset is starting index into frames.
    Spinner(Glyph_string frames,
            Period_t period    = Period_t{100},
            std::size_t offset = 0)
        : index_{offset}, period_{period}, frames_{std::move(frames)}
    {
        height_policy.fixed(1);
        width_policy.fixed(1);
        this->start();
    }

    void set_frames(Glyph_string frames) { frames_ = std::move(frames); }

    void set_period(Period_t period)
    {
        if (started_) {
            this->stop();
            period_ = period;
            this->start();
        }
        else
            period_ = period;
    }

    void start()
    {
        if (started_)
            return;
        this->enable_animation(period_);
        started_ = true;
    }

    void stop()
    {
        this->disable_animation();
        started_ = false;
    }

   protected:
    auto paint_event() -> bool override
    {
        if (index_ < frames_.size())
            Painter{*this}.put(frames_[index_], 0, 0);
        return Widget::paint_event();
    }

    auto timer_event() -> bool override
    {
        ++index_;
        if (index_ >= frames_.size())
            index_ = 0;
        return Widget::timer_event();
    }

   private:
    std::size_t index_ = 0;
    bool started_      = false;
    Period_t period_;
    Glyph_string frames_;
};

/// Specific Spinners
struct Spinner_cycle : Spinner {
    Spinner_cycle(Period_t period = Period_t{100}) : Spinner{"⠁⠈⠐⠠⢀⡀⠄⠂", period}
    {}
};

struct Spinner_cycle_ccw : Spinner {
    Spinner_cycle_ccw(Period_t period = Period_t{100})
        : Spinner{"⠁⠂⠄⡀⢀⠠⠐⠈", period}
    {}
};

struct Spinner_falling : Spinner {
    Spinner_falling(Period_t period = Period_t{100})
        : Spinner{"⠁⠂⠄⡀⡈⡐⡠⣀⣁⣂⣄⣌⣔⣤⣥⣦⣮⣶⣷⣿", period}
    {}
};

struct Spinner_rising : Spinner {
    Spinner_rising(Period_t period = Period_t{100})
        : Spinner{"⡀⠄⠂⠁⢁⠡⠑⠉⡉⠍⠋⢋⠫⠛⡛⠟⢟⠿⡿⣿", period}
    {}
};

struct Spinner_fill : Spinner {
    Spinner_fill(Period_t period = Period_t{100}) : Spinner{"⡀⣀⣄⣤⣦⣶⣷⣿", period}
    {}
};

struct Spinner_top_fill : Spinner {
    Spinner_top_fill(Period_t period = Period_t{100})
        : Spinner{"⠁⠉⠋⠛⠟⠿⡿⣿", period}
    {}
};

struct Spinner_tail : Spinner {
    Spinner_tail(Period_t period = Period_t{100})
        : Spinner{"⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏", period}
    {}
};

struct Spinner_switch : Spinner {
    Spinner_switch(Period_t period = Period_t{100})
        : Spinner{"⢹⢺⢼⣸⣇⡧⡗⡏", period}
    {}
};

struct Spinner_chase : Spinner {
    Spinner_chase(Period_t period = Period_t{100}) : Spinner{"⢄⢂⢁⡁⡈⡐⡠", period}
    {}
};

struct Spinner_line : Spinner {
    Spinner_line(Period_t period = Period_t{100}) : Spinner{"-\\|/", period} {}
};

struct Spinner_block_cycle : Spinner {
    Spinner_block_cycle(Period_t period = Period_t{100})
        : Spinner{"▖▘▝▗", period}
    {}
};

struct Spinner_fade : Spinner {
    Spinner_fade(Period_t period = Period_t{100}, std::size_t offset = 0)
        : Spinner{" ░▒▓██▓▒░ ", period, offset}
    {}
};

struct Spinner_quarter_circles : Spinner {
    Spinner_quarter_circles(Period_t period = Period_t{100})
        : Spinner{"◜◝◞◟", period}
    {}
};

struct Spinner_triangles : Spinner {
    Spinner_triangles(Period_t period = Period_t{100}) : Spinner{"◤◥◢◣", period}
    {}
};

struct Spinner_empty_triangles : Spinner {
    Spinner_empty_triangles(Period_t period = Period_t{100})
        : Spinner{"◸◹◿◺", period}
    {}
};

struct Spinner_clock : Spinner {
    Spinner_clock(Period_t period = Period_t{100}) : Spinner{"◴◷◶◵", period} {}
};

struct Spinner_box : Spinner {
    Spinner_box(Period_t period = Period_t{100}) : Spinner{"▤▧▥▨", period} {}
};

struct Spinner_cross : Spinner {
    Spinner_cross(Period_t period = Period_t{100}) : Spinner{"┽╀┾╁", period} {}
};

struct Spinner_vertical_pass : Spinner {
   public:
    Spinner_vertical_pass(Period_t period = Period_t{100})
        : Spinner{first() + second() + " ", period}
    {}

   private:
    auto first() -> Glyph_string { return "▁▂▃▄▅▆▇█"; }
    auto second() -> Glyph_string
    {
        auto result = first();
        result.add_attributes(Attribute::Inverse);
        return result;
    }
};

struct Spinner_horizontal_pass : Spinner {
   public:
    Spinner_horizontal_pass(Period_t period = Period_t{100})
        : Spinner{first() + second() + " ", period}
    {}

   private:
    auto first() -> Glyph_string { return "▏▎▍▌▋▊▉█"; }
    auto second() -> Glyph_string
    {
        auto result = first();
        result.add_attributes(Attribute::Inverse);
        return result;
    }
};

struct Spinner_bump : Spinner {
    Spinner_bump(Period_t period = Period_t{100})
        : Spinner{" ▁▂▃▄▅▆▇█▇▆▅▄▃▂▁", period}
    {}
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_SPINNER_HPP
