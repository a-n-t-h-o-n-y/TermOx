#include "phase_sim.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <ox/ox.hpp>
#include <sstream>
#include <string>

using namespace ox;
using namespace std::chrono_literals;
using phase::Light;
using phase::Vec;

namespace {
constexpr RGB background{0x080c18};
constexpr RGB foreground{0xb8c9dc};
enum class Kind { Orbit, Bloom, Loom, Flock };
constexpr std::array<std::string_view, 4> hints{
    "Drag to launch · click to inspect · right-click to remove · wheel to zoom",
    "Drag to seed · right-drag to erase · wheel changes brush size",
    "Drag to move the center · wheel changes speed · right-click clears the drawing",
    "Move to guide · click to place an obstacle · right-click to remove · wheel "
    "scatters"};

// Smooth additive trails, resolved to a colored Braille glyph in each terminal cell.
class Trails {
   public:
    int width = 0, height = 0;
    double aspect = 1;
    std::vector<Light> pixels;
    DotGrid dots;
    void resize(Area cells)
    {
        if (cells.width * 2 == width && cells.height * 4 == height) return;
        width = cells.width * 2;
        height = cells.height * 4;
        aspect = height ? (double)width / height : 1;
        pixels.assign((std::size_t)width * height, {});
        dots.reset({width, height});
    }
    void clear() { std::fill(pixels.begin(), pixels.end(), Light{}); }
    void fade(double multiplier)
    {
        for (auto& p : pixels)
            p = p * multiplier;
    }
    auto screen(Vec p) const -> Vec
    { return {(p.x / std::max(.01, aspect) + 1) * width / 2, (p.y + 1) * height / 2}; }
    void point(Vec p, Light color, double strength = 1)
    {
        p = screen(p);
        if (!phase::finite(p) || p.x < -3 || p.x > width + 3 || p.y < -3 ||
            p.y > height + 3)
            return;
        int x = (int)std::floor(p.x), y = (int)std::floor(p.y);
        for (int yy = y - 1; yy <= y + 1; ++yy)
            for (int xx = x - 1; xx <= x + 1; ++xx) {
                if (xx < 0 || yy < 0 || xx >= width || yy >= height) continue;
                double dx = xx + .5 - p.x, dy = yy + .5 - p.y;
                double weight = std::exp(-(dx * dx + dy * dy) * 1.3) * strength;
                auto& out = pixels[(std::size_t)yy * width + xx];
                out = out + color * weight;
                out.r = std::min(5., out.r);
                out.g = std::min(5., out.g);
                out.b = std::min(5., out.b);
            }
    }
    void line(Vec a, Vec b, Light color, double strength = 1)
    {
        if (!phase::finite(a) || !phase::finite(b)) return;
        // Clip before sampling: launches outside the viewport never create unbounded
        // work.
        Vec d = b - a;
        double lo = 0, hi = 1;
        auto clip = [&](double p, double q) {
            if (p == 0) return q >= 0;
            double r = q / p;
            if (p < 0)
                lo = std::max(lo, r);
            else
                hi = std::min(hi, r);
            return lo <= hi;
        };
        if (!clip(-d.x, a.x + aspect) || !clip(d.x, aspect - a.x) ||
            !clip(-d.y, a.y + 1) || !clip(d.y, 1 - a.y))
            return;
        b = a + d * hi;
        a = a + d * lo;
        int steps = std::max(1, (int)std::ceil((screen(b) - screen(a)).length()));
        for (int i = 0; i <= steps; ++i)
            point(a + (b - a) * ((double)i / steps), color, strength);
    }
    static auto rgb(Light light) -> RGB
    {
        auto channel = [](double x) {
            return (std::uint8_t)std::clamp(255 * (1 - std::exp(-x)), 0., 255.);
        };
        return {channel(light.r), channel(light.g), channel(light.b)};
    }
    void paint(Canvas canvas)
    {
        for (int cy = 0; cy < height / 4; ++cy)
            for (int cx = 0; cx < width / 2; ++cx) {
                Light peak;
                double brightest = 0;
                for (int y = 0; y < 4; ++y)
                    for (int x = 0; x < 2; ++x) {
                        auto p = pixels[(std::size_t)(cy * 4 + y) * width + cx * 2 + x];
                        double energy = std::max({p.r, p.g, p.b});
                        if (energy > brightest) {
                            brightest = energy;
                            peak = p;
                        }
                    }
                for (int y = 0; y < 4; ++y)
                    for (int x = 0; x < 2; ++x) {
                        auto p = pixels[(std::size_t)(cy * 4 + y) * width + cx * 2 + x];
                        dots.set_dot(
                            {cx * 2 + x, cy * 4 + y},
                            std::max({p.r, p.g, p.b}) > std::max(.06, brightest * .32));
                    }
                auto bg = background;
                auto fg = rgb(peak * 2.8 + Light{.03, .04, .06});
                dots.set_cell_colors({cx, cy}, {.foreground = fg, .background = bg});
            }
        dots.paint(canvas);
    }
};

class Scene : public Widget {
   public:
    using Widget::handle;
    Kind kind;
    phase::Orbit orbit;
    phase::Bloom bloom;
    phase::Loom loom;
    phase::Flock flock;
    std::array<double, 3> knobs{.5, .5, .5};
    int preset = 0;
    bool paused = false;
    double elapsed = 0;
    sl::Signal<void()> changed;

    explicit Scene(Kind type) : Widget{FocusPolicy::Strong}, kind{type}
    {
        if (kind == Kind::Orbit) knobs[2] = .85;
        if (kind == Kind::Loom) knobs[2] = .8;
        reset();
    }
    void reset()
    {
        elapsed = 0;
        chemistry_ = 0;
        zoom_ = 1;
        speed_ = 1;
        drag_ = false;
        release_pointer();
        trails_.clear();
        switch (kind) {
            case Kind::Orbit:
                orbit.reset(preset);
                sync_mass();
                break;
            case Kind::Bloom: bloom.reset(preset); break;
            case Kind::Loom: loom.reset(preset); break;
            case Kind::Flock: flock.reset(preset, aspect()); break;
        }
        last_ = std::chrono::steady_clock::now();
        request_repaint();
        changed();
    }
    void theme_changed() override
    {
        // Stack detaches inactive pages, stopping their timers. Resume the saved
        // playback state when this page is attached again, without catching up.
        last_ = std::chrono::steady_clock::now();
        if (!paused) timer_.start();
    }
    void pause(bool value)
    {
        paused = value;
        if (paused)
            timer_.stop();
        else {
            last_ = std::chrono::steady_clock::now();
            timer_.start();
        }
        changed();
        request_repaint();
    }
    void parameter(int index, double value)
    {
        if (knobs[index] == value) return;
        knobs[index] = value;
        if (kind == Kind::Orbit && index == 1 && orbit.selected >= 0)
            orbit.bodies[orbit.selected].mass = .02 * std::pow(2000., value);
        request_repaint();
        changed();
    }
    auto description() const -> std::string
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2);
        if (kind == Kind::Orbit) {
            out << orbit.bodies.size() << " bodies";
            if (orbit.selected >= 0)
                out << " · mass " << orbit.bodies[orbit.selected].mass;
        }
        else if (kind == Kind::Bloom)
            out << "brush " << (int)(2 + knobs[2] * 12);
        else if (kind == Kind::Loom)
            out << "ratio " << ratio() << " · " << speed_ << "x";
        else
            out << flock.birds.size() << " birds · " << flock.obstacles.size()
                << " obstacles";
        if (paused) out << " · paused";
        return out.str();
    }
    void advance(double dt)
    {
        dt = std::clamp(dt, 0., .06);
        auto previous_time = elapsed;
        elapsed += dt;
        if (!size().width || !size().height) return;
        if (kind == Kind::Orbit) {
            trails_.fade(std::exp(-dt * (2.8 - knobs[2] * 2.65)));
            auto before = orbit.bodies;
            orbit.step(dt, .025 + knobs[0] * .15);
            sync_mass();
            for (std::size_t i = 0; i < orbit.bodies.size(); ++i) {
                auto& b = orbit.bodies[i];
                auto color = phase::spectrum(b.hue);
                if (before.size() == orbit.bodies.size())
                    trails_.line(before[i].p * zoom_, b.p * zoom_, color, .65);
                trails_.point(b.p * zoom_, color, 1.3);
            }
        }
        else if (kind == Kind::Bloom) {
            if (drag_) brush(pointer_, erase_);
            // A bounded accumulator gives the same chemistry rate at different frame
            // rates.
            chemistry_ += dt * (120 + knobs[0] * 480);
            int iterations = (int)chemistry_;
            chemistry_ -= iterations;
            bloom.step(iterations, (knobs[1] - .5) * .012);
        }
        else if (kind == Kind::Loom) {
            trails_.fade(std::exp(-dt * (.008 + std::pow(1 - knobs[2], 3) * .3)));
            double end = loom.time + dt * speed_ * 2.4;
            auto previous = loom.arms(loom.time, ratio(), reach())[2];
            for (int i = 1; i <= 100; ++i) {
                double t = loom.time + (end - loom.time) * i / 100;
                auto p = loom.arms(t, ratio(), reach())[2];
                trails_.line(
                    previous, p, phase::spectrum(t * .045 + .52),
                    .24 * std::min(
                              1.,
                              (trails_.screen(p) - trails_.screen(previous)).length()));
                previous = p;
            }
            loom.time = end;
        }
        else {
            trails_.fade(std::exp(-dt * 4.5));
            auto previous = flock.birds;
            flock.step(dt, aspect(), .25 + knobs[0] * 2, .3 + knobs[1] * 2.5,
                       .4 + knobs[2] * 1.7, world(pointer_), guided_);
            for (std::size_t i = 0; i < flock.birds.size(); ++i) {
                auto b = flock.birds[i];
                auto color = phase::spectrum(.43 + b.hue * .42 +
                                             std::atan2(b.v.y, b.v.x) * .035);
                if ((b.p - previous[i].p).length() < .2)
                    trails_.line(previous[i].p, b.p, color, .27);
                trails_.point(b.p, color, .8);
            }
        }
        if ((int)(elapsed * 4) != (int)(previous_time * 4)) changed();
        request_repaint();
    }
    auto tick() -> EventResponse
    {
        auto now = std::chrono::steady_clock::now();
        double dt = std::chrono::duration<double>(now - last_).count();
        last_ = now;
        if (!paused) advance(dt);
        return {.repaint = !paused};
    }
    auto paint(Canvas canvas) -> Cursor override
    {
        if (kind == Kind::Bloom)
            paint_bloom(canvas);
        else {
            trails_.paint(canvas);
            if (kind == Kind::Orbit) {
                for (int i = 0; i < (int)orbit.bodies.size(); ++i) {
                    auto b = orbit.bodies[i];
                    auto p = cell(b.p * zoom_);
                    auto style =
                        Style{.background = background,
                              .foreground = Trails::rgb(phase::spectrum(b.hue) * 3)};
                    put(canvas, p, b.mass > 2 ? U"●" : U"•", style);
                    if (i == orbit.selected) {
                        put(canvas, {p.x - 1, p.y}, U"‹", style);
                        put(canvas, {p.x + 1, p.y}, U"›", style);
                    }
                }
                if (drag_) {
                    auto a = world(start_), b = world(pointer_);
                    for (int i = 0; i < 24; ++i)
                        put(canvas, cell(a + (b - a) * (i / 24.)), U"·",
                            {.foreground = RGB{0x96aeca}});
                    put(canvas, cell(b), U"+", {.foreground = RGB{0xffffff}});
                }
            }
            else if (kind == Kind::Loom) {
                auto arms = loom.arms(loom.time, ratio(), reach());
                put(canvas, cell(arms[2]), U"•", {.foreground = RGB{0xffffff}});
                put(canvas, cell(loom.center), U"·", {.foreground = RGB{0x65758b}});
            }
            else {
                for (auto p : flock.obstacles) {
                    auto c = cell(p);
                    put(canvas, c, U"◌", {.foreground = RGB{0xd9a76c}});
                }
            }
        }
        return {};
    }
    auto handle(KeyEvent const& e) -> EventResponse override
    {
        if (e.action == KeyAction::Release) return {};
        if (e.matches(U' ', {}, e.action))
            pause(!paused);
        else if (e.matches(U'r', {}, e.action))
            reset();
        else if (e.matches(U'.', {}, e.action)) {
            pause(true);
            advance(1. / 30);
        }
        else
            return {};
        return {.handled = true, .repaint = true};
    }
    auto handle(MousePress const& e) -> EventResponse override
    {
        pointer_ = e.at;
        if (e.button == MouseButton::Right) {
            if (kind == Kind::Orbit) {
                orbit.remove(orbit.nearest(world(e.at) / zoom_, .12 / zoom_));
                sync_mass();
            }
            else if (kind == Kind::Loom)
                trails_.clear();
            else if (kind == Kind::Flock) {
                auto p = world(e.at);
                auto found = std::ranges::min_element(
                    flock.obstacles, {}, [p](Vec v) { return (v - p).length2(); });
                if (found != flock.obstacles.end() && (*found - p).length() < .2)
                    flock.obstacles.erase(found);
            }
            else {
                drag_ = true;
                erase_ = true;
                brush(e.at, true);
                capture_pointer(MouseButton::Right);
            }
        }
        else if (e.button == MouseButton::Left) {
            drag_ = true;
            erase_ = false;
            start_ = e.at;
            capture_pointer();
            if (kind == Kind::Bloom) brush(e.at, false);
            if (kind == Kind::Loom) loom.center = world(e.at);
            if (kind == Kind::Flock) {
                if (flock.obstacles.size() < 24) flock.obstacles.push_back(world(e.at));
                drag_ = false;
                release_pointer();
            }
        }
        else
            return {};
        request_repaint();
        changed();
        return {.handled = true, .repaint = true};
    }
    auto handle(MouseMove const& e) -> EventResponse override
    {
        pointer_ = e.at;
        guided_ = true;
        if (drag_ && kind == Kind::Bloom) brush(e.at, erase_);
        if (drag_ && kind == Kind::Loom) loom.center = world(e.at);
        return {.handled = true, .repaint = drag_};
    }
    auto handle(MouseRelease const& e) -> EventResponse override
    {
        if (!drag_) return {};
        if (kind == Kind::Orbit) {
            auto origin = world(start_) / zoom_;
            auto delta = (world(e.at) - world(start_)) / zoom_;
            auto found = orbit.nearest(origin, .10 / zoom_);
            if (delta.length() < .04 / zoom_ && found >= 0)
                orbit.selected = found;
            else
                orbit.launch(origin, delta * 3, .2);
            sync_mass();
        }
        drag_ = false;
        release_pointer();
        changed();
        request_repaint();
        return {.handled = true, .repaint = true};
    }
    auto pointer_cancel() -> EventResponse override
    {
        drag_ = false;
        return {.repaint = true};
    }
    auto mouse_leave() -> EventResponse override
    {
        guided_ = false;
        return {};
    }
    auto handle(MouseWheel const& e) -> EventResponse override
    {
        double amount = std::clamp(-e.dy, -4, 4);
        if (kind == Kind::Orbit) {
            zoom_ = std::clamp(zoom_ * std::pow(1.15, amount), .2, 5.);
            trails_.clear();
        }
        if (kind == Kind::Loom)
            speed_ = std::clamp(speed_ * std::pow(1.15, amount), .1, 5.);
        if (kind == Kind::Bloom) knobs[2] = std::clamp(knobs[2] + amount * .05, 0., 1.);
        if (kind == Kind::Flock)
            for (auto& b : flock.birds)
                b.v = phase::unit(b.p - world(e.at)) * .9;
        changed();
        request_repaint();
        return {.handled = true, .repaint = true};
    }
    auto healthy() const -> bool
    {
        for (auto b : orbit.bodies)
            if (!phase::finite(b.p) || !phase::finite(b.v) || !std::isfinite(b.mass) ||
                b.mass <= 0)
                return false;
        for (auto b : flock.birds)
            if (!phase::finite(b.p) || !phase::finite(b.v)) return false;
        for (auto c : bloom.cells)
            if (!std::isfinite(c.u) || !std::isfinite(c.v) || c.u < 0 || c.u > 1 ||
                c.v < 0 || c.v > 1)
                return false;
        return true;
    }

   protected:
    void on_arrange(Rect) override
    {
        trails_.resize(size());
        if (kind == Kind::Flock && !flock_sized_ && size().width && size().height) {
            flock.reset(preset, aspect());
            flock_sized_ = true;
        }
        if (kind == Kind::Flock)
            for (auto& b : flock.birds)
                b.p.x = std::clamp(b.p.x, -aspect(), aspect());
    }

   private:
    Trails trails_;
    Timer timer_{*this, 33ms, [](Scene& scene) { return scene.tick(); }, true};
    std::chrono::steady_clock::time_point last_ = std::chrono::steady_clock::now();
    bool drag_ = false, erase_ = false, guided_ = false, flock_sized_ = false;
    Point start_{}, pointer_{};
    double zoom_ = 1, speed_ = 1, chemistry_ = 0;
    auto aspect() const -> double
    { return std::max(.1, size().width / std::max(2., 2. * size().height)); }
    auto world(Point p) const -> Vec
    {
        return {((p.x + .5) / std::max(1, size().width) * 2 - 1) * aspect(),
                (p.y + .5) / std::max(1, size().height) * 2 - 1};
    }
    auto cell(Vec p) const -> Point
    {
        return {
            (int)std::clamp((p.x / aspect() + 1) * size().width / 2, -10000., 10000.),
            (int)std::clamp((p.y + 1) * size().height / 2, -10000., 10000.)};
    }
    auto ratio() const -> double
    {
        return std::array{2.618033988, 3.141592654, 1.414213562}[preset] +
               (knobs[0] - .5) * 8;
    }
    auto reach() const -> double { return (.5 + knobs[1]) * std::min(1., aspect()); }
    void sync_mass()
    {
        if (orbit.selected >= 0)
            knobs[1] = std::clamp(
                std::log(orbit.bodies[orbit.selected].mass / .02) / std::log(2000.), 0.,
                1.);
    }
    void brush(Point p, bool erase)
    {
        bloom.seed((p.x + .5) / std::max(1, size().width),
                   (p.y + .5) / std::max(1, size().height), 2 + knobs[2] * 12, erase);
    }
    void paint_bloom(Canvas canvas)
    {
        auto extent = Area{canvas.size().width, canvas.size().height * 2};
        std::vector<RGB> pixels(pixel_count(extent), background);
        for (int y = 0; y < extent.height; ++y)
            for (int x = 0; x < extent.width; ++x) {
                double u = (x + .5) / std::max(1, extent.width),
                       v = (y + .5) / std::max(1, extent.height);
                double density = bloom.sample(u, v);
                double dx = bloom.sample(u + .005, v) - bloom.sample(u - .005, v);
                double dy = bloom.sample(u, v + .008) - bloom.sample(u, v - .008);
                double shade = std::clamp(.8 + (dx - dy) * 5, .25, 1.6);
                auto ink = phase::spectrum(.56 + density * 1.4 + preset * .12);
                auto color = Light{.025, .036, .075} + ink * (density * 4.5 * shade);
                pixels[(std::size_t)y * extent.width + x] = Trails::rgb(color);
            }
        paint_pixels(canvas, extent, pixels);
    }
};

constexpr std::array<std::array<std::string_view, 3>, 4> presets{
    {{"Solar", "Binary", "Cluster"},
     {"Coral", "Maze", "Spots"},
     {"Rosette", "Knot", "Ribbon"},
     {"Murmuration", "Current", "Shoal"}}};
constexpr std::array<std::array<std::string_view, 3>, 4> parameter_names{
    {{"Gravity", "Mass", "Trails"},
     {"Growth", "Feed", "Brush"},
     {"Ratio", "Reach", "Trails"},
     {"Cohere", "Separate", "Speed"}}};

auto make_page(Kind kind)
{
    int index = (int)kind;
    auto choices = std::vector<ListItem>{};
    for (auto name : presets[index])
        choices.push_back({std::string{name}});
    auto page = Column{
        Scene{kind},
        Row{Button{{.label = {.text = "Pause"}}}, Button{{.label = {.text = "Step"}}},
            Button{{.label = {.text = "Reset"}}},
            Select{{.items = std::move(choices), .selected = 0}},
            Label{{.align = Align::Right, .style = {.foreground = RGB{0x7d93ad}}}}},
        Row{Label{std::string{parameter_names[index][0]}},
            Slider{{.range = {0, 1, .025}, .value = .5}},
            Label{std::string{parameter_names[index][1]}},
            Slider{{.range = {0, 1, .025}, .value = .5}},
            Label{std::string{parameter_names[index][2]}},
            Slider{{.range = {0, 1, .025}, .value = .5}}},
        Label{{.text = std::string{hints[index]},
               .align = Align::Left,
               .style = {.foreground = RGB{0x60758d}}}},
    };
    auto& [scene, actions, parameters, help] = page.children;
    auto& [pause, step, reset, select, status] = actions.children;
    actions.height_policy = SizePolicy::fixed(1);
    actions.gap = 1;
    parameters.height_policy = SizePolicy::fixed(1);
    parameters.gap = 1;
    pause.width_policy = SizePolicy::fixed(10);
    step.width_policy = SizePolicy::fixed(8);
    reset.width_policy = SizePolicy::fixed(9);
    select.width_policy = SizePolicy::fixed(16);
    get_child<0>(parameters).width_policy = SizePolicy::fixed(7);
    get_child<2>(parameters).width_policy = SizePolicy::fixed(8);
    get_child<4>(parameters).width_policy = SizePolicy::fixed(6);
    status.set_text(scene.description());
    get_child<3>(parameters).set_value(scene.knobs[1]);
    get_child<5>(parameters).set_value(scene.knobs[2]);
    Connection{.signal = pause.on_activate,
               .slot = [](Scene& s) { s.pause(!s.paused); }}(scene);
    Connection{.signal = step.on_activate, .slot = [](Scene& s) {
                   s.pause(true);
                   s.advance(1. / 30);
               }}(scene);
    Connection{.signal = reset.on_activate, .slot = [](Scene& s) { s.reset(); }}(scene);
    Connection{.signal = select.on_change, .slot = [](auto index, Scene& s) {
                   if (index) {
                       s.preset = (int)*index;
                       s.reset();
                   }
               }}(scene);
    Connection{.signal = get_child<1>(parameters).on_change,
               .slot = [](double value, Scene& s) { s.parameter(0, value); }}(scene);
    Connection{.signal = get_child<3>(parameters).on_change,
               .slot = [](double value, Scene& s) { s.parameter(1, value); }}(scene);
    Connection{.signal = get_child<5>(parameters).on_change,
               .slot = [](double value, Scene& s) { s.parameter(2, value); }}(scene);
    Connection{
        .signal = scene.changed,
        .slot = [](Scene& s, Label& text, Button& button, Slider& brush, Slider& mass) {
            text.set_text(s.description());
            auto caption = s.paused ? "Resume" : "Pause";
            if (button.label.text != caption) {
                button.label.text = caption;
                button.refresh();
            }
            if (s.kind == Kind::Bloom) brush.set_value(s.knobs[2]);
            if (s.kind == Kind::Orbit) mass.set_value(s.knobs[1]);
        }}(scene, status, pause, get_child<5>(parameters), get_child<3>(parameters));
    return page;
}

void require(bool value, std::string_view message)
{
    if (!value) throw std::runtime_error{std::string{message}};
}

void check_models()
{
    auto orbit = phase::Orbit{};
    orbit.reset(1);
    auto momentum = [](auto const& bodies) {
        Vec result;
        for (auto b : bodies)
            result = result + b.v * b.mass;
        return result;
    };
    auto before = momentum(orbit.bodies);
    for (int i = 0; i < 300; ++i)
        orbit.step(1. / 30, .1);
    require((momentum(orbit.bodies) - before).length() < 1e-8, "Orbit lost momentum");
    orbit.bodies = {{{0, 0}, {1, 0}, 1, 0}, {{.001, 0}, {-1, 0}, 1, 0}};
    orbit.step(0, .1);
    require(orbit.bodies.size() == 1 && orbit.bodies[0].mass == 2 &&
                orbit.bodies[0].v.length() == 0,
            "Orbit collision did not conserve mass/momentum");
    for (int i = 0; i < 60; ++i)
        orbit.launch({i * .1, 0}, {}, .2);
    require(orbit.bodies.size() == 48, "Orbit body limit");
    auto bloom = phase::Bloom{};
    bloom.reset(0);
    double before_v = bloom.sample(.5, .5);
    bloom.seed(.5, .5, 5);
    require(bloom.sample(.5, .5) > before_v, "Bloom brush did not deposit reagent");
    bloom.seed(.5, .5, 5, true);
    require(bloom.sample(.5, .5) == 0, "Bloom eraser failed");
    bloom.step(50, 0);
    for (auto c : bloom.cells)
        require(std::isfinite(c.u) && std::isfinite(c.v), "Bloom unstable");
}

void write_json(Frame const& frame)
{
    auto color = [](Color c) {
        if (auto rgb = std::get_if<RGB>(&c))
            return std::to_string(rgb->red) + "," + std::to_string(rgb->green) + "," +
                   std::to_string(rgb->blue);
        return std::string{};
    };
    std::cout << "{\"width\":" << frame.size().width
              << ",\"height\":" << frame.size().height << ",\"rows\":[";
    for (int y = 0; y < frame.size().height; ++y) {
        if (y) std::cout << ',';
        std::cout << '[';
        for (int x = 0; x < frame.size().width; ++x) {
            if (x) std::cout << ',';
            auto const& c = frame[{x, y}];
            std::cout << "{\"text\":" << std::quoted(esc::utf32_to_utf8(c.text))
                      << ",\"fg\":" << std::quoted(color(c.style.foreground))
                      << ",\"bg\":" << std::quoted(color(c.style.background))
                      << ",\"columns\":" << c.columns << '}';
        }
        std::cout << ']';
    }
    std::cout << "]}\n";
}
}  // namespace

int main(int argc, char** argv)
try {
    bool check = false, json = false;
    int scene_index = 0, frames = 180, width = 120, height = 40;
    for (int i = 1; i < argc; ++i) {
        auto arg = std::string_view{argv[i]};
        auto number = [&] {
            if (++i == argc) throw std::invalid_argument{"Missing option value"};
            std::string text = argv[i];
            std::size_t end = 0;
            int value = std::stoi(text, &end);
            if (end != text.size()) throw std::invalid_argument{"Invalid integer"};
            return value;
        };
        if (arg == "--check")
            check = true;
        else if (arg == "--json")
            json = true;
        else if (arg == "--scene")
            scene_index = number();
        else if (arg == "--frames")
            frames = number();
        else if (arg == "--width")
            width = number();
        else if (arg == "--height")
            height = number();
        else if (arg == "--help") {
            std::cout << "Phase: Orbit, Bloom, Loom, Flock\n"
                      << "Click tabs or use Ctrl-Tab / Shift-Ctrl-Tab. Tab moves "
                         "between controls.\n"
                      << "On the canvas: Space pauses, R resets, period steps. Ctrl-C "
                         "exits.\n"
                      << "phase [--check] [--json --scene 0..3 --frames N --width W "
                         "--height H]\n";
            return 0;
        }
        else
            throw std::invalid_argument{"Unknown option"};
    }
    if (scene_index < 0 || scene_index > 3 || frames < 0 || frames > 10000 ||
        width < 0 || width > 500 || height < 0 || height > 200)
        throw std::invalid_argument{"Option out of range"};
    auto root = Tabs{std::tuple{make_page(Kind::Orbit), make_page(Kind::Bloom),
                                make_page(Kind::Loom), make_page(Kind::Flock)},
                     std::vector<std::string>{"Orbit", "Bloom", "Loom", "Flock"}};
    root.show_separator = false;
    root.focus_policy = FocusPolicy::Click;
    root.selected_style = {.background = RGB{0x163342},
                           .foreground = RGB{0x9ce8da},
                           .enabled = Trait::Bold,
                           .disabled = Trait::Inverse};
    auto app = Application{root};
    auto theme = Theme::dark();
    theme.palette.background = background;
    theme.palette.surface = RGB{0x111d2d};
    theme.palette.foreground = foreground;
    theme.palette.accent = RGB{0x74d7c4};
    theme.palette.selection = RGB{0x234452};
    theme.motion.enabled = false;
    app.set_theme(theme);
    std::array<Scene*, 4> scenes{&get_child<0>(std::get<0>(root.pages.children)),
                                 &get_child<0>(std::get<1>(root.pages.children)),
                                 &get_child<0>(std::get<2>(root.pages.children)),
                                 &get_child<0>(std::get<3>(root.pages.children))};
    if (check || json) {
        for (auto* s : scenes)
            s->pause(true);
        app.dispatch(esc::Resize{{width, height}});
        if (check) {
            check_models();
            for (int i = 0; i < 4; ++i) {
                root.select(i);
                app.set_focus(*scenes[i]);
                for (int p = 0; p < 3; ++p) {
                    scenes[i]->preset = p;
                    scenes[i]->reset();
                    for (int n = 0; n < 60; ++n)
                        scenes[i]->advance(1. / 30);
                    require(scenes[i]->healthy(), "Nonfinite scene state");
                    app.paint();
                }
                auto time = scenes[i]->elapsed;
                root.select((i + 1) % 4);
                app.step();
                root.select(i);
                require(scenes[i]->elapsed == time, "Hidden scene advanced");
                app.set_focus(*scenes[i]);
                app.dispatch(KeyEvent{.key = U'.'});
                require(scenes[i]->elapsed > time && scenes[i]->paused,
                        "Single step failed");
                scenes[i]->handle(
                    MousePress{.at = {25, 12}, .button = MouseButton::Left});
                scenes[i]->handle(
                    MouseMove{.at = {35, 15}, .held_button = MouseButton::Left});
                scenes[i]->handle(
                    MouseRelease{.at = {35, 15}, .button = MouseButton::Left});
                scenes[i]->handle(MouseWheel{.at = {35, 15}, .dy = -1});
                for (auto size :
                     {Area{40, 12}, Area{1, 1}, Area{0, 0}, Area{120, 40}}) {
                    app.dispatch(esc::Resize{size});
                    app.paint();
                }
                require(scenes[i]->healthy(), "Resize or interaction corrupted scene");
            }
            root.select(0);
            scenes[0]->pause(false);
            root.select(1);
            scenes[1]->pause(false);
            double hidden = scenes[0]->elapsed, visible = scenes[1]->elapsed;
            app.step(EventLoop::Clock::now() + 100ms);
            require(scenes[0]->elapsed == hidden, "Hidden timer continued to simulate");
            require(scenes[1]->elapsed > visible, "Visible timer did not simulate");
            root.select(0);
            app.step(EventLoop::Clock::now() + 100ms);
            require(scenes[0]->elapsed > hidden,
                    "Returning to a tab did not resume it");
            for (auto* scene : scenes)
                scene->pause(true);
            std::cout << "Phase: four scenes, twelve presets, physics, brushes, "
                         "pause/step, state retention and resizing passed.\n";
            return 0;
        }
        root.select(scene_index);
        app.set_focus(*scenes[scene_index]);
        for (int n = 0; n < frames; ++n)
            scenes[scene_index]->advance(1. / 30);
        app.paint();
        write_json(app.loop().screen);
        return 0;
    }
    Connection{
        .signal = root.on_select, .slot = [](auto index, auto& tabs) {
            if (!index) return;
            // Tabs handles focus recovery when switching from a page. A click on the
            // strip should also put keyboard shortcuts on the selected canvas.
            int i = 0;
            std::apply(
                [&](auto&... page) {
                    ((i++ == (int)*index ? (void)tabs.request_focus(get_child<0>(page))
                                         : (void)0),
                     ...);
                },
                tabs.pages.children);
        }}(root);
    return app.run({.mouse_mode = MouseMode::Move});
}
catch (std::exception const& error) {
    std::cerr << "phase: " << error.what() << '\n';
    return 1;
}
