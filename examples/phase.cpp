#include "phase_sim.hpp"

#include <chrono>
#include <deque>
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
    "Pull to launch · right-drag to pan · wheel to zoom · Home to recenter · "
    "right-click removes",
    "Drag to seed · right-drag to erase · wheel changes brush size",
    "Drag to move the center · wheel changes speed · right-click clears the drawing",
    "Move to guide · click to place an obstacle · right-click to remove · wheel "
    "scatters"};

// Smooth additive trails, resolved to a colored Braille glyph in each terminal cell.
class Trails {
   public:
    int width = 0, height = 0;
    double aspect = 1;
    struct Cell {
        std::array<Light, 8> pixels{};
        double decay = 0;
        bool active = false;
    };
    std::vector<Cell> cells;
    std::vector<std::size_t> active;
    double decay = 0;
    DotGrid dots;
    // Fade only cells that are touched or displayed. Camera reprojection can replay
    // many historical strokes without rescanning the viewport between each one.
    void touch(Cell& c)
    {
        if (c.decay == decay) return;
        double multiplier = std::exp(c.decay - decay);
        for (auto& p : c.pixels)
            p = p * multiplier;
        c.decay = decay;
    }
    void resize(Area extent)
    {
        if (extent.width * 2 == width && extent.height * 4 == height) return;
        width = extent.width * 2;
        height = extent.height * 4;
        aspect = height ? (double)width / height : 1;
        cells.assign((std::size_t)extent.width * extent.height, {});
        active.clear();
        decay = 0;
        dots.reset({width, height});
    }
    void clear()
    {
        for (auto i : active)
            cells[i] = {};
        active.clear();
        decay = 0;
    }
    void fade(double multiplier) { decay -= std::log(multiplier); }
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
                auto index = (std::size_t)(yy / 4) * (width / 2) + xx / 2;
                auto& c = cells[index];
                if (!c.active) {
                    c.active = true;
                    active.push_back(index);
                }
                touch(c);
                auto& out = c.pixels[(yy % 4) * 2 + xx % 2];
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
        if (!phase::clip_line(a, b, -aspect, aspect, -1, 1)) return;
        int steps = std::max(1, (int)std::ceil((screen(b) - screen(a)).length()));
        for (int i = 0; i <= steps; ++i)
            point(a + (b - a) * ((double)i / steps), color, strength);
    }
    static auto rgb(Light light) -> RGB
    {
        // Interpolated tone curve: less than one 8-bit level of error, with no
        // exponentials in the per-pixel shading loop.
        static auto const curve = [] {
            std::array<double, 4097> values{};
            for (std::size_t i = 0; i < values.size(); ++i)
                values[i] = 255 * (1 - std::exp(-(double)i / 256));
            return values;
        }();
        auto channel = [](double x) {
            x = std::clamp(x * 256, 0., 4096.);
            auto i = (std::size_t)x;
            return (std::uint8_t)(curve[i] * (1 - (x - i)) +
                                  curve[std::min(i + 1, std::size_t{4096})] * (x - i));
        };
        return {channel(light.r), channel(light.g), channel(light.b)};
    }
    void paint(Canvas canvas)
    {
        std::size_t keep = 0;
        for (auto index : active) {
            auto& c = cells[index];
            touch(c);
            Light peak;
            double brightest = 0;
            for (auto p : c.pixels) {
                double energy = std::max({p.r, p.g, p.b});
                if (energy > brightest) {
                    brightest = energy;
                    peak = p;
                }
            }
            if (brightest < .0001) {
                c = {};
                continue;
            }
            active[keep++] = index;
            if (brightest <= .06) continue;
            int cx = (int)(index % (width / 2)), cy = (int)(index / (width / 2));
            for (int y = 0; y < 4; ++y)
                for (int x = 0; x < 2; ++x) {
                    auto p = c.pixels[y * 2 + x];
                    dots.set_dot(
                        {cx * 2 + x, cy * 4 + y},
                        std::max({p.r, p.g, p.b}) > std::max(.06, brightest * .32));
                }
            dots.set_cell_colors({cx, cy},
                                 {.foreground = rgb(peak * 2.8 + Light{.03, .04, .06}),
                                  .background = background});
            // Keep DotGrid's coordinates while clipping its paint to this live cell.
            dots.paint(canvas.subcanvas({cx, cy}, {1, 1})
                           .subcanvas({-cx, -cy}, canvas.size()));
        }
        active.resize(keep);
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

    explicit Scene(Kind type) : Widget{FocusPolicy::Strong}, kind{type} { reset(); }
    void reset()
    {
        elapsed = 0;
        chemistry_ = 0;
        zoom_ = 1;
        camera_ = {};
        history_.clear();
        decay_ = 0;
        panning_ = false;
        orbit_dirty_ = false;
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
            out << orbit.bodies.size() << " bodies · " << zoom_ << "x";
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
            if (orbit_dirty_) redraw_orbit();
            double fade = dt * 2.8 * std::pow(10., 1 - 2 * knobs[2]);
            decay_ += fade;
            trails_.fade(std::exp(-fade));
            auto before = orbit.bodies;
            orbit.step(dt, .025 + knobs[0] * .15);
            sync_mass();
            OrbitTrace trace{decay_, {}};
            for (std::size_t i = 0; i < orbit.bodies.size(); ++i) {
                auto& b = orbit.bodies[i];
                trace.segments.push_back(
                    {before.size() == orbit.bodies.size() ? before[i].p : b.p, b.p,
                     phase::spectrum(b.hue)});
            }
            draw_trace(trace);
            history_.push_back(std::move(trace));
            // Retain even offscreen paths, but bound history by visibility and count.
            while (!history_.empty() &&
                   (decay_ - history_.front().decay > 12 || history_.size() > 4096))
                history_.pop_front();
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
            trails_.fade(
                std::exp(-dt * (std::log(2.) / (.12 * std::pow(50., knobs[2])))));
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
                       .4 * std::pow(20., knobs[2]), world(pointer_), guided_);
            for (std::size_t i = 0; i < flock.birds.size(); ++i) {
                auto b = flock.birds[i];
                auto color = phase::spectrum(.43 + b.hue * .42 +
                                             std::atan2(b.v.y, b.v.x) * .035);
                // Only break the trail when a bird wraps across the viewport.
                if ((b.p - previous[i].p - b.v * dt).length2() < 1e-12)
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
            if (kind == Kind::Orbit && orbit_dirty_) redraw_orbit();
            trails_.paint(canvas);
            if (kind == Kind::Orbit) {
                for (int i = 0; i < (int)orbit.bodies.size(); ++i) {
                    auto b = orbit.bodies[i];
                    auto p = cell(view(b.p));
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
                    auto start = cell(view(anchor_));
                    Vec a{(double)start.x, (double)start.y};
                    Vec b{(double)pointer_.x, (double)pointer_.y};
                    if (phase::clip_line(a, b, 0, size().width - 1., 0,
                                         size().height - 1.)) {
                        int steps = (int)std::ceil(
                            std::max(std::abs(b.x - a.x), std::abs(b.y - a.y)));
                        for (int i = 0; i <= steps; ++i) {
                            auto p = a + (b - a) * ((double)i / std::max(1, steps));
                            put(canvas, {(int)std::lround(p.x), (int)std::lround(p.y)},
                                U"·",
                                {.background = background,
                                 .foreground = RGB{0x96aeca}});
                        }
                    }
                    put(canvas, pointer_, U"+",
                        {.background = background, .foreground = RGB{0xffffff}});
                }
            }
            else if (kind == Kind::Loom) {
                auto arms = loom.arms(loom.time, ratio(), reach());
                put(canvas, cell(arms[2]), U"•",
                    {.background = background, .foreground = RGB{0xffffff}});
                put(canvas, cell(loom.center), U"·",
                    {.background = background, .foreground = RGB{0x65758b}});
            }
            else {
                for (auto p : flock.obstacles) {
                    auto c = cell(p);
                    put(canvas, c, U"◌",
                        {.background = background, .foreground = RGB{0xd9a76c}});
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
        else if (kind == Kind::Orbit && e.matches(KeyCode::Home, {}, e.action)) {
            camera_ = {};
            zoom_ = 1;
            orbit_dirty_ = true;
            changed();
        }
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
        if (drag_ || panning_) return {};
        pointer_ = e.at;
        if (e.button == MouseButton::Right) {
            if (kind == Kind::Orbit) {
                panning_ = true;
                pan_moved_ = false;
                start_ = e.at;
                anchor_ = position(e.at);
                capture_pointer(MouseButton::Right);
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
            anchor_ = position(e.at);
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
        if (panning_) {
            pan_moved_ |= e.at != start_;
            camera_ = anchor_ - world(e.at) / zoom_;
            orbit_dirty_ = true;
        }
        if (drag_ && kind == Kind::Bloom) brush(e.at, erase_);
        if (drag_ && kind == Kind::Loom) loom.center = world(e.at);
        return {.handled = true, .repaint = drag_ || panning_};
    }
    auto handle(MouseRelease const& e) -> EventResponse override
    {
        if (panning_) {
            if (e.button != MouseButton::Right) return {};
            camera_ = anchor_ - world(e.at) / zoom_;
            if (!pan_moved_ && e.at == start_) {
                orbit.remove(orbit.nearest(anchor_, .12 / zoom_));
                sync_mass();
            }
            panning_ = false;
            orbit_dirty_ = true;
            release_pointer();
            changed();
            return {.handled = true, .repaint = true};
        }
        if (!drag_) return {};
        if (kind == Kind::Orbit) {
            auto origin = anchor_;
            auto delta = position(e.at) - origin;
            auto found = orbit.nearest(origin, .10 / zoom_);
            if (delta.length() < .04 / zoom_ && found >= 0)
                orbit.selected = found;
            else
                orbit.launch(origin, delta * -3, .2);
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
        panning_ = false;
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
            auto fixed = position(e.at);
            zoom_ = std::clamp(zoom_ * std::pow(1.04, amount), .1, 10.);
            camera_ = fixed - world(e.at) / zoom_;
            orbit_dirty_ = true;
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
        bool resized =
            trails_.width != size().width * 2 || trails_.height != size().height * 4;
        if (kind != Kind::Bloom) trails_.resize(size());
        if (resized && kind == Kind::Orbit) orbit_dirty_ = true;
        if (kind == Kind::Flock && !flock_sized_ && size().width && size().height) {
            flock.reset(preset, aspect());
            flock_sized_ = true;
        }
        if (kind == Kind::Flock)
            for (auto& b : flock.birds)
                b.p.x = std::clamp(b.p.x, -aspect(), aspect());
    }

   private:
    struct Segment {
        Vec a, b;
        Light color;
    };
    struct OrbitTrace {
        double decay;
        std::vector<Segment> segments;
    };
    std::deque<OrbitTrace> history_;
    double decay_ = 0;
    Vec camera_{}, anchor_{};
    bool panning_ = false, pan_moved_ = false, orbit_dirty_ = false;
    Trails trails_;
    auto view(Vec p) const -> Vec { return (p - camera_) * zoom_; }
    auto position(Point p) const -> Vec { return camera_ + world(p) / zoom_; }
    void draw_trace(OrbitTrace const& trace)
    {
        for (auto const& s : trace.segments) {
            trails_.line(view(s.a), view(s.b), s.color, .65);
            trails_.point(view(s.b), s.color, 1.3);
        }
    }
    void redraw_orbit()
    {
        orbit_dirty_ = false;
        trails_.clear();
        double previous = history_.empty() ? decay_ : history_.front().decay;
        for (auto const& trace : history_) {
            trails_.fade(std::exp(previous - trace.decay));
            draw_trace(trace);
            previous = trace.decay;
        }
        trails_.fade(std::exp(previous - decay_));
    }
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
    struct SampleAxis {
        int lo, hi;
        double fraction;
    };
    std::vector<std::array<SampleAxis, 3>> bloom_x_, bloom_y_;
    std::vector<RGB> bloom_pixels_;
    std::array<Light, 1025> bloom_inks_{};
    int ink_preset_ = -1;
    static void sample_axis(std::vector<std::array<SampleAxis, 3>>& result,
                            int count,
                            int source,
                            double offset,
                            int stride)
    {
        if ((int)result.size() == count) return;
        result.resize(count);
        for (int i = 0; i < count; ++i)
            for (int j = 0; j < 3; ++j) {
                double p = std::clamp((i + .5) / count + (j - 1) * offset, 0., 1.) *
                           (source - 1);
                int low = (int)p;
                result[i][j] = {low * stride, std::min(low + 1, source - 1) * stride,
                                p - low};
            }
    }
    void paint_bloom(Canvas canvas)
    {
        auto extent = Area{canvas.size().width, canvas.size().height * 2};
        bloom_pixels_.resize(pixel_count(extent), background);
        sample_axis(bloom_x_, extent.width, phase::Bloom::width, .005, 1);
        sample_axis(bloom_y_, extent.height, phase::Bloom::height, .008,
                    phase::Bloom::width);
        if (ink_preset_ != preset) {
            for (std::size_t i = 0; i < bloom_inks_.size(); ++i)
                bloom_inks_[i] =
                    phase::spectrum(.56 + (double)i / 1024 * 1.4 + preset * .12);
            ink_preset_ = preset;
        }
        // Interpolate each source row once, then reuse it across the output width.
        std::array<std::array<double, phase::Bloom::width>, 3> rows{};
        auto sample = [&](SampleAxis x, int row) {
            return rows[row][x.lo] * (1 - x.fraction) + rows[row][x.hi] * x.fraction;
        };
        for (int y = 0; y < extent.height; ++y) {
            for (int row = 0; row < 3; ++row) {
                auto v = bloom_y_[y][row];
                for (int x = 0; x < phase::Bloom::width; ++x)
                    rows[row][x] = bloom.cells[v.lo + x].v * (1 - v.fraction) +
                                   bloom.cells[v.hi + x].v * v.fraction;
            }
            for (int x = 0; x < extent.width; ++x) {
                auto const& u = bloom_x_[x];
                double density = sample(u[1], 1);
                double dx = sample(u[2], 1) - sample(u[0], 1);
                double dy = sample(u[1], 2) - sample(u[1], 0);
                double shade = std::clamp(.8 + (dx - dy) * 5, .25, 1.6);
                double at = std::clamp(density * 1024, 0., 1024.);
                auto index = (std::size_t)at;
                double fraction = at - index;
                auto ink =
                    bloom_inks_[index] * (1 - fraction) +
                    bloom_inks_[std::min(index + 1, std::size_t{1024})] * fraction;
                auto color = Light{.025, .036, .075} + ink * (density * 4.5 * shade);
                bloom_pixels_[(std::size_t)y * extent.width + x] = Trails::rgb(color);
            }
        }
        paint_pixels(canvas, extent, bloom_pixels_);
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

void check_interactions()
{
    auto scene = Scene{Kind::Orbit};
    scene.pause(true);
    auto app = Application{scene};
    auto theme = Theme::dark();
    theme.palette.background = background;
    app.set_theme(theme);
    app.dispatch(esc::Resize{{120, 40}});
    auto snapshot = [&] {
        app.request_repaint();
        app.paint();
        return app.loop().screen;
    };
    auto braille = [](Frame const& frame) {
        int count = 0;
        for (int y = 0; y < frame.size().height; ++y)
            for (auto const& c : frame.row(y))
                count += c.text.size() == 1 && c.text[0] > U'\u2800' &&
                         c.text[0] <= U'\u28ff';
        return count;
    };
    auto compare = [&](Frame const& expected) {
        auto actual = snapshot();
        int different = 0;
        for (int y = 0; y < expected.size().height; ++y)
            for (int x = 0; x < expected.size().width; ++x)
                different += expected[{x, y}].text != actual[{x, y}].text;

        require(different <= 2, "Camera round trip lost or shifted trails");
    };
    for (int n = 0; n < 90; ++n)
        scene.advance(1. / 30);
    auto original = snapshot();
    require(braille(original) > 20, "Orbit trails missing");
    // The sun stays under the pointer; zoom uses small reversible increments.
    scene.handle(MouseWheel{.at = {60, 20}, .dy = -1});
    auto zoomed = snapshot();
    require(zoomed[{60, 20}].text == U"●" && braille(zoomed) > 20,
            "Zoom lost its anchor or trails");
    scene.handle(MouseWheel{.at = {60, 20}, .dy = 1});
    compare(original);
    // Go far out and back at an off-center anchor without advancing time.
    for (int n = 0; n < 30; ++n)
        scene.handle(MouseWheel{.at = {24, 12}, .dy = 1});
    snapshot();
    for (int n = 0; n < 30; ++n)
        scene.handle(MouseWheel{.at = {24, 12}, .dy = -1});
    compare(original);
    auto count = scene.orbit.bodies.size();
    scene.handle(MousePress{.at = {60, 20}, .button = MouseButton::Right});
    scene.handle(MouseMove{.at = {69, 24}, .held_button = MouseButton::Right});
    require(snapshot()[{69, 24}].text == U"●", "Grab pan did not follow the pointer");
    scene.handle(MouseRelease{.at = {69, 24}, .button = MouseButton::Right});
    require(scene.orbit.bodies.size() == count, "Panning removed a body");
    scene.handle(KeyEvent{.key = KeyCode::Home});
    compare(original);
    app.dispatch(esc::Resize{{480, 160}});
    require(braille(snapshot()) > 20, "Resize cleared world-space trails");
    app.dispatch(esc::Resize{{0, 0}});
    snapshot();
    app.dispatch(esc::Resize{{120, 40}});
    compare(original);
    // Long and offscreen drag guides must be continuous and share the canvas color.
    scene.handle(MousePress{.at = {2, 10}, .button = MouseButton::Left});
    scene.handle(MouseMove{.at = {110, 10}, .held_button = MouseButton::Left});
    auto guide = snapshot();
    for (int x = 2; x < 110; ++x)
        require(guide[{x, 10}].text == U"·" &&
                    guide[{x, 10}].style.background == Color{background},
                "Launch guide has gaps or a mismatched background");
    scene.handle(MouseMove{.at = {1000000, 10}, .held_button = MouseButton::Left});
    guide = snapshot();
    require(guide[{119, 10}].text == U"·", "Offscreen launch guide was not clipped");
    scene.handle(MouseRelease{.at = {110, 25}, .button = MouseButton::Left});
    require(scene.orbit.bodies.size() == count + 1 &&
                scene.orbit.bodies.back().v.x < 0 && scene.orbit.bodies.back().v.y < 0,
            "Launch did not oppose the pull");
    scene.handle(MousePress{.at = {2, 10}, .button = MouseButton::Right});
    scene.handle(MouseRelease{.at = {2, 10}, .button = MouseButton::Right});
    require(scene.orbit.bodies.size() == count, "Right click no longer removes a body");
    scene.parameter(2, 0);
    scene.orbit.bodies.clear();
    scene.orbit.selected = -1;
    for (int n = 0; n < 20; ++n)
        scene.advance(1. / 30);
    require(braille(snapshot()) == 0, "Minimum Orbit trails persist too long");

    scene.kind = Kind::Loom;
    scene.reset();
    for (int n = 0; n < 30; ++n)
        scene.advance(1. / 30);
    auto loom = snapshot();
    require(braille(loom) > 0, "Loom did not draw");
    for (int y = 0; y < loom.size().height; ++y)
        for (auto const& c : loom.row(y))
            require(c.style.background == Color{background},
                    "Loom overlay background mismatch");
    scene.loom.center = {100, 100};
    for (int n = 0; n < 45; ++n)
        scene.advance(1. / 30);
    require(braille(snapshot()) == 0, "Minimum Loom trails persist too long");
    scene.kind = Kind::Flock;
    scene.reset();
    scene.parameter(2, 1);
    for (int n = 0; n < 240; ++n)
        scene.advance(1. / 30);
    require(scene.healthy(), "Fast flock became unstable");
    for (auto b : scene.flock.birds)
        require(b.v.length() >= 1.44 - 1e-9 && b.v.length() <= 4 + 1e-9,
                "Flock maximum speed did not increase");

    scene.kind = Kind::Bloom;
    for (int preset = 0; preset < 3; ++preset) {
        scene.preset = preset;
        scene.reset();
        auto frame = snapshot();
        for (int y = 0; y < 80; ++y)
            for (int x = 0; x < 120; ++x) {
                double u = (x + .5) / 120, v = (y + .5) / 80;
                double density = scene.bloom.sample(u, v);
                double dx =
                    scene.bloom.sample(u + .005, v) - scene.bloom.sample(u - .005, v);
                double dy =
                    scene.bloom.sample(u, v + .008) - scene.bloom.sample(u, v - .008);
                auto color =
                    Light{.025, .036, .075} +
                    phase::spectrum(.56 + density * 1.4 + preset * .12) *
                        (density * 4.5 * std::clamp(.8 + (dx - dy) * 5, .25, 1.6));
                auto channel = [](double c) { return (int)(255 * (1 - std::exp(-c))); };
                auto style = frame[{x, y / 2}].style;
                auto actual =
                    std::get<RGB>(y % 2 ? style.foreground : style.background);
                require(std::abs(actual.red - channel(color.r)) <= 1 &&
                            std::abs(actual.green - channel(color.g)) <= 1 &&
                            std::abs(actual.blue - channel(color.b)) <= 1,
                        "Bloom shading optimization changed colors");
            }
    }
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
    bool check = false, json = false, benchmark = false;
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
        else if (arg == "--benchmark")
            benchmark = true;
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
                      << "phase [--check] [--json | --benchmark] [--scene 0..3 "
                         "--frames N --width W "
                         "--height H]\n";
            return 0;
        }
        else
            throw std::invalid_argument{"Unknown option"};
    }
    if (scene_index < 0 || scene_index > 3 || frames < 0 || frames > 10000 ||
        width < 0 || width > 2000 || height < 0 || height > 1000 ||
        (json && benchmark) || (benchmark && frames == 0))
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
    if (check || json || benchmark) {
        for (auto* s : scenes)
            s->pause(true);
        app.dispatch(esc::Resize{{width, height}});
        if (check) {
            check_models();
            check_interactions();
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
                         "pause/step, camera navigation, trails, shading and resizing "
                         "passed.\n";
            return 0;
        }
        root.select(scene_index);
        app.set_focus(*scenes[scene_index]);
        if (benchmark) {
            auto profile = esc::TerminalProfile{};
            profile.control = true;
            profile.colors = esc::ColorSupport::TrueColor;
            auto renderer = Renderer{profile};
            std::array<double, 3> timings{};
            std::size_t bytes = 0;
            for (int n = -30; n < frames; ++n) {
                auto a = std::chrono::steady_clock::now();
                scenes[scene_index]->advance(1. / 30);
                auto b = std::chrono::steady_clock::now();
                app.paint();
                auto c = std::chrono::steady_clock::now();
                auto output = renderer.render(app.loop().screen);
                auto d = std::chrono::steady_clock::now();
                if (n < 0) continue;
                timings[0] += std::chrono::duration<double, std::milli>(b - a).count();
                timings[1] += std::chrono::duration<double, std::milli>(c - b).count();
                timings[2] += std::chrono::duration<double, std::milli>(d - c).count();
                bytes += output.size();
            }
            std::cout << "scene,width,height,frames,simulation_ms,paint_ms,encode_ms,"
                         "bytes_per_frame\n"
                      << scene_index << ',' << width << ',' << height << ',' << frames;
            for (auto t : timings)
                std::cout << ',' << t / frames;
            std::cout << ',' << bytes / frames << '\n';
            return 0;
        }
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
