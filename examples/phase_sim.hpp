#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <random>
#include <vector>

namespace phase {
constexpr double tau = 2 * std::numbers::pi;
struct Vec {
    double x = 0, y = 0;
    auto operator+(Vec b) const -> Vec { return {x + b.x, y + b.y}; }
    auto operator-(Vec b) const -> Vec { return {x - b.x, y - b.y}; }
    auto operator*(double s) const -> Vec { return {x * s, y * s}; }
    auto operator/(double s) const -> Vec { return *this * (1 / s); }
    auto length2() const -> double { return x * x + y * y; }
    auto length() const -> double { return std::sqrt(length2()); }
};
inline auto unit(Vec v) -> Vec { return v / std::max(1e-9, v.length()); }
inline auto limit(Vec v, double speed) -> Vec
{ return v * std::min(1., speed / std::max(1e-9, v.length())); }
inline auto finite(Vec v) -> bool { return std::isfinite(v.x) && std::isfinite(v.y); }
// Liang–Barsky clipping bounds raster work, including captured drags offscreen.
inline auto clip_line(Vec& a,
                      Vec& b,
                      double left,
                      double right,
                      double top,
                      double bottom) -> bool
{
    if (!finite(a) || !finite(b) || left > right || top > bottom) return false;
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
    if (!clip(-d.x, a.x - left) || !clip(d.x, right - a.x) || !clip(-d.y, a.y - top) ||
        !clip(d.y, bottom - a.y))
        return false;
    b = a + d * hi;
    a = a + d * lo;
    return true;
}
struct Light {
    double r = 0, g = 0, b = 0;
    auto operator+(Light v) const -> Light { return {r + v.r, g + v.g, b + v.b}; }
    auto operator*(double s) const -> Light { return {r * s, g * s, b * s}; }
};
inline auto spectrum(double t) -> Light
{
    return {.55 + .45 * std::cos(tau * t), .55 + .45 * std::cos(tau * (t + .33)),
            .55 + .45 * std::cos(tau * (t + .67))};
}

struct Body {
    Vec p, v;
    double mass, hue;
};
struct Orbit {
    std::vector<Body> bodies;
    int selected = 0;
    void reset(int preset)
    {
        bodies.clear();
        selected = 0;
        if (preset == 0) {
            bodies.push_back({{}, {}, 24, .10});
            for (int i = 0; i < 5; ++i) {
                double a = i * 1.8, r = .27 + i * .13;
                Vec radial{std::cos(a), std::sin(a)};
                bodies.push_back({radial * r,
                                  Vec{-radial.y, radial.x} * std::sqrt(.10 * 24 / r),
                                  .035 + .015 * i, .45 + i * .12});
            }
        }
        else if (preset == 1) {
            bodies = {{{-.3, 0}, {0, -.84}, 8.5, .05},
                      {{.3, 0}, {0, .84}, 8.5, .55},
                      {{0, .85}, {-1.4, 0}, .025, .8}};
        }
        else {
            for (int i = 0; i < 12; ++i) {
                double a = i * tau / 12, r = .35 + .17 * std::sin(i * 2.3);
                Vec p{std::cos(a) * r, std::sin(a) * r};
                bodies.push_back({p, Vec{-p.y, p.x} * .8, .8, i / 12.});
            }
        }
    }
    void step(double dt, double gravity)
    {
        dt *= .4;
        for (int sub = 0; sub < 6; ++sub) {
            std::vector<Vec> acceleration(bodies.size());
            for (std::size_t i = 0; i < bodies.size(); ++i)
                for (std::size_t j = i + 1; j < bodies.size(); ++j) {
                    auto d = bodies[j].p - bodies[i].p;
                    double r2 = d.length2() + .0025;
                    auto force = d * (gravity / (r2 * std::sqrt(r2)));
                    acceleration[i] = acceleration[i] + force * bodies[j].mass;
                    acceleration[j] = acceleration[j] - force * bodies[i].mass;
                }
            for (std::size_t i = 0; i < bodies.size(); ++i) {
                bodies[i].v = bodies[i].v + acceleration[i] * (dt / 6);
                bodies[i].p = bodies[i].p + bodies[i].v * (dt / 6);
            }
            // Inelastic collisions conserve total mass and momentum.
            for (std::size_t i = 0; i < bodies.size(); ++i)
                for (std::size_t j = i + 1; j < bodies.size();) {
                    double radius =
                        .007 * (std::cbrt(bodies[i].mass) + std::cbrt(bodies[j].mass));
                    if ((bodies[i].p - bodies[j].p).length() < radius) {
                        auto& a = bodies[i];
                        auto b = bodies[j];
                        double mass = a.mass + b.mass;
                        a.p = (a.p * a.mass + b.p * b.mass) / mass;
                        a.v = (a.v * a.mass + b.v * b.mass) / mass;
                        a.mass = mass;
                        bodies.erase(bodies.begin() + (std::ptrdiff_t)j);
                        if (selected == (int)j)
                            selected = (int)i;
                        else if (selected > (int)j)
                            --selected;
                    }
                    else
                        ++j;
                }
        }
    }
    auto nearest(Vec p, double radius) const -> int
    {
        int found = -1;
        for (int i = 0; i < (int)bodies.size(); ++i) {
            double d = (p - bodies[i].p).length();
            if (d < radius) {
                radius = d;
                found = i;
            }
        }
        return found;
    }
    void launch(Vec p, Vec v, double mass)
    {
        if (bodies.size() == 48) return;
        bodies.push_back({p, limit(v, 8), mass, bodies.size() * .137});
        selected = (int)bodies.size() - 1;
    }
    void remove(int i)
    {
        if (i < 0) return;
        bodies.erase(bodies.begin() + i);
        selected = bodies.empty() ? -1 : std::min(selected, (int)bodies.size() - 1);
    }
};

// Gray–Scott reaction diffusion with periodic boundaries and a nine-point Laplacian.
struct Bloom {
    static constexpr int width = 192, height = 128;
    struct Cell {
        double u = 1, v = 0;
    };
    std::vector<Cell> cells = std::vector<Cell>(width * height), next = cells;
    int preset = 0;
    void seed(double x, double y, double radius, bool erase = false)
    {
        if (!std::isfinite(x) || !std::isfinite(y) || x < -1 || x > 2 || y < -1 ||
            y > 2)
            return;
        int cx = (int)(x * width), cy = (int)(y * height);
        int r = (int)radius;
        for (int yy = std::max(0, cy - r); yy <= std::min(height - 1, cy + r); ++yy)
            for (int xx = std::max(0, cx - r); xx <= std::min(width - 1, cx + r); ++xx)
                if ((xx - cx) * (xx - cx) + (yy - cy) * (yy - cy) <= r * r)
                    cells[yy * width + xx] = erase ? Cell{} : Cell{.45, .85};
    }
    void reset(int p)
    {
        preset = p;
        std::fill(cells.begin(), cells.end(), Cell{});
        for (int i = 0; i < 23; ++i) {
            double a = i * 2.39996, r = .055 * std::sqrt(i);
            seed(.5 + r * std::cos(a), .5 + r * std::sin(a), 3 + i % 4);
        }
        step(160, 0);
    }
    void step(int iterations, double shift)
    {
        constexpr std::array<double, 3> feeds{.0545, .029, .0367};
        constexpr std::array<double, 3> kills{.062, .057, .0649};
        double feed = feeds[preset] + shift, kill = kills[preset];
        for (int n = 0; n < iterations; ++n) {
            for (int y = 0; y < height; ++y) {
                int up = (y + height - 1) % height, down = (y + 1) % height;
                for (int x = 0; x < width; ++x) {
                    int l = (x + width - 1) % width, r = (x + 1) % width;
                    auto c = cells[y * width + x];
                    double lu = -c.u, lv = -c.v;
                    auto add = [&](int xx, int yy, double weight) {
                        auto p = cells[yy * width + xx];
                        lu += p.u * weight;
                        lv += p.v * weight;
                    };
                    add(l, y, .2);
                    add(r, y, .2);
                    add(x, up, .2);
                    add(x, down, .2);
                    add(l, up, .05);
                    add(r, up, .05);
                    add(l, down, .05);
                    add(r, down, .05);
                    double reaction = c.u * c.v * c.v;
                    next[y * width + x] = {
                        std::clamp(c.u + lu - reaction + feed * (1 - c.u), 0., 1.),
                        std::clamp(c.v + .5 * lv + reaction - (feed + kill) * c.v, 0.,
                                   1.)};
                }
            }
            cells.swap(next);
        }
    }
    auto sample(double x, double y) const -> double
    {
        x = std::clamp(x, 0., 1.) * (width - 1);
        y = std::clamp(y, 0., 1.) * (height - 1);
        int ix = (int)x, iy = (int)y;
        int rx = std::min(ix + 1, width - 1), by = std::min(iy + 1, height - 1);
        double a = x - ix, b = y - iy;
        return (cells[iy * width + ix].v * (1 - a) + cells[iy * width + rx].v * a) *
                   (1 - b) +
               (cells[by * width + ix].v * (1 - a) + cells[by * width + rx].v * a) * b;
    }
};

struct Loom {
    double time = 0;
    Vec center;
    int preset = 0;
    void reset(int p)
    {
        time = 0;
        center = {};
        preset = p;
    }
    auto arms(double t, double ratio, double reach) const -> std::array<Vec, 3>
    {
        double a = t, b = -t * ratio;
        double r = .42 * reach;
        Vec elbow = center + Vec{std::cos(a), std::sin(a)} * r;
        Vec end = elbow + Vec{std::cos(b), std::sin(b)} * (r * .78);
        if (preset == 1)
            end = end + Vec{std::sin(t * 2.03), std::cos(t * 3.01)} * (.16 * reach);
        if (preset == 2)
            end = center + Vec{std::sin(t), std::sin(t * ratio + .7)} * (.7 * reach);
        return {center, elbow, end};
    }
};

struct Bird {
    Vec p, v;
    double hue;
};
struct Flock {
    std::vector<Bird> birds;
    std::vector<Vec> obstacles;
    std::mt19937 rng{17};
    int preset = 0;
    auto random(double low, double high) -> double
    { return std::uniform_real_distribution<double>{low, high}(rng); }
    void reset(int p, double aspect)
    {
        preset = p;
        rng.seed(17);
        birds.clear();
        obstacles.clear();
        for (int i = 0; i < (p == 2 ? 260 : 180); ++i) {
            double a = random(0, tau);
            birds.push_back({{random(-aspect, aspect), random(-1, 1)},
                             Vec{std::cos(a), std::sin(a)} * .3,
                             i / 260.});
        }
    }
    void step(double dt,
              double aspect,
              double cohesion,
              double separation,
              double speed,
              Vec pointer,
              bool guided)
    {
        std::vector<Vec> velocities(birds.size());
        auto delta = [aspect](Vec d) {
            if (d.x > aspect) d.x -= 2 * aspect;
            if (d.x < -aspect) d.x += 2 * aspect;
            if (d.y > 1) d.y -= 2;
            if (d.y < -1) d.y += 2;
            return d;
        };
        for (std::size_t i = 0; i < birds.size(); ++i) {
            auto const& b = birds[i];
            Vec align, center, repel;
            int count = 0;
            for (std::size_t j = 0; j < birds.size(); ++j) {
                if (i == j) continue;
                Vec d = delta(birds[j].p - b.p);
                double distance = d.length2();
                if (distance < .10) {
                    align = align + birds[j].v;
                    center = center + d;
                    ++count;
                }
                if (distance < .006) repel = repel - d / (distance + .0001);
            }
            Vec force = repel * (.05 * separation);
            if (count)
                force = force + (align / count - b.v) * 1.4 +
                        center / count * (1.2 * cohesion);
            if (guided) force = force + limit(pointer - b.p, .7) * 1.4;
            for (auto obstacle : obstacles) {
                Vec d = b.p - obstacle;
                if (d.length2() < .11)
                    force = force + unit(d) * (3 * (1 - d.length() / .34));
            }
            if (preset == 1)
                force = force + Vec{std::sin(b.p.y * 5), std::cos(b.p.x * 4)} * .22;
            auto velocity = b.v + limit(force, 3) * dt;
            velocities[i] =
                unit(velocity) * std::clamp(velocity.length(), .18 * speed, .5 * speed);
        }
        for (std::size_t i = 0; i < birds.size(); ++i) {
            auto& b = birds[i];
            b.v = velocities[i];
            b.p = b.p + b.v * dt;
            b.p.x = std::fmod(b.p.x + aspect * 3, aspect * 2) - aspect;
            b.p.y = std::fmod(b.p.y + 3, 2.) - 1;
        }
    }
};
}  // namespace phase
