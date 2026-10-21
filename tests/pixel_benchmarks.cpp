#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ox/dotgrid.hpp>
#include <ox/pixelgrid.hpp>

int main()
{
    std::cout << "workload,iterations,median_us_per_operation,checksum\n";
    for (auto extent : {128, 1024}) {
        for (auto clipped : {false, true}) {
            auto const size = ox::Area{extent, extent};
            auto grid = ox::PixelGrid{
                size,
                std::vector<ox::RGB>((std::size_t)extent * extent, ox::RGB{0x123456})};
            auto frame = ox::Frame{{80, 24}};
            auto canvas = clipped
                              ? ox::Canvas{frame}
                                    .subcanvas({2, 1}, {30, 12})
                                    .subcanvas({-50, -20}, {extent, extent / 2})
                              : ox::Canvas{frame}.subcanvas({}, {extent, extent / 2});
            std::size_t checksum = 0;
            auto work = [&] {
                grid.paint(canvas);
                for (int y = 0; y < frame.size().height; ++y) {
                    for (auto cell : frame.row(y)) {
                        if (auto color = std::get_if<ox::RGB>(&cell.style.background)) {
                            checksum += color->red + color->green + color->blue;
                        }
                    }
                }
            };
            for (int i = 0; i < 3; ++i) {
                work();
            }
            checksum = 0;
            auto samples = std::vector<double>{};
            for (int sample = 0; sample < 5; ++sample) {
                auto const start = std::chrono::steady_clock::now();
                for (int i = 0; i < 20; ++i) {
                    work();
                }
                samples.push_back(std::chrono::duration<double, std::micro>(
                                      std::chrono::steady_clock::now() - start)
                                      .count() /
                                  20);
            }
            std::ranges::sort(samples);
            std::cout << "pixels-" << extent
                      << (clipped ? "-inherited-clip" : "-frame-clip") << ",20,"
                      << std::fixed << std::setprecision(3) << samples[2] << ','
                      << checksum << '\n';
        }
    }
    for (auto size : {ox::Area{200, 60}, ox::Area{480, 160}}) {
        auto frame = ox::Frame{size};
        auto canvas = ox::Canvas{frame};
        auto dots = ox::DotGrid{{size.width * 2, size.height * 4}};
        auto pixels = ox::PixelGrid{{size.width, size.height * 2}};
        for (auto mode : {0, 1}) {
            std::vector<double> samples;
            std::size_t checksum = 0;
            for (int sample = -1; sample < 5; ++sample) {
                double elapsed = 0;
                for (int i = 0; i < 20; ++i) {
                    // Prepare alternating frames outside the timed painting section.
                    auto color = ox::RGB{(std::uint8_t)(i % 2 ? 200 : 50), 80, 120};
                    for (int y = 0; y < size.height; ++y)
                        for (int x = 0; x < size.width; ++x) {
                            if (mode == 0) {
                                dots.set_dot({x * 2, y * 4}, i % 2);
                                dots.set_dot({x * 2 + 1, y * 4 + 3}, !(i % 2));
                                dots.set_cell_colors({x, y}, {.foreground = color});
                            }
                            else {
                                pixels.set_pixel({x, y * 2}, color);
                                pixels.set_pixel({x, y * 2 + 1}, ox::RGB{0x345678});
                            }
                        }
                    auto start = std::chrono::steady_clock::now();
                    if (mode == 0)
                        dots.paint(canvas);
                    else
                        pixels.paint(canvas);
                    elapsed += std::chrono::duration<double, std::micro>(
                                   std::chrono::steady_clock::now() - start)
                                   .count();
                    if (sample >= 0)
                        for (int y = 0; y < size.height; ++y)
                            for (auto const& c : frame.row(y))
                                checksum +=
                                    c.text[0] +
                                    std::get<ox::RGB>(mode == 0 ? c.style.foreground
                                                                : c.style.background)
                                        .red;
                }
                if (sample >= 0) samples.push_back(elapsed / 20);
            }
            std::ranges::sort(samples);
            std::cout << (mode == 0 ? "dots-animated-" : "pixels-animated-")
                      << size.width << 'x' << size.height << ",20," << samples[2] << ','
                      << checksum << '\n';
        }
    }
}
