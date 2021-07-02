#include <termox/widget/widgets/spinner.hpp>

#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/painter/trait.hpp>
#include <termox/widget/pipe.hpp>

namespace ox {

Spinner::Spinner(Glyph_string frames, Duration_t period, int width, int offset)
    : frames_{std::move(frames)}, period_{period}, width_{width}, index_{offset}
{
    *this | pipe::fixed_height(1) | pipe::fixed_width(width_);
}

Spinner::Spinner(Parameters p)
    : Spinner{std::move(p.frames), p.period, p.width, p.offset}
{}

void Spinner::set_frames(Glyph_string frames) { frames_ = std::move(frames); }

auto Spinner::frames() const noexcept -> Glyph_string const& { return frames_; }

void Spinner::set_period(Duration_t period)
{
    if (started_) {
        this->stop();
        period_ = period;
        this->start();
    }
    else
        period_ = period;
}

auto Spinner::period() const noexcept -> Duration_t { return period_; }

void Spinner::set_width(int width)
{
    width_       = width;
    width_policy = Size_policy::fixed(width_);
}

auto Spinner::spinner_width() const noexcept -> int { return width_; }

void Spinner::set_offset(int offset) { index_ = offset; }

void Spinner::start()
{
    if (started_)
        return;
    this->enable_animation(period_);
    started_ = true;
}

void Spinner::stop()
{
    this->disable_animation();
    started_ = false;
    this->update();
}

auto Spinner::paint_event(Painter& p) -> bool
{
    if (started_) {
        for (auto x = 0; x < width_; ++x)
            p.put(frames_[index_], {x, 0});
    }
    return Widget::paint_event(p);
}

auto Spinner::timer_event() -> bool
{
    ++index_;
    if (index_ >= frames_.size())
        index_ = 0;
    this->update();
    return Widget::timer_event();
}

auto spinner(Glyph_string frames,
             Spinner::Duration_t period,
             int width,
             int offset) -> std::unique_ptr<Spinner>
{
    return std::make_unique<Spinner>(std::move(frames), period, width, offset);
}

auto spinner(Spinner::Parameters p) -> std::unique_ptr<Spinner>
{
    return std::make_unique<Spinner>(std::move(p));
}

Spinner_cycle::Spinner_cycle(Duration_t period, int width, int offset)
    : Spinner{U"⠁⠈⠐⠠⢀⡀⠄⠂", period, width, offset}
{}

Spinner_cycle::Spinner_cycle(Parameters p)
    : Spinner_cycle{p.period, p.width, p.offset}
{}

auto spinner_cycle(Spinner::Duration_t period, int width, int offset)
    -> std::unique_ptr<Spinner_cycle>
{
    return std::make_unique<Spinner_cycle>(period, width, offset);
}

auto spinner_cycle(Spinner_cycle::Parameters p)
    -> std::unique_ptr<Spinner_cycle>
{
    return std::make_unique<Spinner_cycle>(std::move(p));
}

Spinner_cycle_ccw::Spinner_cycle_ccw(Duration_t period, int width, int offset)
    : Spinner{U"⠁⠂⠄⡀⢀⠠⠐⠈", period, width, offset}
{}

Spinner_fall::Spinner_fall(Duration_t period, int width, int offset)
    : Spinner{U"⠁⠂⠄⡀⡈⡐⡠⣀⣁⣂⣄⣌⣔⣤⣥⣦⣮⣶⣷⣿", period, width, offset}
{}

Spinner_fall_two::Spinner_fall_two(Duration_t period, int width, int offset)
    : Spinner{U" ⠁⠉⠋⠛⠟⠿⡿⣿⣿⣿⣿⣷⣶⣦⣤⣄⣀⡀ ", period, width, offset}
{}

Spinner_fall_three::Spinner_fall_three(Duration_t period, int width, int offset)
    : Spinner{U"   ⠁⠂⠄⡀⡈⡐⡠⣀⣁⣂⣄⣌⣔⣤⣥⣦⣮⣶⣷⣿⣿⣿⣿⣿⣿⡿⠿⢟⠟⡛⠛⠫⢋⠋⠍⡉⠉⠑⠡⢁⠁⠂⠄⡀   ", period,
              width, offset}
{}

Spinner_rise::Spinner_rise(Duration_t period, int width, int offset)
    : Spinner{U"⡀⠄⠂⠁⢁⠡⠑⠉⡉⠍⠋⢋⠫⠛⡛⠟⢟⠿⡿⣿", period, width, offset}
{}

Spinner_rise_two::Spinner_rise_two(Duration_t period, int width, int offset)
    : Spinner{U"    ⡀⣀⣄⣤⣦⣶⣷⣿⣿⣿⣿⡿⠿⠟⠛⠋⠉⠁    ", period, width, offset}
{}

Spinner_rise_three::Spinner_rise_three(Duration_t period, int width, int offset)
    : Spinner{U"   ⡀⠄⠂⠁⢁⠡⠑⠉⡉⠍⠋⢋⠫⠛⡛⠟⢟⠿⡿⣿⣿⣿⣿⣿⣿⣷⣶⣮⣦⣥⣤⣔⣌⣄⣂⣁⣀⡠⡐⡈⡀⠄⠂⠁   ", period,
              width, offset}
{}

Spinner_fill::Spinner_fill(Duration_t period, int width, int offset)
    : Spinner{U"⡀⣀⣄⣤⣦⣶⣷⣿", period, width, offset}
{}

Spinner_top_fill::Spinner_top_fill(Duration_t period, int width, int offset)
    : Spinner{U"⠁⠉⠋⠛⠟⠿⡿⣿", period, width, offset}
{}

Spinner_tail::Spinner_tail(Duration_t period, int width, int offset)
    : Spinner{U"⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏", period, width, offset}
{}

Spinner_switch::Spinner_switch(Duration_t period, int width, int offset)
    : Spinner{U"⢹⢺⢼⣸⣇⡧⡗⡏", period, width, offset}
{}

Spinner_chase::Spinner_chase(Duration_t period, int width, int offset)
    : Spinner{U"⢄⢂⢁⡁⡈⡐⡠", period, width, offset}
{}

Spinner_line::Spinner_line(Duration_t period, int width, int offset)
    : Spinner{U"-\\|/", period, width, offset}
{}

Spinner_block_cycle::Spinner_block_cycle(Duration_t period,
                                         int width,
                                         int offset)
    : Spinner{U"▖▘▝▗", period, width, offset}
{}

Spinner_fade::Spinner_fade(Duration_t period, int width, int offset)
    : Spinner{U"         ░░▒▓▓██████████████████▓▓▒░░         ", period, width,
              offset}
{}

Spinner_fade_trail::Spinner_fade_trail(Duration_t period, int width, int offset)
    : Spinner{U"⢀⢀⠠⠠⢀⢀⠠⠠⢀░░▒▓▓██████████████████▓▓▒░░⢀⢀⠠⠠⢀⢀⠠⠠⢀", period, width,
              offset}
{}

Spinner_quarter_circles::Spinner_quarter_circles(Duration_t period,
                                                 int width,
                                                 int offset)
    : Spinner{U"◜◝◞◟", period, width, offset}
{}

Spinner_triangles::Spinner_triangles(Duration_t period, int width, int offset)
    : Spinner{U"◤◥◢◣", period, width, offset}
{}

Spinner_empty_triangles::Spinner_empty_triangles(Duration_t period,
                                                 int width,
                                                 int offset)
    : Spinner{U"◸◹◿◺", period, width, offset}
{}

Spinner_clock::Spinner_clock(Duration_t period, int width, int offset)
    : Spinner{U"◴◷◶◵", period, width, offset}
{}

Spinner_box::Spinner_box(Duration_t period, int width, int offset)
    : Spinner{U"▤▧▥▨", period, width, offset}
{}

Spinner_cross::Spinner_cross(Duration_t period, int width, int offset)
    : Spinner{U"┽╀┾╁", period, width, offset}
{}

Spinner_vertical_pass::Spinner_vertical_pass(Duration_t period,
                                             int width,
                                             int offset)
    : Spinner{first().append(second()).append(U" "), period, width, offset}
{}

auto Spinner_vertical_pass::first() -> Glyph_string { return U"▁▂▃▄▅▆▇█"; }

auto Spinner_vertical_pass::second() -> Glyph_string
{
    auto result = first();
    result.add_traits(Trait::Inverse);
    return result;
}

Spinner_horizontal_pass::Spinner_horizontal_pass(Duration_t period,
                                                 int width,
                                                 int offset)
    : Spinner{first().append(second()).append(U" "), period, width, offset}
{}

auto Spinner_horizontal_pass::first() -> Glyph_string { return U"▏▎▍▌▋▊▉█"; }

auto Spinner_horizontal_pass::second() -> Glyph_string
{
    auto result = first();
    result.add_traits(Trait::Inverse);
    return result;
}

Spinner_bump::Spinner_bump(Duration_t period, int width, int offset)
    : Spinner{U" ▁▂▃▄▅▆▇█▇▆▅▄▃▂▁", period, width, offset}
{}

}  // namespace ox
