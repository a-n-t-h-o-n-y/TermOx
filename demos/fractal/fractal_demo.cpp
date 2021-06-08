#include "fractal_demo.hpp"

#include <cassert>
#include <cmath>
#include <future>
#include <thread>
#include <utility>
#include <vector>

#include <termox/termox.hpp>

#include "float_t.hpp"
#include "julia.hpp"
#include "mandelbrot.hpp"

namespace {

using fractal::Float_t;

auto constexpr zoom_scale = 0.05;

auto constexpr default_boundary = ox::Boundary<Float_t>{-2, 2, 2, -2};

using ox::Color;
using ox::RGB;

/// Fractal colors start at color 16.
auto const custom_palette = ox::Palette{
    {Color::Background, RGB{000000}}, {Color::Foreground, RGB{0xFFFFFF}},
    {Color{16}, RGB{0x02044e}},       {Color{17}, RGB{0x180450}},
    {Color{18}, RGB{0x270252}},       {Color{19}, RGB{0x330154}},
    {Color{20}, RGB{0x3e0055}},       {Color{21}, RGB{0x480056}},
    {Color{22}, RGB{0x520057}},       {Color{23}, RGB{0x5c0058}},
    {Color{24}, RGB{0x650058}},       {Color{25}, RGB{0x6e0058}},
    {Color{26}, RGB{0x770058}},       {Color{27}, RGB{0x800058}},
    {Color{28}, RGB{0x880057}},       {Color{29}, RGB{0x900056}},
    {Color{30}, RGB{0x980455}},       {Color{31}, RGB{0x9f0d54}},
    {Color{32}, RGB{0xa71553}},       {Color{33}, RGB{0xad1c52}},
    {Color{34}, RGB{0xb42350}},       {Color{35}, RGB{0xba2b4e}},
    {Color{36}, RGB{0xc0324d}},       {Color{37}, RGB{0xc6394b}},
    {Color{38}, RGB{0xcb4049}},       {Color{39}, RGB{0xd04847}},
    {Color{40}, RGB{0xd54f45}},       {Color{41}, RGB{0xd95644}},
    {Color{42}, RGB{0xdd5e42}},       {Color{43}, RGB{0xe06640}},
    {Color{44}, RGB{0xe36d3e}},       {Color{45}, RGB{0xe6753d}},
    {Color{46}, RGB{0xe87d3b}},       {Color{47}, RGB{0xea853a}},
    {Color{48}, RGB{0xec8d39}},       {Color{49}, RGB{0xed9438}},
    {Color{50}, RGB{0xee9c38}},       {Color{51}, RGB{0xefa439}},
    {Color{52}, RGB{0xefac3a}},       {Color{53}, RGB{0xefb43b}},
    {Color{54}, RGB{0xefbc3e}},       {Color{55}, RGB{0xeec441}},
    {Color{56}, RGB{0xefbc3e}},       {Color{57}, RGB{0xefb43b}},
    {Color{58}, RGB{0xefac3a}},       {Color{59}, RGB{0xefa439}},
    {Color{60}, RGB{0xee9c38}},       {Color{61}, RGB{0xed9438}},
    {Color{62}, RGB{0xec8d39}},       {Color{63}, RGB{0xea853a}},
    {Color{64}, RGB{0xe87d3b}},       {Color{65}, RGB{0xe6753d}},
    {Color{66}, RGB{0xe36d3e}},       {Color{67}, RGB{0xe06640}},
    {Color{68}, RGB{0xdd5e42}},       {Color{69}, RGB{0xd95644}},
    {Color{70}, RGB{0xd54f45}},       {Color{71}, RGB{0xd04847}},
    {Color{72}, RGB{0xcb4049}},       {Color{73}, RGB{0xc6394b}},
    {Color{74}, RGB{0xc0324d}},       {Color{75}, RGB{0xba2b4e}},
    {Color{76}, RGB{0xb42350}},       {Color{77}, RGB{0xad1c52}},
    {Color{78}, RGB{0xa71553}},       {Color{79}, RGB{0x9f0d54}},
    {Color{80}, RGB{0x980455}},       {Color{81}, RGB{0x900056}},
    {Color{82}, RGB{0x880057}},       {Color{83}, RGB{0x800058}},
    {Color{84}, RGB{0x770058}},       {Color{85}, RGB{0x6e0058}},
    {Color{86}, RGB{0x650058}},       {Color{87}, RGB{0x5c0058}},
    {Color{88}, RGB{0x520057}},       {Color{89}, RGB{0x480056}},
    {Color{90}, RGB{0x3e0055}},       {Color{91}, RGB{0x330154}},
    {Color{92}, RGB{0x270252}},       {Color{93}, RGB{0x180450}}};

/// Adds the given offset in each direction to the given boundary.
[[nodiscard]] auto apply_offsets(ox::Boundary<Float_t> boundary,
                                 ox::Boundary<Float_t> const& offsets)
    -> ox::Boundary<Float_t>
{
    boundary.west += offsets.west;
    boundary.east += offsets.east;
    boundary.north += offsets.north;
    boundary.south += offsets.south;
    return boundary;
}

/// Given a fractal function for a single point, will generate a field of points
template <typename Fn>
[[nodiscard]] auto generate_points(ox::Boundary<Float_t> boundary,
                                   Float_t x_step,
                                   Float_t y_step,
                                   unsigned int resolution,
                                   Fn&& generator)
    -> std::vector<std::pair<ox::Color_graph<Float_t>::Coordinate, ox::Color>>
{
    auto result = std::vector<
        std::pair<ox::Color_graph<Float_t>::Coordinate, ox::Color>>{};
    auto const count = ((boundary.east - boundary.west) / x_step) *
                       ((boundary.north - boundary.south) / y_step);
    result.reserve(count);
    for (auto x = boundary.west; x <= boundary.east; x += x_step) {
        for (auto y = boundary.south; y <= boundary.north; y += y_step) {
            result.push_back(
                {{x, y},
                 ox::Color(
                     ((std::forward<Fn>(generator)(x, y, resolution) - 1) %
                      (custom_palette.size() - 2)) +
                     16)});
        }
    }
    return result;
}

/// Generates a field of fractal values in parallel, depending on hardward cores
template <typename Fn>
[[nodiscard]] auto par_generate_points(ox::Boundary<Float_t> boundary,
                                       Float_t x_step,
                                       Float_t y_step,
                                       unsigned int resolution,
                                       Fn&& generator)
    -> std::vector<std::pair<ox::Color_graph<Float_t>::Coordinate, ox::Color>>
{
    auto result = std::vector<
        std::pair<ox::Color_graph<Float_t>::Coordinate, ox::Color>>{};
    auto const count = ((boundary.east - boundary.west) / x_step) *
                       ((boundary.north - boundary.south) / y_step);
    result.reserve(count);

    // Parallelize Into Chunks
    auto const thread_count = std::thread::hardware_concurrency();
    auto const chunk_distance =
        (boundary.east - boundary.west) / (double)thread_count;
    auto boundaries = std::vector<ox::Boundary<Float_t>>{};
    boundaries.reserve(thread_count);
    for (auto i = 0uL; i < thread_count; ++i) {
        auto b = boundary;
        b.west = b.west + (i * chunk_distance);
        b.east = b.west + chunk_distance;
        boundaries.push_back(b);
    }

    // Launch Threads
    auto futs = std::vector<std::future<std::vector<
        std::pair<ox::Color_graph<Float_t>::Coordinate, ox::Color>>>>{};
    futs.reserve(thread_count);
    for (auto const& b : boundaries) {
        futs.push_back(std::async(std::launch::async, [=] {
            return generate_points(b, x_step, y_step, resolution, generator);
        }));
    }

    // Wait and Concat Results
    for (auto& f : futs) {
        auto points = f.get();
        result.insert(std::end(result), std::begin(points), std::end(points));
    }

    return result;
}

}  // namespace

namespace fractal {

Float_edit::Float_edit()
{
    using namespace ox;
    *this | pipe::fixed_height(1);
    this->second | bg(Color::White) | fg(Color::Black);
}

Top_bar::Top_bar()
{
    using namespace ox::pipe;
    *this | fixed_height(1);

    this->add_option(U"< Mandelbrot >").connect([this] {
        fractal_changed.emit(Fractal::Mandelbrot);
    });

    this->add_option(U"<    Julia   >").connect([this] {
        fractal_changed.emit(Fractal::Julia);
    });
}

Fractal_demo::Fractal_demo()
    : ox::VPair<Top_bar, ox::Color_graph<Float_t>>{{}, {{-2, 2, 2, -2}, {}}}
{
    using namespace ox::pipe;

    auto const pal = custom_palette;

    *this | direct_focus() |
        on_focus_in([=] { ox::Terminal::set_palette(pal); }) |
        forward_focus(graph);

    graph | strong_focus();

    top_bar.fractal_changed.connect([this](auto type) {
        fractal_type_ = type;
        this->reset();
    });

    graph.mouse_moved.connect([this](auto const& m) {
        if (fractal_type_ != Fractal::Julia)
            return;
        auto const b  = graph.boundary();
        auto const hr = ((b.east - b.west) / (double)graph.area().width);
        auto const vr = ((b.north - b.south) / (double)graph.area().height);
        julia_c_.real(b.west + (m.at.x * hr));
        julia_c_.imag(b.south + (m.at.y * vr));
        this->reset();
    });

    graph.key_pressed.connect([this](auto k) {
        switch (k) {
            case ox::Key::Arrow_right:
                this->increment_h_offset(+1);
                this->reset();
                break;
            case ox::Key::Arrow_left:
                this->increment_h_offset(-1);
                this->reset();
                break;
            case ox::Key::Arrow_up:
                this->increment_v_offset(+1);
                this->reset();
                break;
            case ox::Key::Arrow_down:
                this->increment_v_offset(-1);
                this->reset();
                break;
            default: break;
        }
        // If statements to get around switch warning, not in enum.
        if (k == (ox::Mod::Ctrl | ox::Key::Arrow_up))
            this->zoom_in();
        else if (k == (ox::Mod::Ctrl | ox::Key::Arrow_down))
            this->zoom_out();
    });

    graph.mouse_wheel_scrolled.connect([this](auto m) {
        switch (m.button) {
            case ox::Mouse::Button::ScrollUp: this->zoom_in_at(m.at); break;
            case ox::Mouse::Button::ScrollDown: this->zoom_out_at(m.at); break;
            default: break;
        }
    });

    graph | on_resize([this](auto, auto) { this->reset(); });
    graph | on_enable([this] { this->reset(); });

    this->reset(default_boundary);
}

void Fractal_demo::reset(ox::Boundary<Float_t> b)
{
    if (graph.area().height == 0 || graph.area().width == 0)
        return;
    graph.set_boundary(b);

    auto const x_interval =
        Float_t(b.east - b.west) / (graph.area().width * 1.001);
    auto const y_interval =
        Float_t(b.north - b.south) / (graph.area().height * 2.001);

    auto const distance   = b.east - b.west;
    auto const resolution = std::clamp((int)std::log(1. / distance), 1, 8) *
                            (custom_palette.size() - 2);

    switch (fractal_type_) {
        case Fractal::Mandelbrot:
            graph.reset(par_generate_points(b, x_interval, y_interval,
                                            resolution, mandelbrot));
            break;
        case Fractal::Julia:
            graph.reset(
                par_generate_points(b, x_interval, y_interval, resolution,
                                    [&](auto x, auto y, auto mi) {
                                        return julia(x, y, julia_c_, mi);
                                    }));
            break;
    }
}

void Fractal_demo::reset()
{
    this->reset(apply_offsets(default_boundary, offsets_));
}

void Fractal_demo::increment_h_offset(int direction)
{
    assert(direction == 1 || direction == -1);
    auto constexpr fraction = (Float_t)1. / (Float_t)25.;

    auto const b      = graph.boundary();
    auto const amount = direction * (b.east - b.west) * fraction;
    offsets_.west += amount;
    offsets_.east += amount;
}

void Fractal_demo::increment_v_offset(int direction)
{
    assert(direction == 1 || direction == -1);
    auto constexpr fraction = (Float_t)1. / (Float_t)25.;

    auto const b      = graph.boundary();
    auto const amount = direction * (b.north - b.south) * fraction;
    offsets_.north += amount;
    offsets_.south += amount;
}

void Fractal_demo::zoom_in()
{
    auto const b          = graph.boundary();
    auto const h_distance = (b.east - b.west);
    auto const v_distance = (b.north - b.south);

    offsets_.west += h_distance * zoom_scale;
    offsets_.east -= h_distance * zoom_scale;
    offsets_.north -= v_distance * zoom_scale;
    offsets_.south += v_distance * zoom_scale;

    this->reset();
}

void Fractal_demo::zoom_in_at(ox::Point p)
{
    auto const b = graph.boundary();

    // Horizontal
    auto const h_distance = b.east - b.west;
    auto const h_ratio    = h_distance / graph.area().width;
    auto const h_coord    = h_ratio * p.x;
    offsets_.west += h_coord * zoom_scale;
    offsets_.east -= (h_distance - h_coord) * zoom_scale;

    // Vertical
    auto const v_distance = b.north - b.south;
    auto const v_ratio    = v_distance / graph.area().height;
    auto const v_coord    = v_ratio * (graph.area().height - p.y);
    offsets_.south += v_coord * zoom_scale;
    offsets_.north -= (v_distance - v_coord) * zoom_scale;

    this->reset();
}

void Fractal_demo::zoom_out()
{
    auto const b          = graph.boundary();
    auto const h_distance = (b.east - b.west);
    auto const v_distance = (b.north - b.south);

    offsets_.west -= h_distance * zoom_scale;
    offsets_.east += h_distance * zoom_scale;
    offsets_.north += v_distance * zoom_scale;
    offsets_.south -= v_distance * zoom_scale;

    this->reset();
}

void Fractal_demo::zoom_out_at(ox::Point p)
{
    auto const b = graph.boundary();

    // Horizontal
    auto const h_distance = b.east - b.west;
    auto const h_ratio    = h_distance / graph.area().width;
    auto const h_coord    = h_ratio * p.x;
    offsets_.west -= h_coord * zoom_scale;
    offsets_.east += (h_distance - h_coord) * zoom_scale;

    // Vertical
    auto const v_distance = b.north - b.south;
    auto const v_ratio    = v_distance / graph.area().height;
    auto const v_coord    = v_ratio * (graph.area().height - p.y);
    offsets_.south -= v_coord * zoom_scale;
    offsets_.north += (v_distance - v_coord) * zoom_scale;

    this->reset();
}

}  // namespace fractal
