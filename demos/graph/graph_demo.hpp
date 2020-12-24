#ifndef TERMOX_DEMOS_GRAPH_GRAPH_DEMO_HPP
#define TERMOX_DEMOS_GRAPH_GRAPH_DEMO_HPP
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/graph.hpp>
#include <termox/widget/widgets/number_edit.hpp>

namespace graph {

// TODO graph's template parameter? double everywhere so far.

class Graph_generator {
   public:
    virtual ~Graph_generator() = default;

   public:
    /// Return points to display in Graph.
    virtual auto generate() -> std::vector<ox::Graph<>::Coordinates> = 0;

    /// Increment internal state.
    virtual void step_forward() = 0;

    /// Decrement internal state.
    virtual void step_backward() = 0;

    /// Retrieve the default boundary dimensions this should be displayed with.
    virtual auto default_boundary() const -> ox::Graph<>::Boundary = 0;
};

class Sine final : public Graph_generator {
   public:
    auto generate() -> std::vector<ox::Graph<>::Coordinates> override
    {
        auto result = std::vector<ox::Graph<>::Coordinates>{};
        for (auto i = -3.14; i <= 3.14; i += 0.001)
            result.push_back({i, std::sin(i * t_)});
        return result;
    }

    void step_forward() override { t_ += 0.01; }

    void step_backward() override { t_ -= 0.01; }

    auto default_boundary() const -> ox::Graph<>::Boundary override
    {
        return {-3.14, 3.14, -1, 1};
    }

   private:
    double t_{0.};
};

class Sine_three final : public Graph_generator {
   public:
    auto generate() -> std::vector<ox::Graph<>::Coordinates> override
    {
        auto result = std::vector<ox::Graph<>::Coordinates>{};
        for (auto i = -3.14; i <= 3.14; i += 0.001) {
            result.push_back({i, std::sin(i * 40) + std::sin(i * t_ * 0.5) +
                                     std::sin(i * t_)});
        }
        return result;
    }

    void step_forward() override { t_ += 0.01; }

    void step_backward() override { t_ -= 0.01; }

    auto default_boundary() const -> ox::Graph<>::Boundary override
    {
        return {-3.14, 3.14, -2, 2};
    }

   private:
    double t_{0.};
};

class Sine_phase final : public Graph_generator {
   public:
    auto generate() -> std::vector<ox::Graph<>::Coordinates> override
    {
        auto static_sine = std::vector<ox::Graph<>::Coordinates>{};
        auto moving_sine = std::vector<ox::Graph<>::Coordinates>{};
        auto combination = std::vector<ox::Graph<>::Coordinates>{};
        for (auto i = 2 * -3.14; i <= 2 * 3.14; i += 0.001) {
            static_sine.push_back({i, std::sin(i - (a_ * 0.25))});
            moving_sine.push_back({i, std::sin(i + a_)});
            combination.push_back(
                {i, static_sine.back().y + moving_sine.back().y});
        }
        combination.insert(std::end(combination), std::begin(static_sine),
                           std::end(static_sine));
        combination.insert(std::end(combination), std::begin(moving_sine),
                           std::end(moving_sine));
        return combination;
    }

    void step_forward() override { a_ += 0.01; }

    void step_backward() override { a_ -= 0.01; }

    auto default_boundary() const -> ox::Graph<>::Boundary override
    {
        return {2 * -3.14, 2 * 3.14, -2, 2};
    }

   private:
    double a_{0.};
};

class Circle final : public Graph_generator {
   public:
    auto generate() -> std::vector<ox::Graph<>::Coordinates> override
    {
        return implicit_points(
            [](double x, double y) {
                // return std::pow(x, 2.) + std::pow(y, 2.) - radius_;
                // Biology
                // return std::sin(std::sin(x) + std::cos(y)) -
                //        std::cos(std::sin(x * y) + std::cos(x));
                // Dizzy
                return std::abs(std::sin(std::pow(x, 2) - std::pow(y, 2))) -
                       (std::sin(x + y) + std::cos(x * y));
            },
            this->default_boundary(), 0.01);
    }

    void step_forward() override
    {
        if (radius_ <= 0.1 || radius_ > 1.)
            direction_ = -direction_;
        radius_ += (0.01 * direction_);
    }

    void step_backward() override
    {
        // radius_ -= 0.01;
    }

    auto default_boundary() const -> ox::Graph<>::Boundary override
    {
        // return {-1.1, 1.1, -1.1, 1.1};
        return {-10.1, 10.1, -10.1, 10.1};
    }

   private:
    double radius_{1.};
    double direction_{-1.};

   private:
    /// Function_t => auto f(double x, double y) -> double
    /// If f(x, y) is nearly zero, then it is a point.
    template <typename Function_t>
    static auto implicit_points(Function_t f,
                                ox::Graph<>::Boundary b,
                                double interval)
        -> std::vector<ox::Graph<>::Coordinates>
    {
        auto result = std::vector<ox::Graph<>::Coordinates>{};
        for (auto x = b.west; x < b.east; x += interval) {
            for (auto y = b.north; y < b.south; y += interval) {
                if (nearly_zero(f(x, y), interval))
                    result.push_back({x, y});
            }
        }
        return result;
    }

    static auto nearly_zero(double zero, double threshold) -> bool
    {
        return std::abs(zero) <= threshold;
    }
};

class Graph_core : public ox::Graph<double> {
   public:
    sl::Signal<void(Graph::Boundary)> boundary_changed;

   public:
    template <typename Generator_t = Sine, typename... Args>
    explicit Graph_core(Args&&... args)
    {
        this->set_generator<Generator_t>(std::forward<Args>(args)...);
        // this->enable_animation(std::chrono::milliseconds{16});
        this->brush.foreground = ox::Color::Light_green;
    }

    template <typename Generator_t, typename... Args>
    void set_generator(Args&&... args)
    {
        generator_ = std::make_unique<Generator_t>(std::forward<Args>(args)...);
        this->set_boundary(generator_->default_boundary());
        this->Graph::clear();
        this->Graph::add(generator_->generate());
    }

   protected:
    auto timer_event() -> bool override
    {
        generator_->step_forward();
        this->Graph::clear();
        this->Graph::add(generator_->generate());
        return Graph::timer_event();
    }

   private:
    std::unique_ptr<Graph_generator> generator_{nullptr};

   private:
    void set_boundary(Boundary const& b)
    {
        this->Graph::set_boundary(b);
        boundary_changed(b);
    }
};

class Graph_demo : public ox::layout::Horizontal<> {
   private:
    struct Settings : ox::layout::Vertical<> {
       public:
        sl::Signal<void(std::string)> graph_changed;
        sl::Signal<void(int)> step_interval_changed;

        ox::Labeled_cycle_box& graph_box =
            make_child<ox::Labeled_cycle_box>("Graph");
        ox::Labeled_number_edit<int>& step_interval_box =
            make_child<ox::Labeled_number_edit<int>>("StepInterval(ms)", 16);

       public:
        Settings()
        {
            this->width_policy.fixed(25);
            this->border.enable();
            this->border.segments.disable_all();
            this->border.segments.west.enable();

            graph_box.cycle_box.add_option("Sine").connect(
                [this] { graph_changed("sine"); });
            graph_box.cycle_box.add_option("Sine Three").connect([this] {
                graph_changed("sine-three");
            });
            graph_box.cycle_box.add_option("Sine Phase").connect([this] {
                graph_changed("sine-phase");
            });
            graph_box.cycle_box.add_option("Circle").connect(
                [this] { graph_changed("circle"); });
            step_interval_box.number_edit.value_set.connect(
                [this](int interval) { step_interval_changed(interval); });
        }
    };

   public:
    Graph_core& core_   = this->make_child<Graph_core>();
    Settings& settings_ = this->make_child<Settings>();

   public:
    Graph_demo()
    {
        settings_.graph_changed.connect([this](std::string gen) {
            if (gen == "sine")
                core_.set_generator<Sine>();
            else if (gen == "sine-three")
                core_.set_generator<Sine_three>();
            else if (gen == "sine-phase")
                core_.set_generator<Sine_phase>();
            else if (gen == "circle")
                core_.set_generator<Circle>();
        });
        settings_.step_interval_changed.connect([this](int interval) {
            core_.Widget::disable_animation();
            core_.Widget::enable_animation(std::chrono::milliseconds{interval});
        });
    }
};

}  // namespace graph
#endif  // TERMOX_DEMOS_GRAPH_GRAPH_DEMO_HPP
