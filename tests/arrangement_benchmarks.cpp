#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <ox/ox.hpp>
#include <string_view>
#include <vector>
std::size_t allocations = 0;
auto operator new(std::size_t size) -> void*
{
    auto* p = std::malloc(size ? size : 1);
    if (!p) throw std::bad_alloc{};
    ++allocations;
    return p;
}
auto operator new[](std::size_t size) -> void* { return ::operator new(size); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
std::size_t checksum = 0;
template <class F>
void measure(std::string name, F work)
{
    for (int i = 0; i < 10; ++i)
        checksum += work(i);
    std::vector<double> samples;
    auto const initial = checksum;
    std::size_t calls = 0;
    for (int sample = 0; sample < 5; ++sample) {
        auto const before = allocations;
        auto const start = std::chrono::steady_clock::now();
        for (int i = 0; i < 100; ++i)
            checksum += work(i);
        auto const elapsed = std::chrono::duration<double, std::micro>(
                                 std::chrono::steady_clock::now() - start)
                                 .count();
        calls += allocations - before;
        samples.push_back(elapsed / 100);
    }
    std::ranges::sort(samples);
    std::cout << name << ",100," << std::fixed << std::setprecision(3) << samples[2]
              << ',' << checksum - initial << ',' << (double)calls / 500 << '\n';
}
int main(int argc, char** argv)
{
    if (argc > 1 && std::string_view{argv[1]} != "--arrangement") { return 2; }
    std::cout << "workload,iterations,median_us_per_operation,checksum,allocations_per_"
                 "operation\n";
    for (int count : {10, 1000}) {
        auto row = ox::Row<std::vector<ox::Widget>>{};
        row.children.reserve((std::size_t)count);
        for (int i = 0; i < count; ++i)
            row.children.emplace_back(ox::FocusPolicy::None,
                                      ox::SizePolicy::flex((float)(1 + i % 5)));
        ox::arrange(row, {{}, {2000, 20}});
        auto sum = [&] {
            std::size_t s = 0;
            for (auto const& c : row.children)
                s += (std::size_t)(c.at().x + c.size().width + c.size().height);
            return s;
        };
        measure("arrange.unchanged." + std::to_string(count), [&](int) {
            ox::arrange(row, row.bounds());
            return sum();
        });
        measure("arrange.resize." + std::to_string(count), [&](int i) {
            ox::arrange(row, {{}, {2000 + i % 2, 20}});
            return sum();
        });
        measure("arrange.policy." + std::to_string(count), [&](int i) {
            row.children[0].size_policy = ox::SizePolicy::fixed(1 + i % 2);
            ox::arrange(row, row.bounds());
            return sum();
        });
    }
}
