#ifndef TERMOX_DEMOS_FRACTAL_FRACTAL_DEMO_HPP
#define TERMOX_DEMOS_FRACTAL_FRACTAL_DEMO_HPP
#include <complex>

#include <signals_light/signal.hpp>
#include <termox/widget/boundary.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/graph.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/number_edit.hpp>
#include <termox/widget/widgets/text_view.hpp>
#include <termox/widget/widgets/toggle_button.hpp>

#include "float_t.hpp"

namespace fractal {

enum class Fractal { Mandelbrot, Julia };

struct Float_edit : ox::HPair<ox::HLabel, ox::Number_edit<Float_t>> {
    Float_edit();
};

class Top_bar : public ox::HPair<ox::Cycle_box, ox::Toggle_button> {
   public:
    ox::Cycle_box& selector    = this->first;
    ox::Toggle_button& buttons = this->second;

   public:
    sl::Signal<void(Fractal)> fractal_changed;
    sl::Signal<void()>& instructions_request = buttons.top_pressed;
    sl::Signal<void()>& fractal_view_request = buttons.bottom_pressed;

   public:
    Top_bar();
};

class Instructions : public ox::Text_view {
   public:
    Instructions();
};

class Fractal_demo
    : public ox::VPair<Top_bar,
                       ox::SPair<ox::Color_graph<Float_t>, Instructions>> {
   public:
    Top_bar& top_bar                = this->first;
    ox::Color_graph<Float_t>& graph = this->second.first;

   public:
    Fractal_demo();

   private:
    Fractal fractal_type_          = Fractal::Mandelbrot;
    ox::Boundary<Float_t> offsets_ = {0, 0, 0, 0};
    std::complex<Float_t> julia_c_ = {-0.79, 0.15};

   private:
    /// Recalculate the currently set fractal_type_ with b and set Color_graph.
    void reset(ox::Boundary<Float_t> b);

    /// Reset with the current boundary parameters
    void reset();

    /// Move the horizontal boundary offset.
    /** Moved in \p direction by a fraction of the horizontal length. */
    void increment_h_offset(int direction);

    /// Move the vertical boundary offset.
    /** Moved in \p direction by a fraction of the vertical length. */
    void increment_v_offset(int direction);

    /// Centered zoom in.
    void zoom_in();

    /// Zoom in towards Point \p p.
    void zoom_in_at(ox::Point p);

    /// Centered zoom out.
    void zoom_out();

    /// Zoom out towards Point \p p.
    void zoom_out_at(ox::Point p);

    void show_instructions() { this->second.set_active_page(1); }

    void show_fractals() { this->second.set_active_page(0); }
};

}  // namespace fractal
#endif  // TERMOX_DEMOS_FRACTAL_FRACTAL_DEMO_HPP
