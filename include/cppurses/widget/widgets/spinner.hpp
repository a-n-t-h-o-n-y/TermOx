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
    explicit Spinner(Glyph_string frames,
                     Period_t period    = Period_t{100},
                     std::size_t width  = 1,
                     std::size_t offset = 0)
        : frames_{std::move(frames)},
          period_{period},
          width_{width},
          index_{offset}
    {
        height_policy.fixed(1);
        width_policy.fixed(width_);
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
        this->update();
    }

   protected:
    auto paint_event() -> bool override
    {
        if (started_)
            for (auto x = 0uL; x < width_; ++x) {
                Painter{*this}.put(frames_[index_], x, 0);
            }
        return Widget::paint_event();
    }

    auto timer_event() -> bool override
    {
        ++index_;
        if (index_ >= frames_.size())
            index_ = 0;
        this->update();
        return Widget::timer_event();
    }

   private:
    bool started_ = false;
    Glyph_string frames_;
    Period_t period_;
    std::size_t width_;
    std::size_t index_;
};

/// Specific Spinners
struct Spinner_cycle : Spinner {
    Spinner_cycle(Period_t period    = Period_t{100},
                  std::size_t width  = 1uL,
                  std::size_t offset = 0uL)
        : Spinner{"⠁⠈⠐⠠⢀⡀⠄⠂", period, width, offset}
    {}
};

struct Spinner_cycle_ccw : Spinner {
    Spinner_cycle_ccw(Period_t period    = Period_t{100},
                      std::size_t width  = 1uL,
                      std::size_t offset = 0uL)
        : Spinner{"⠁⠂⠄⡀⢀⠠⠐⠈", period, width, offset}
    {}
};

struct Spinner_fall : Spinner {
    Spinner_fall(Period_t period    = Period_t{100},
                 std::size_t width  = 1uL,
                 std::size_t offset = 0uL)
        : Spinner{"⠁⠂⠄⡀⡈⡐⡠⣀⣁⣂⣄⣌⣔⣤⣥⣦⣮⣶⣷⣿", period, width, offset}
    {}
};

struct Spinner_fall_two : Spinner {
    Spinner_fall_two(Period_t period    = Period_t{100},
                     std::size_t width  = 1uL,
                     std::size_t offset = 0uL)
        : Spinner{" ⠁⠉⠋⠛⠟⠿⡿⣿⣿⣿⣿⣷⣶⣦⣤⣄⣀⡀ ", period, width, offset}
    {}
};

struct Spinner_fall_three : Spinner {
    Spinner_fall_three(Period_t period    = Period_t{100},
                       std::size_t width  = 1uL,
                       std::size_t offset = 0uL)
        : Spinner{"   ⠁⠂⠄⡀⡈⡐⡠⣀⣁⣂⣄⣌⣔⣤⣥⣦⣮⣶⣷⣿⣿⣿⣿⣿⣿⡿⠿⢟⠟⡛⠛⠫⢋⠋⠍⡉⠉⠑⠡⢁⠁⠂⠄⡀   ", period,
                  width, offset}
    {}
};

struct Spinner_rise : Spinner {
    Spinner_rise(Period_t period    = Period_t{100},
                 std::size_t width  = 1uL,
                 std::size_t offset = 0uL)
        : Spinner{"⡀⠄⠂⠁⢁⠡⠑⠉⡉⠍⠋⢋⠫⠛⡛⠟⢟⠿⡿⣿", period, width, offset}
    {}
};

struct Spinner_rise_two : Spinner {
    Spinner_rise_two(Period_t period    = Period_t{100},
                     std::size_t width  = 1uL,
                     std::size_t offset = 0uL)
        : Spinner{"    ⡀⣀⣄⣤⣦⣶⣷⣿⣿⣿⣿⡿⠿⠟⠛⠋⠉⠁    ", period, width, offset}
    {}
};

struct Spinner_rise_three : Spinner {
    Spinner_rise_three(Period_t period    = Period_t{100},
                       std::size_t width  = 1uL,
                       std::size_t offset = 0uL)
        : Spinner{"   ⡀⠄⠂⠁⢁⠡⠑⠉⡉⠍⠋⢋⠫⠛⡛⠟⢟⠿⡿⣿⣿⣿⣿⣿⣿⣷⣶⣮⣦⣥⣤⣔⣌⣄⣂⣁⣀⡠⡐⡈⡀⠄⠂⠁   ", period,
                  width, offset}
    {}
};

struct Spinner_fill : Spinner {
    Spinner_fill(Period_t period    = Period_t{100},
                 std::size_t width  = 1uL,
                 std::size_t offset = 0uL)
        : Spinner{"⡀⣀⣄⣤⣦⣶⣷⣿", period, width, offset}
    {}
};

struct Spinner_top_fill : Spinner {
    Spinner_top_fill(Period_t period    = Period_t{100},
                     std::size_t width  = 1uL,
                     std::size_t offset = 0uL)
        : Spinner{"⠁⠉⠋⠛⠟⠿⡿⣿", period, width, offset}
    {}
};

struct Spinner_tail : Spinner {
    Spinner_tail(Period_t period    = Period_t{100},
                 std::size_t width  = 1uL,
                 std::size_t offset = 0uL)
        : Spinner{"⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏", period, width, offset}
    {}
};

struct Spinner_switch : Spinner {
    Spinner_switch(Period_t period    = Period_t{100},
                   std::size_t width  = 1uL,
                   std::size_t offset = 0uL)
        : Spinner{"⢹⢺⢼⣸⣇⡧⡗⡏", period, width, offset}
    {}
};

struct Spinner_chase : Spinner {
    Spinner_chase(Period_t period    = Period_t{100},
                  std::size_t width  = 1uL,
                  std::size_t offset = 0uL)
        : Spinner{"⢄⢂⢁⡁⡈⡐⡠", period, width, offset}
    {}
};

struct Spinner_line : Spinner {
    Spinner_line(Period_t period    = Period_t{100},
                 std::size_t width  = 1uL,
                 std::size_t offset = 0uL)
        : Spinner{"-\\|/", period, width, offset}
    {}
};

struct Spinner_block_cycle : Spinner {
    Spinner_block_cycle(Period_t period    = Period_t{100},
                        std::size_t width  = 1uL,
                        std::size_t offset = 0uL)
        : Spinner{"▖▘▝▗", period, width, offset}
    {}
};

struct Spinner_fade : Spinner {
    Spinner_fade(Period_t period    = Period_t{100},
                 std::size_t width  = 1uL,
                 std::size_t offset = 0uL)
        : Spinner{"         ░░▒▓▓██████████████████▓▓▒░░         ", period,
                  width, offset}
    {}
};

/// Best used with multiple spinners horizontally, each offset one from neighbor
struct Spinner_fade_trail : Spinner {
    Spinner_fade_trail(Period_t period    = Period_t{100},
                       std::size_t width  = 1uL,
                       std::size_t offset = 0uL)
        : Spinner{"⢀⢀⠠⠠⢀⢀⠠⠠⢀░░▒▓▓██████████████████▓▓▒░░⢀⢀⠠⠠⢀⢀⠠⠠⢀", period,
                  width, offset}
    {}
};

struct Spinner_quarter_circles : Spinner {
    Spinner_quarter_circles(Period_t period    = Period_t{100},
                            std::size_t width  = 1uL,
                            std::size_t offset = 0uL)
        : Spinner{"◜◝◞◟", period, width, offset}
    {}
};

struct Spinner_triangles : Spinner {
    Spinner_triangles(Period_t period    = Period_t{100},
                      std::size_t width  = 1uL,
                      std::size_t offset = 0uL)
        : Spinner{"◤◥◢◣", period, width, offset}
    {}
};

struct Spinner_empty_triangles : Spinner {
    Spinner_empty_triangles(Period_t period    = Period_t{100},
                            std::size_t width  = 1uL,
                            std::size_t offset = 0uL)
        : Spinner{"◸◹◿◺", period, width, offset}
    {}
};

struct Spinner_clock : Spinner {
    Spinner_clock(Period_t period    = Period_t{100},
                  std::size_t width  = 1uL,
                  std::size_t offset = 0uL)
        : Spinner{"◴◷◶◵", period, width, offset}
    {}
};

struct Spinner_box : Spinner {
    Spinner_box(Period_t period    = Period_t{100},
                std::size_t width  = 1uL,
                std::size_t offset = 0uL)
        : Spinner{"▤▧▥▨", period, width, offset}
    {}
};

struct Spinner_cross : Spinner {
    Spinner_cross(Period_t period    = Period_t{100},
                  std::size_t width  = 1uL,
                  std::size_t offset = 0uL)
        : Spinner{"┽╀┾╁", period, width, offset}
    {}
};

struct Spinner_vertical_pass : Spinner {
   public:
    Spinner_vertical_pass(Period_t period    = Period_t{100},
                          std::size_t width  = 1uL,
                          std::size_t offset = 0uL)
        : Spinner{first() + second() + " ", period, width, offset}
    {}

   private:
    auto first() -> Glyph_string { return "▁▂▃▄▅▆▇█"; }
    auto second() -> Glyph_string
    {
        auto result = first();
        result.add_traits(Trait::Inverse);
        return result;
    }
};

struct Spinner_horizontal_pass : Spinner {
   public:
    Spinner_horizontal_pass(Period_t period    = Period_t{100},
                            std::size_t width  = 1uL,
                            std::size_t offset = 0uL)
        : Spinner{first() + second() + " ", period, width, offset}
    {}

   private:
    auto first() -> Glyph_string { return "▏▎▍▌▋▊▉█"; }
    auto second() -> Glyph_string
    {
        auto result = first();
        result.add_traits(Trait::Inverse);
        return result;
    }
};

struct Spinner_bump : Spinner {
    Spinner_bump(Period_t period    = Period_t{100},
                 std::size_t width  = 1uL,
                 std::size_t offset = 0uL)
        : Spinner{" ▁▂▃▄▅▆▇█▇▆▅▄▃▂▁", period, width, offset}
    {}
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_SPINNER_HPP
