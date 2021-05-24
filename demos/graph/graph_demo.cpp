#include "graph_demo.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

#include <termox/painter/palette/apple_ii.hpp>
#include <termox/termox.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/toggle_button.hpp>

namespace {

/// Return true if \p zero is within \p threshold of zero.
[[nodiscard]] auto nearly_zero(double zero, double threshold) -> bool
{
    return std::abs(zero) <= threshold;
}

/// Function_t => auto f(double x, double y) -> double
/** If f(x, y) is nearly zero, then it is a point. */
template <typename Function_t>
[[nodiscard]] auto implicit_points(
    Function_t f,
    ox::Boundary<double> b,
    std::vector<ox::Graph<>::Coordinate>& container)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    auto constexpr h_coord_count = 320;
    auto constexpr v_coord_count = 240;
    auto const h_interval        = (b.east - b.west) / h_coord_count;
    auto const v_interval        = (b.north - b.south) / v_coord_count;
    container.clear();
    for (auto x = b.west; x <= b.east; x += h_interval) {
        for (auto y = b.south; y <= b.north; y += v_interval) {
            if (::nearly_zero(f(x, y), h_interval))
                container.push_back({x, y});
        }
    }
    return container;
}

}  // namespace

namespace graph {

auto Sine::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    auto constexpr coord_count = 10'000;
    auto const interval        = (b.east - b.west) / coord_count;
    coords_.clear();
    for (auto i = b.west; i <= b.east; i += interval)
        coords_.push_back({i, std::sin(i * t_)});
    return coords_;
}

void Sine::step_forward() { t_ += 0.01; }

auto Sine::default_boundary() const -> ox::Boundary<double>
{
    return {-3.14, 3.14, 1, -1};
}

// -----------------------------------------------------------------------------

auto Sine_three::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    auto constexpr coord_count = 10'000;
    auto const interval        = (b.east - b.west) / coord_count;
    coords_.clear();
    for (auto i = b.west; i <= b.east; i += interval) {
        coords_.push_back(
            {i, std::sin(i * 40) + std::sin(i * t_ * 0.5) + std::sin(i * t_)});
    }
    return coords_;
}

void Sine_three::step_forward() { t_ += 0.01; }

auto Sine_three::default_boundary() const -> ox::Boundary<double>
{
    return {-3.14, 3.14, 2, -2};
}

// -----------------------------------------------------------------------------

auto Sine_phase::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    auto constexpr coord_count = 10'000;
    auto const interval        = (b.east - b.west) / coord_count;
    static_sine_.clear();
    moving_sine_.clear();
    coords_.clear();
    for (auto i = b.west; i <= b.east; i += interval) {
        static_sine_.push_back({i, std::sin(i - (a_ * 0.25))});
        moving_sine_.push_back({i, std::sin(i + a_)});
        coords_.push_back({i, static_sine_.back().y + moving_sine_.back().y});
    }
    coords_.insert(std::end(coords_), std::begin(static_sine_),
                   std::end(static_sine_));
    coords_.insert(std::end(coords_), std::begin(moving_sine_),
                   std::end(moving_sine_));
    return coords_;
}

void Sine_phase::step_forward() { a_ += 0.01; }

auto Sine_phase::default_boundary() const -> ox::Boundary<double>
{
    return {2 * -3.14, 2 * 3.14, 2, -2};
}

// -----------------------------------------------------------------------------

auto Circle::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) { return x * x + y * y - radius_; }, b,
        coords_);
}

void Circle::step_forward()
{
    if (radius_ <= 0.1 || radius_ > 2.)
        direction_ = -direction_;
    radius_ += (0.01 * direction_);
}

auto Circle::default_boundary() const -> ox::Boundary<double>
{
    return {-2, 2, 2, -2};
}

// -----------------------------------------------------------------------------

auto Biology::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            return std::sin(std::sin(x) + std::cos(y)) -
                   std::cos(std::sin(x * y) + std::cos(x)) + z_;
        },
        b, coords_);
}

void Biology::step_forward()
{
    if (z_ <= -1.5 || z_ > 2.1)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Biology::default_boundary() const -> ox::Boundary<double>
{
    return {-15, 15, 15, -15};
}

// -----------------------------------------------------------------------------

auto Dizzy::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            return std::abs(std::sin(x * x - y * y)) -
                   (std::sin(x + y) + std::cos(x * y)) + z_;
        },
        b, coords_);
}

void Dizzy::step_forward()
{
    if (z_ <= -3 || z_ > 2.5)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Dizzy::default_boundary() const -> ox::Boundary<double>
{
    return {-25, 25, 25, -25};
}

// -----------------------------------------------------------------------------

auto Board::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            auto const euler_constant = std::exp(1.0);
            return std::pow(euler_constant, std::sin(x) + std::cos(y)) -
                   std::sin(std::pow(euler_constant, x + y)) - z_;
        },
        b, coords_);
}

void Board::step_forward()
{
    if (z_ <= -1 || z_ > 8)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Board::default_boundary() const -> ox::Boundary<double>
{
    return {-15, 15, 15, -15};
}

// -----------------------------------------------------------------------------

auto Alien::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            return std::sin(std::cos(std::tan(x * y))) -
                   std::sin(std::cos(std::tan(x))) +
                   std::sin(std::cos(std::tan(y))) - z_;
        },
        b, coords_);
}

void Alien::step_forward()
{
    if (z_ <= -2.5 || z_ > 2.5)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Alien::default_boundary() const -> ox::Boundary<double>
{
    return {-7, 7, 7, -7};
}

// -----------------------------------------------------------------------------

auto Mirror::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            return std::sin(y) - std::sin(std::tgamma(x + 1)) - z_;
        },
        b, coords_);
}

void Mirror::step_forward()
{
    if (z_ <= -2.2 || z_ > 2.2)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Mirror::default_boundary() const -> ox::Boundary<double>
{
    return {-5, 5, 7, -3};
}

// -----------------------------------------------------------------------------

auto Tunnel::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            return std::cos(1. / x) - std::sin(1. / y) - z_;
        },
        b, coords_);
}

void Tunnel::step_forward()
{
    if (z_ <= -2.2 || z_ > 2.2)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Tunnel::default_boundary() const -> ox::Boundary<double>
{
    return {-5, 5, 5, -5};
}

// -----------------------------------------------------------------------------

auto Star::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            return std::sin(y * y * x * x * x) - std::cos(y * y * y * x * x) -
                   z_;
        },
        b, coords_);
}

void Star::step_forward()
{
    if (z_ <= -2.2 || z_ > 2.2)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Star::default_boundary() const -> ox::Boundary<double>
{
    return {-8, 8, 8, -8};
}

// -----------------------------------------------------------------------------

auto Moons::generate(ox::Boundary<double> b)
    -> std::vector<ox::Graph<>::Coordinate>&
{
    return ::implicit_points(
        [this](double x, double y) {
            return std::sin(x * x + y * y) - std::cos(x) - z_;
        },
        b, coords_);
}

void Moons::step_forward()
{
    if (z_ <= -2.2 || z_ > 2.2)
        direction_ = -direction_;
    z_ += (0.01 * direction_);
}

auto Moons::default_boundary() const -> ox::Boundary<double>
{
    return {-8, 8, 8, -8};
}

// -----------------------------------------------------------------------------

template <typename Generator_t, typename... Args>
Graph_core::Graph_core(Args&&... args)
{
    using namespace ox::pipe;

    this->set_generator<Generator_t>(std::forward<Args>(args)...);
    *this | fg(ox::apple_ii::Lime) | strong_focus();
}

template <typename Generator_t, typename... Args>
void Graph_core::set_generator(Args&&... args)
{
    generator_ = std::make_unique<Generator_t>(std::forward<Args>(args)...);
    this->reset_scale_and_offset();
    this->set_boundary_and_generate(generator_->default_boundary());
}

void Graph_core::set_boundary_and_generate(ox::Boundary<double> b)
{
    this->Graph::set_boundary(b);
    this->Graph::reset(generator_->generate(b));
}

/// Return a reference to the current Graph_generator object.
auto Graph_core::generator() const -> Graph_generator const&
{
    assert(generator_ != nullptr);
    return *generator_;
}

/// Return a reference to the current Graph_generator object.
auto Graph_core::generator() -> Graph_generator&
{
    assert(generator_ != nullptr);
    return *generator_;
}

void Graph_core::set_animation_interval(std::chrono::milliseconds interval)
{
    assert(interval.count() > 0);
    animation_interval_ = interval;
    if (this->Widget::is_animated()) {
        this->Widget::disable_animation();
        this->Widget::enable_animation(animation_interval_);
    }
}

auto Graph_core::animation_interval() const -> std::chrono::milliseconds
{
    return animation_interval_;
}

void Graph_core::set_scale(double s)
{
    assert(s > 0);
    scale_ = s;
    auto const bounds =
        ox::scale(this->generator().default_boundary() + offset_, scale_);
    this->set_boundary_and_generate(bounds);
}

auto Graph_core::scale() const -> double { return scale_; }

void Graph_core::set_offset(ox::Boundary<double> offset)
{
    auto const bounds =
        ox::scale(this->generator().default_boundary() + offset_, scale_);
    assert(bounds.west < bounds.east && bounds.south < bounds.north);

    offset_ = offset;
    this->set_boundary_and_generate(bounds);
}

auto Graph_core::offset() const -> ox::Boundary<double> { return offset_; }

auto Graph_core::timer_event() -> bool
{
    generator_->step_forward();
    this->Graph::reset(generator_->generate(this->Graph::boundary()));
    return Graph::timer_event();
}

auto Graph_core::disable_event() -> bool
{
    this->Widget::disable_animation();
    return Graph::disable_event();
}

auto Graph_core::mouse_wheel_event(ox::Mouse const& m) -> bool
{
    auto const s = this->scale();
    switch (m.button) {
        case ox::Mouse::Button::ScrollUp:
            this->set_scale(std::max(s - (s * 0.01), 0.001));
            scale_modified.emit(this->scale());
            break;
        case ox::Mouse::Button::ScrollDown:
            this->set_scale(std::min(s + (s * 0.01), 50.));
            scale_modified.emit(this->scale());
            break;
        default: break;
    }
    return ox::Graph<double>::mouse_wheel_event(m);
}

auto Graph_core::key_press_event(ox::Key k) -> bool
{
    switch (k) {
        case ox::Key::Arrow_left: {
            auto const b = [this] {
                auto const previous = this->Graph::boundary();
                auto const amount   = (previous.east - previous.west) * .025;
                offset_             = ox::scroll_west(offset_, amount);
                offset_modified.emit(offset_);
                return ox::scroll_west(previous, amount);
            }();
            this->set_boundary_and_generate(b);
            break;
        }
        case ox::Key::Arrow_right: {
            auto const b = [this] {
                auto const previous = this->Graph::boundary();
                auto const amount   = (previous.east - previous.west) * .025;
                offset_             = ox::scroll_east(offset_, amount);
                offset_modified.emit(offset_);
                return ox::scroll_east(previous, amount);
            }();
            this->set_boundary_and_generate(b);
            break;
        }
        case ox::Key::Arrow_up: {
            auto const b = [this] {
                auto const previous = this->Graph::boundary();
                auto const amount   = (previous.north - previous.south) * .025;
                offset_             = ox::scroll_north(offset_, amount);
                offset_modified.emit(offset_);
                return ox::scroll_north(previous, amount);
            }();
            this->set_boundary_and_generate(b);
            break;
        }
        case ox::Key::Arrow_down: {
            auto const b = [this] {
                auto const previous = this->Graph::boundary();
                auto const amount   = (previous.north - previous.south) * .025;
                offset_             = ox::scroll_south(offset_, amount);
                offset_modified.emit(offset_);
                return ox::scroll_south(previous, amount);
            }();
            this->set_boundary_and_generate(b);
            break;
        }
        default: break;
    }
    return ox::Graph<double>::key_press_event(k);
}

void Graph_core::reset_scale_and_offset()
{
    scale_  = 1;
    offset_ = {0, 0, 0, 0};
}

Step_interval::Step_interval(Parameters)
    : ox::HPair<ox::HLabel, ox::Int_edit>{{U"StepInterval(ms)"},
                                          {16, {1, 5'000}}}
{
    *this | ox::pipe::fixed_height(1);
}

Scale_box::Scale_box(Parameters)
    : ox::HPair<ox::HLabel, ox::Double_edit>{{U"Scale"}, {1.}}
{
    using namespace ox;
    using namespace ox::pipe;

    *this | fixed_height(1);
    label | fixed_width(8) | bg(apple_ii::Violet) | fg(apple_ii::White);

    edit | bg(apple_ii::Lavendar) | fg(apple_ii::Violet);
    edit.set_increment(0.01);
    edit.set_range({0.001, 50});
}

Offset_box::Offset_box(Parameters p)
    : ox::HPair<ox::HLabel, ox::Double_edit>{{std::move(p.label)}, {0.}}
{
    using namespace ox;
    using namespace ox::pipe;

    *this | fixed_height(1);
    label | fixed_width(8) | bg(apple_ii::Violet) | fg(apple_ii::White);
    edit | bg(apple_ii::Lavendar) | fg(apple_ii::Violet);
}

Offset_boxes::Offset_boxes(Parameters)
    : ox::VTuple<ox::HLabel, Offset_box, Offset_box, Offset_box, Offset_box>{
          {U"Offsets" | ox::Trait::Bold},
          {U"West"},
          {U"East"},
          {U"North"},
          {U"South"}}
{
    *this | ox::pipe::fixed_height(5);
}

Settings::Settings()
    : ox::VTuple<ox::Labeled_cycle_box,
                 ox::HLine,
                 Step_interval,
                 ox::Toggle_button,
                 ox::HLine,
                 ox::Widget,
                 ox::HLine,
                 Scale_box,
                 Offset_boxes,
                 ox::Confirm_button>{
          {U"Graph"}, {}, {}, {U"Start", U"Stop"}, {}, {},
          {},         {}, {}, {U"Reset"}}
{
    using namespace ox;
    using namespace ox::pipe;
    *this | fixed_width(25) | west_border();

    start_stop_btn | fixed_height(1);
    start_stop_btn.top | bg(apple_ii::Green);
    start_stop_btn.bottom | bg(apple_ii::Dark_pink);

    graph_box.cycle_box.add_option("Sine").connect(
        [this] { graph_changed.emit("sine"); });

    graph_box.cycle_box.add_option("Sine Three").connect([this] {
        graph_changed.emit("sine-three");
    });

    graph_box.cycle_box.add_option("Sine Phase").connect([this] {
        graph_changed.emit("sine-phase");
    });

    graph_box.cycle_box.add_option("Circle").connect(
        [this] { graph_changed.emit("circle"); });

    graph_box.cycle_box.add_option("Biology").connect(
        [this] { graph_changed.emit("biology"); });

    graph_box.cycle_box.add_option("Dizzy").connect(
        [this] { graph_changed.emit("dizzy"); });

    graph_box.cycle_box.add_option("Board").connect(
        [this] { graph_changed.emit("board"); });

    graph_box.cycle_box.add_option("Alien").connect(
        [this] { graph_changed.emit("alien"); });

    graph_box.cycle_box.add_option("Mirror").connect(
        [this] { graph_changed.emit("mirror"); });

    graph_box.cycle_box.add_option("Tunnel").connect(
        [this] { graph_changed.emit("tunnel"); });

    graph_box.cycle_box.add_option("Star").connect(
        [this] { graph_changed.emit("star"); });

    graph_box.cycle_box.add_option("Moons").connect(
        [this] { graph_changed.emit("moons"); });

    step_interval_box.edit.submitted.connect(
        [this](int interval) { step_interval_changed(interval); });
}

void Settings::zero_offset_boxes()
{
    offset_boxes.west_box.edit.set_value(0);
    offset_boxes.east_box.edit.set_value(0);
    offset_boxes.north_box.edit.set_value(0);
    offset_boxes.south_box.edit.set_value(0);
}

void Settings::reset_scale_box() { scale_box.edit.set_value(1); }

template <typename Generator_t, typename... Args>
void Graph_demo::set_generator(Args&&... args)
{
    core.set_generator<Generator_t>(std::forward<Args>(args)...);
    settings.zero_offset_boxes();
    settings.reset_scale_box();
}

Graph_demo::Graph_demo()
{
    using namespace ox::pipe;

    settings.graph_changed.connect([this](std::string gen) {
        if (gen == "sine")
            this->set_generator<Sine>();
        else if (gen == "sine-three")
            this->set_generator<Sine_three>();
        else if (gen == "sine-phase")
            this->set_generator<Sine_phase>();
        else if (gen == "circle")
            this->set_generator<Circle>();
        else if (gen == "biology")
            this->set_generator<Biology>();
        else if (gen == "dizzy")
            this->set_generator<Dizzy>();
        else if (gen == "board")
            this->set_generator<Board>();
        else if (gen == "alien")
            this->set_generator<Alien>();
        else if (gen == "mirror")
            this->set_generator<Mirror>();
        else if (gen == "tunnel")
            this->set_generator<Tunnel>();
        else if (gen == "star")
            this->set_generator<Star>();
        else if (gen == "moons")
            this->set_generator<Moons>();
    });

    settings.step_interval_changed.connect([this](int interval) {
        core.set_animation_interval(std::chrono::milliseconds{interval});
    });

    settings.start_stop_btn.top_pressed.connect(
        [this] { core.enable_animation(core.animation_interval()); });

    settings.start_stop_btn.bottom_pressed.connect(
        [this] { core.disable_animation(); });

    core.disabled.connect([this] { settings.start_stop_btn.show_top(); });

    settings.scale_box.edit.submitted.connect(
        [this](double amount) { core.set_scale(amount); });

    settings.offset_boxes.west_set.connect([this](double offset) {
        core.set_offset([&] {
            auto const db = core.generator().default_boundary();
            auto off      = core.offset();
            if ((db.west + offset) < (db.east + off.east))
                off.west = offset;
            else {
                off.west = db.east + off.east - db.west - 0.001;
                settings.offset_boxes.west_box.edit.set_value(off.west);
            }
            return off;
        }());
    });

    settings.offset_boxes.east_set.connect([this](double offset) {
        core.set_offset([&] {
            auto const db = core.generator().default_boundary();
            auto off      = core.offset();
            if ((db.east + offset) > (db.west + off.west))
                off.east = offset;
            else {
                off.east = db.west + off.west + 0.001 - db.east;
                settings.offset_boxes.east_box.edit.set_value(off.east);
            }
            return off;
        }());
    });

    settings.offset_boxes.north_set.connect([this](double offset) {
        core.set_offset([&] {
            auto const db = core.generator().default_boundary();
            auto off      = core.offset();
            if ((db.north + offset) > (db.south + off.south))
                off.north = offset;
            else {
                off.north = db.south + off.south + 0.001 - db.north;
                settings.offset_boxes.north_box.edit.set_value(off.north);
            }
            return off;
        }());
    });

    settings.offset_boxes.south_set.connect([this](double offset) {
        core.set_offset([&] {
            auto const db = core.generator().default_boundary();
            auto off      = core.offset();
            if ((db.south + offset) < (db.north + off.north))
                off.south = offset;
            else {
                off.south = db.north + off.north - db.south - 0.001;
                settings.offset_boxes.south_box.edit.set_value(off.south);
            }
            return off;
        }());
    });

    settings.reset_btn | on_press([this] {
        core.reset_scale_and_offset();
        core.set_boundary_and_generate(core.generator().default_boundary());
        settings.zero_offset_boxes();
        settings.reset_scale_box();
    });
    settings.reset_btn.main_btn | bg(ox::apple_ii::Green);

    core.scale_modified.connect(
        [&](double s) { settings.scale_box.edit.set_value(s); });

    core.offset_modified.connect([this](auto b) {
        settings.offset_boxes.west_box.edit.set_value(b.west);
        settings.offset_boxes.east_box.edit.set_value(b.east);
        settings.offset_boxes.north_box.edit.set_value(b.north);
        settings.offset_boxes.south_box.edit.set_value(b.south);
    });
}

}  // namespace graph
