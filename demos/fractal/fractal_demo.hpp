#ifndef TERMOX_DEMOS_FRACTAL_FRACTAL_DEMO_HPP
#define TERMOX_DEMOS_FRACTAL_FRACTAL_DEMO_HPP
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

#include <iostream>  //temp

#include <termox/termox.hpp>

#include "float_t.hpp"
#include "julia.hpp"
#include "mandelbrot.hpp"

namespace fractal {

/// Given a fractal function for a single point, will generate a field of points
/** For julia, wrap it in a lambda and hard code the value of c in there. */
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
                 ox::Color(std::forward<Fn>(generator)(x, y, resolution) - 1)});
        }
    }
    return result;
}

[[nodiscard]] inline auto generate_palette(std::uint16_t count) -> ox::Palette
{
    assert(count <= 256);
    auto result        = ox::Palette{};
    auto const h_ratio = 359. / count;
    auto const l_ratio = 75. / count;
    for (auto i = std::uint16_t{1}; i <= count; ++i) {
        result.push_back({ox::Color(count - i),
                          ox::HSL{static_cast<std::uint16_t>(h_ratio * i), 70,
                                  static_cast<std::uint8_t>(l_ratio * i)}});
    }
    return result;
}

enum class Fractal { Mandelbrot, Julia };

class Side_pane : public ox::Bordered<ox::VTuple<ox::Cycle_box, ox::Widget>> {
   public:
    ox::Cycle_box& picker = this->wrapped.get<0>();

   public:
    sl::Signal<void(Fractal)> fractal_changed;

   public:
    Side_pane()
    {
        using namespace ox::pipe;
        *this | take_west() | fixed_width(16);

        picker.add_option(U"Mandelbrot").connect([this] {
            fractal_changed.emit(Fractal::Mandelbrot);
        });

        picker.add_option(U"Julia").connect(
            [this] { fractal_changed.emit(Fractal::Julia); });
    }
};

class Fractal_demo : public ox::HPair<ox::Color_graph<Float_t>, Side_pane> {
   public:
    ox::Color_graph<Float_t>& graph = this->first;
    Side_pane& side_pane            = this->second;

   public:
    Fractal_demo()
        : ox::HPair<ox::Color_graph<Float_t>, Side_pane>{{{-2, 2, 2, -2}, {}},
                                                         {}}
    {
        using namespace ox::pipe;

        auto const pal = generate_palette(128);
        // auto const pal = ox::dawn_bringer32::palette;

        *this | direct_focus() |
            on_focus_in([=] { ox::Terminal::set_palette(pal); }) |
            forward_focus(graph);

        side_pane.fractal_changed.connect([this](auto type) {
            fractal_type_ = type;
            this->reset();
        });

        graph | strong_focus();

        graph.key_pressed.connect([this](auto k) {
            switch (k) {
                case ox::Key::Arrow_right:
                    this->increment_h_offset();
                    this->reset();
                    break;
                case ox::Key::Arrow_left:
                    this->decrement_h_offset();
                    this->reset();
                    break;
                case ox::Key::Arrow_up:
                    this->increment_v_offset();
                    this->reset();
                    break;
                case ox::Key::Arrow_down:
                    this->decrement_v_offset();
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
                case ox::Mouse::Button::ScrollDown:
                    this->zoom_out_at(m.at);
                    break;
                default: break;
            }
        });

        *this | on_resize([this](auto, auto) { this->reset(); });

        this->reset(default_boundary, mandelbrot);
    }

   private:
    static auto constexpr default_boundary =
        ox::Boundary<Float_t>{-2, 2, 2, -2};

    static auto constexpr zoom_scale = 0.05;

    Fractal fractal_type_          = Fractal::Mandelbrot;
    ox::Boundary<Float_t> offsets_ = {0, 0, 0, 0};

   private:
    template <typename Fn>
    void reset(ox::Boundary<Float_t> b, Fn&& generator)
    {
        if (this->area().width == 0 || this->area().height == 0)
            return;
        auto const x_interval = Float_t(b.east - b.west) / this->area().width;
        auto const y_interval =
            Float_t(b.north - b.south) / (this->area().height * 2.1);

        auto points = generate_points(b, x_interval, y_interval,
                                      ox::Terminal::current_palette().size(),
                                      std::forward<Fn>(generator));

        this->first.set_boundary(b);
        this->first.reset(std::move(points));
    }

    void reset()
    {
        auto const bound = apply_offsets(default_boundary, offsets_);

        switch (fractal_type_) {
            case Fractal::Mandelbrot: this->reset(bound, mandelbrot); break;
            case Fractal::Julia:
                this->reset(bound, [](auto x, auto y, auto mi) {
                    return julia(x, y, std::complex<Float_t>{-0.79, 0.15}, mi);
                });
                break;
        }
    }

    void increment_h_offset()
    {
        auto const amount =
            Float_t(graph.boundary().east - graph.boundary().west) / 25.;
        offsets_.west += amount;
        offsets_.east += amount;
    }

    void decrement_h_offset()
    {
        auto const amount =
            Float_t(graph.boundary().east - graph.boundary().west) / 25.;
        offsets_.west -= amount;
        offsets_.east -= amount;
    }

    void increment_v_offset()
    {
        auto const amount =
            Float_t(graph.boundary().north - graph.boundary().south) / 25.;
        offsets_.north += amount;
        offsets_.south += amount;
    }

    void decrement_v_offset()
    {
        auto const amount =
            Float_t(graph.boundary().north - graph.boundary().south) / 15.;
        offsets_.north -= amount;
        offsets_.south -= amount;
    }

    /// Centered zoom in.
    void zoom_in()
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

    void zoom_in_at(ox::Point p)
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

    /// Centered zoom out.
    void zoom_out()
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

    void zoom_out_at(ox::Point p)
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

   private:
    /// Adds the given offset in each direction to the given boundary.
    [[nodiscard]] static auto apply_offsets(
        ox::Boundary<Float_t> boundary,
        ox::Boundary<Float_t> const& offsets) -> ox::Boundary<Float_t>
    {
        boundary.west += offsets.west;
        boundary.east += offsets.east;
        boundary.north += offsets.north;
        boundary.south += offsets.south;
        return boundary;
    }
};

}  // namespace fractal
#endif  // TERMOX_DEMOS_FRACTAL_FRACTAL_DEMO_HPP
