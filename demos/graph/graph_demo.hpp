#ifndef TERMOX_DEMOS_GRAPH_GRAPH_DEMO_HPP
#define TERMOX_DEMOS_GRAPH_GRAPH_DEMO_HPP
#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/confirm_button.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/graph.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/line.hpp>
#include <termox/widget/widgets/number_edit.hpp>
#include <termox/widget/widgets/toggle_button.hpp>
#include "termox/widget/array.hpp"

namespace graph {

class Graph_generator {
   public:
    virtual ~Graph_generator() = default;

   public:
    /// Return points to display in Graph.
    virtual auto generate(ox::Boundary<double>)
        -> std::vector<ox::Graph<>::Coordinate>& = 0;

    /// Increment internal state.
    virtual void step_forward() = 0;

    /// Retrieve the default boundary dimensions this should be displayed with.
    virtual auto default_boundary() const -> ox::Boundary<double> = 0;
};

class Sine final : public Graph_generator {
   public:
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double t_ = 0.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Sine_three final : public Graph_generator {
   public:
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double t_ = 0.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Tan final : public Graph_generator {
   public:
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double t_         = 0.;
    double direction_ = 1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Sine_phase final : public Graph_generator {
   public:
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double a_ = 0.;
    std::vector<ox::Graph<>::Coordinate> static_sine_;
    std::vector<ox::Graph<>::Coordinate> moving_sine_;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Circle final : public Graph_generator {
   public:
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double radius_    = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Biology final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Dizzy final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Board final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Alien final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Mirror final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Tunnel final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Star final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Moons final : public Graph_generator {
    auto generate(ox::Boundary<double> b)
        -> std::vector<ox::Graph<>::Coordinate>& override;

    void step_forward() override;

    auto default_boundary() const -> ox::Boundary<double> override;

   private:
    double z_         = 1.;
    double direction_ = -1.;
    std::vector<ox::Graph<>::Coordinate> coords_;
};

class Graph_core : public ox::Graph<double> {
   public:
    /// Emitted on mouse wheel events, where core has already updated, but
    /// the Setting's scale_box needs to be updated to match.
    sl::Signal<void(double)> scale_modified;

    /// Emitted on key press events, where core has already updated, but / the
    /// Setting's offset_boxes need to be updated to match.
    sl::Signal<void(ox::Boundary<double>)> offset_modified;

   public:
    template <typename Generator_t = Sine, typename... Args>
    explicit Graph_core(Args&&... args);

   public:
    template <typename Generator_t, typename... Args>
    void set_generator(Args&&... args);

    void set_boundary_and_generate(ox::Boundary<double> b);

    /// Return a reference to the current Graph_generator object.
    [[nodiscard]] auto generator() const -> Graph_generator const&;

    /// Return a reference to the current Graph_generator object.
    [[nodiscard]] auto generator() -> Graph_generator&;

    /// Set the number of milliseconds between animation frames.
    void set_animation_interval(std::chrono::milliseconds interval);

    /// Return the number of milliseconds between animation frames.
    [[nodiscard]] auto animation_interval() const -> std::chrono::milliseconds;

    /// Set the scale applied to the Boundary.
    void set_scale(double s);

    /// Return the Boundary scale that is applied.
    [[nodiscard]] auto scale() const -> double;

    /// Set the offset applied to the default boundary.
    void set_offset(ox::Boundary<double> b);

    /// Return the offset applied to the default boundary.
    [[nodiscard]] auto offset() const -> ox::Boundary<double>;

    /// sets scale to one and offsets to zeros, does not regenerate points.
    void reset_scale_and_offset();

   protected:
    auto timer_event() -> bool override;

    auto disable_event() -> bool override;

    auto mouse_wheel_event(ox::Mouse const& m) -> bool override;

    auto key_press_event(ox::Key k) -> bool override;

   private:
    std::unique_ptr<Graph_generator> generator_ = nullptr;
    std::chrono::milliseconds animation_interval_ =
        std::chrono::milliseconds{16};
    double scale_                = 1.;
    ox::Boundary<double> offset_ = {0, 0, 0, 0};
};

class Step_interval : public ox::HPair<ox::HLabel, ox::Int_edit> {
   public:
    ox::HLabel& label = this->first | ox::pipe::fixed_width(17);
    ox::Int_edit& edit =
        this->second | bg(ox::Color::White) | fg(ox::Color::Black);

   public:
    explicit Step_interval();
};

class Scale_box : public ox::HPair<ox::HLabel, ox::Double_edit> {
   public:
    explicit Scale_box(Parameters = {});

   public:
    ox::HLabel& label     = this->first;
    ox::Double_edit& edit = this->second;
};

class Offset_box : public ox::HPair<ox::HLabel, ox::Double_edit> {
   public:
    struct Parameters {
        ox::Glyph_string label;
    };

   public:
    ox::HLabel& label     = this->first;
    ox::Double_edit& edit = this->second;

   public:
    explicit Offset_box(Parameters p);
};

class Offset_boxes
    : public ox::
          VTuple<ox::HLabel, Offset_box, Offset_box, Offset_box, Offset_box> {
   public:
    ox::HLabel& title     = this->get<0>();
    Offset_box& west_box  = this->get<1>();
    Offset_box& east_box  = this->get<2>();
    Offset_box& north_box = this->get<3>();
    Offset_box& south_box = this->get<4>();

   public:
    sl::Signal<void(double)>& west_set  = west_box.edit.submitted;
    sl::Signal<void(double)>& east_set  = east_box.edit.submitted;
    sl::Signal<void(double)>& north_set = north_box.edit.submitted;
    sl::Signal<void(double)>& south_set = south_box.edit.submitted;

   public:
    explicit Offset_boxes(Parameters = {});
};

struct Settings : ox::VTuple<ox::Labeled_cycle_box,
                             ox::HLine,
                             Step_interval,
                             ox::Toggle_button,
                             ox::HLine,
                             ox::Widget,
                             ox::HLine,
                             Scale_box,
                             Offset_boxes,
                             ox::Confirm_button> {
   public:
    sl::Signal<void(std::string)> graph_changed;
    sl::Signal<void(int)> step_interval_changed;

   public:
    ox::Labeled_cycle_box& graph_box  = this->get<0>();
    Step_interval& step_interval_box  = this->get<2>();
    ox::Toggle_button& start_stop_btn = this->get<3>();
    Scale_box& scale_box              = this->get<7>();
    Offset_boxes& offset_boxes        = this->get<8>();
    ox::Confirm_button& reset_btn     = this->get<9>();

   public:
    Settings();

   public:
    void zero_offset_boxes();

    void reset_scale_box();
};

class Graph_demo : public ox::HPair<Graph_core, ox::Bordered<Settings>> {
   public:
    Graph_core& core = this->first;
    Settings& settings =
        this->second | ox::pipe::take_west() | ox::pipe::wrapped();

   public:
    Graph_demo();

   private:
    /// Sets the generator and resets settings boxes to zeros.
    template <typename Generator_t, typename... Args>
    void set_generator(Args&&... args);
};

}  // namespace graph
#endif  // TERMOX_DEMOS_GRAPH_GRAPH_DEMO_HPP
