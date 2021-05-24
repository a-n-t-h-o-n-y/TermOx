#include "snake.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <iterator>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <termox/termox.hpp>

namespace {
using namespace snake;

/// Return random value between [0, max].
[[nodiscard]] auto random_index(std::size_t max) -> std::size_t
{
    static auto gen = std::mt19937{std::random_device{}()};
    auto dist       = std::uniform_int_distribution<std::size_t>{0, max};
    return dist(gen);
}

/// Return true if \p p is found within \p r.
[[nodiscard]] auto contain(Snake const& r, ox::Point p) -> bool
{
    return std::find(std::cbegin(r), std::cend(r), p) != std::cend(r);
}

/// Return true if the given Directions are at 180 degrees from each other.
[[nodiscard]] auto is_180_degrees(Snake::Direction a, Snake::Direction b)
    -> bool
{
    switch (a) {
        using Direction = Snake::Direction;
        case Direction::Left: return b == Direction::Right;
        case Direction::Right: return b == Direction::Left;
        case Direction::Up: return b == Direction::Down;
        case Direction::Down: return b == Direction::Up;
    }
    return false;
}

/// Returns a pair of 0, -1, or +1, cooresponding to the direction \p d.
[[nodiscard]] auto to_units(Snake::Direction d) -> std::pair<short, short>
{
    switch (d) {
        using Direction = Snake::Direction;
        case Direction::Left: return {-1, 0};
        case Direction::Right: return {1, 0};
        case Direction::Up: return {0, -1};
        case Direction::Down: return {0, 1};
    }
    return {0, 0};
}

/// Return the current state of the game.
[[nodiscard]] auto get_state(Snake const& snake, Apple_field const& apples)
    -> Engine::State
{
    using State      = Engine::State;
    auto const& head = snake.head();
    if (head.x >= apples.area().width || head.y >= apples.area().height ||
        head.x < 0 || head.y < 0) {
        return State::Dead;
    }
    if (std::count(std::cbegin(snake), std::cend(snake), head) > 1)
        return State::Dead;
    if (apples.contains(head))
        return State::Apple;
    return State::No_apple;
}

/// Reduce \p duration by \p factor and return the result as ms.
[[nodiscard]] auto reduce_by(std::chrono::milliseconds duration, double factor)
    -> std::chrono::milliseconds
{
    return duration - std::chrono::duration_cast<std::chrono::milliseconds>(
                          duration * factor);
}

[[nodiscard]] auto make_instruction_text() -> ox::Glyph_string
{
    using namespace ox;
    auto const standout = Brush{fg(color::Instruction_text), Trait::Bold};
    auto result         = Glyph_string{U"Start/Stop "};
    result.append(U"Space Bar" | standout);
    result.append(U" - Movement ");
    result.append(U"Arrow Keys" | standout);
    result.append(U" or ");
    result.append(U"'hjkl'" | standout);
    return result;
}

}  // namespace

namespace snake {

void Apple_field::initialize(ox::Area a, Snake const& mask)
{
    this->clear();
    this->resize(a);
    auto constexpr coverage = .004;
    auto const total        = a.width * a.height * coverage;
    for (auto i = 0; i < total; ++i)
        this->create_apple(mask);
}

void Apple_field::create_apple(Snake const& mask)
{
    if (area_.width == 0 || area_.height == 0)
        return;
    if (points_.size() == ((area_.width * area_.height) - mask.size()))
        return;
    auto p = ox::Point{0, 0};
    do {
        p.x = ::random_index(area_.width - 1);
        p.y = ::random_index(area_.height - 1);
    } while (points_.count(p) > 0 && !contain(mask, p));
    points_.insert(p);
}

void Apple_field::remove(ox::Point p) { points_.erase(p); }

auto Apple_field::contains(ox::Point p) const -> bool
{
    return points_.count(p) > 0;
}

auto Apple_field::begin() const -> std::set<ox::Point>::const_iterator
{
    return std::cbegin(points_);
}

auto Apple_field::end() const -> std::set<ox::Point>::const_iterator
{
    return std::cend(points_);
}

auto Apple_field::size() const -> std::size_t { return points_.size(); }

auto Apple_field::area() const -> ox::Area { return area_; }

void Apple_field::resize(ox::Area a)
{
    area_ = a;
    for (auto point = std::begin(points_); point != std::end(points_);) {
        if (point->x >= area_.width || point->y >= area_.height)
            point = points_.erase(point);
        else
            std::advance(point, 1);
    }
}

void Apple_field::clear() { points_.clear(); }

void Snake::initialize(ox::Point p)
{
    points_.clear();
    points_.push_back(p);
    direction_      = Direction::Right;
    next_direction_ = Direction::Right;
}

void Snake::enable_input() { has_input_ = true; }

void Snake::disable_input() { has_input_ = false; }

void Snake::set_direction(Direction d)
{
    if (has_input_ && !::is_180_degrees(d, direction_))
        next_direction_ = d;
}

auto Snake::get_direction() const -> Direction { return direction_; }

void Snake::increment_head()
{
    direction_       = next_direction_;
    auto const units = ::to_units(direction_);
    points_.push_back(
        {points_.back().x + units.first, points_.back().y + units.second});
}

void Snake::decrement_tail()
{
    if (points_.empty())
        return;
    points_.erase(std::begin(points_));
}

auto Snake::head() const -> ox::Point { return points_.back(); }

auto Snake::begin() const -> std::vector<ox::Point>::const_iterator
{
    return std::cbegin(points_);
}

auto Snake::end() const -> std::vector<ox::Point>::const_iterator
{
    return std::cend(points_);
}

auto Snake::size() const -> std::size_t { return points_.size(); }

auto Snake::operator[](std::size_t i) const -> ox::Point { return points_[i]; }

void Engine::reset(ox::Area a)
{
    score_ = 0;
    score(score_);
    step_ = 0;
    snake.initialize({a.width / 2, a.height / 2});
    apples.initialize(a, snake);
}

void Engine::increment()
{
    ++step_;
    snake.increment_head();
    switch (::get_state(snake, apples)) {
        case State::Dead: game_over(); return;
        case State::Apple:
            ++score_;
            score(score_);
            apples.remove(snake.head());
            break;
        case State::No_apple: snake.decrement_tail(); break;
    }
    if (step_ % add_apple_at_ == 0)
        apples.create_apple(snake);
}

auto Engine::is_game_over() const -> bool
{
    return ::get_state(snake, apples) == State::Dead;
}

auto Engine::get_score() const -> unsigned { return score_; }

Game_space::Game_space()
{
    using namespace ox::pipe;
    *this | strong_focus();
    game_over.connect([this] { this->stop(); });
    score.connect([this](unsigned score) {
        auto constexpr limit    = std::chrono::milliseconds{40};
        auto constexpr interval = 7;
        if ((score % interval == 0) && period_ > limit)
            this->set_speed(::reduce_by(period_, .05));
    });
}

void Game_space::start()
{
    if (too_small_) {
        this->stop();  // Emit signals to reset toggle button display.
        return;
    }
    if (engine_.is_game_over())
        this->reset();
    using namespace ox::pipe;
    engine_.snake.enable_input();
    *this | animate(period_);
    started();
}

void Game_space::stop()
{
    using namespace ox::pipe;
    engine_.snake.disable_input();
    *this | disanimate();
    stopped();
}

void Game_space::toggle()
{
    if (this->is_animated())
        this->stop();
    else
        this->start();
}

void Game_space::resize(ox::Area a)
{
    using namespace ox::pipe;
    *this | fixed_width(a.width) | fixed_height(a.height);
    this->stop();
    engine_.reset(a);
    this->set_speed(initial_period_);
    too_small_ = this->is_too_small();
    this->update();
}

void Game_space::reset()
{
    this->stop();
    engine_.reset(engine_.apples.area());
    this->update();
    this->set_speed(initial_period_);
}

void Game_space::set_speed(std::chrono::milliseconds period)
{
    using namespace ox::pipe;
    if (this->is_animated())
        *this | disanimate() | animate(period);
    period_ = period;
}

auto Game_space::paint_event(ox::Painter& p) -> bool
{
    if (too_small_)
        this->paint_size_message(p);
    else
        this->paint_game(p);
    return Widget::paint_event(p);
}

auto Game_space::key_press_event(ox::Key k) -> bool
{
    switch (k) {
        using namespace ox;
        case Key::Space: this->toggle(); break;
        case Key::Arrow_left:
        case Key::h: engine_.snake.set_direction(Snake::Direction::Left); break;
        case Key::Arrow_right:
        case Key::l:
            engine_.snake.set_direction(Snake::Direction::Right);
            break;
        case Key::Arrow_up:
        case Key::k: engine_.snake.set_direction(Snake::Direction::Up); break;
        case Key::Arrow_down:
        case Key::j: engine_.snake.set_direction(Snake::Direction::Down); break;
        default: break;
    }
    return Widget::key_press_event(k);
}

auto Game_space::timer_event() -> bool
{
    engine_.increment();
    this->update();
    return Widget::timer_event();
}

auto Game_space::resize_event(ox::Area new_size, ox::Area old_size) -> bool
{
    too_small_ = this->is_too_small();
    if (too_small_)
        this->stop();
    return Widget::resize_event(new_size, old_size);
}

auto Game_space::is_too_small() -> bool
{
    return this->width() < engine_.apples.area().width ||
           this->height() < engine_.apples.area().height;
}

void Game_space::paint_size_message(ox::Painter& painter)
{
    auto const w = [this] {
        auto const& widg_width = this->width();
        auto const& game_width = engine_.apples.area().width;
        return widg_width < game_width ? game_width - widg_width : 0;
    }();
    auto const h = [this] {
        auto const& widg_height = this->height();
        auto const& game_height = engine_.apples.area().height;
        return widg_height < game_height ? game_height - widg_height : 0;
    }();
    painter.put(U"Screen is too small!", {0, 0});
    painter.put("Needs " + std::to_string(w) + " more width.", {0, 1});
    painter.put("Needs " + std::to_string(h) + " more height.", {0, 2});
}

void Game_space::paint_game(ox::Painter& painter)
{
    for (auto const& p : engine_.apples)
        painter.put(apple, p);

    for (auto const& p : engine_.snake)
        painter.put(snake_body, p);

    if (engine_.snake.size() > 3) {
        painter.put(tail[0], engine_.snake[0]);
        painter.put(tail[1], engine_.snake[1]);
        painter.put(tail[2], engine_.snake[2]);
    }
    else if (engine_.snake.size() == 3) {
        painter.put(tail[1], engine_.snake[0]);
        painter.put(tail[2], engine_.snake[1]);
    }
    else if (engine_.snake.size() == 2)
        painter.put(tail[2], engine_.snake[0]);

    // Head
    switch (engine_.snake.get_direction()) {
        case Snake::Direction::Up:
            painter.put(U'🭯' | fg(color::Snake), engine_.snake.head());
            break;
        case Snake::Direction::Down:
            painter.put(U'🭭' | fg(color::Snake), engine_.snake.head());
            break;
        case Snake::Direction::Left:
            painter.put(U'🭮' | fg(color::Snake), engine_.snake.head());
            break;
        case Snake::Direction::Right:
            painter.put(U'🭬' | fg(color::Snake), engine_.snake.head());
            break;
    }
}

Instructions::Instructions(Parameters) : Text_view{::make_instruction_text()}
{
    using namespace ox::pipe;
    *this | bg(color::Instruction_bg) | fg(color::Instruction_fg) | any_wrap() |
        align_center();
}

Button_bar::Button_bar()
    : ox::HTuple<ox::Toggle_button, ox::Labeled_cycle_box, Instructions>{
          {U"Start" | ox::Trait::Bold, U"Pause" | ox::Trait::Bold},
          {U" Size"},
          {}}
{
    using namespace ox::pipe;
    sizes_ | fixed_width(16);

    sizes_.label | bg(color::Size_bg) | fg(color::Size_fg);
    sizes_.div | bg(color::Size_bg) | fg(color::Size_fg);
    sizes_.cycle_box | bg(color::Size_bg) | fg(color::Size_fg);

    sizes_.cycle_box | no_focus();
    sizes_.cycle_box.add_option(U"Small").connect([this] { size_change('s'); });
    sizes_.cycle_box.add_option(U"Medium").connect(
        [this] { size_change('m'); });
    sizes_.cycle_box.add_option(U"Large").connect([this] { size_change('l'); });
    sizes_.cycle_box.add_option(U"X-Large").connect([this] {
        size_change('x');
    });

    start_pause_btns_ | fixed_width(15);
    start_pause_btns_.top | bg(color::Start_bg);
    start_pause_btns_.bottom | bg(color::Pause_bg);
}

void Button_bar::show_start_button() { start_pause_btns_.show_top(); }

void Button_bar::show_pause_button() { start_pause_btns_.show_bottom(); }

Score::Score()
    : ox::HPair<ox::HLabel, ox::Int_view>{{U"Score: "},
                                          {0, 0, ox::Align::Right}}
{
    using namespace ox::pipe;
    *this | fixed_width(12) | children() | bg(color::Score_bg) |
        fg(color::Score_fg);
}

void Score::set(unsigned score) { score_.set_value(score); }

Snake_game::Snake_game()
{
    using namespace ox;
    using namespace ox::pipe;
    *this | direct_focus();

    bottom_ | fixed_height(1);
    bottom_.buttons.start.connect([this] { game_space_.start(); });
    bottom_.buttons.pause.connect([this] { game_space_.stop(); });

    auto constexpr s_size = ox::Area{60, 17};
    auto constexpr m_size = ox::Area{100, 27};
    auto constexpr l_size = ox::Area{160, 37};
    auto constexpr x_size = ox::Area{230, 47};

    bottom_.buttons.size_change.connect([=](char c) {
        switch (c) {
            case 's': game_space_.resize(s_size); break;
            case 'm': game_space_.resize(m_size); break;
            case 'l': game_space_.resize(l_size); break;
            case 'x': game_space_.resize(x_size); break;
        }
    });

    floating_game.buffer_1 | bg(color::Border);
    floating_game.buffer_2 | bg(color::Border);
    floating_game.widget.buffer_1 | bg(color::Border);
    floating_game.widget.buffer_2 | bg(color::Border);

    game_space_.resize(s_size);
    game_space_.score.connect(
        [this](unsigned score) { bottom_.score.set(score); });
    game_space_.started.connect(
        [this] { bottom_.buttons.show_pause_button(); });
    game_space_.stopped.connect(
        [this] { bottom_.buttons.show_start_button(); });
}

auto Snake_game::focus_in_event() -> bool
{
    ox::Terminal::set_palette(snake_palette);
    ox::System::set_focus(game_space_);
    return ox::layout::Vertical<>::focus_in_event();
}

}  // namespace snake
