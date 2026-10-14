#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
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
}
