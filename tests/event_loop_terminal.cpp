#include <chrono>
#include <iostream>
#include <string_view>
#include <thread>

#include <ox/core/event_loop.hpp>

// Compare the former 16 ms pump with readiness waiting on the same terminal and
// queue. Only the worker posts across threads; terminal I/O stays on this thread.
int main(int argc, char** argv)
{
    using namespace std::chrono_literals;
    using Clock = ox::EventLoop::Clock;
    auto const polling = argc == 2 && std::string_view{argv[1]} == "--polling";
    auto terminal = ox::Terminal{};
    auto loop = ox::EventLoop{};
    if (argc == 2 && std::string_view{argv[1]} == "--escape") {
        struct EscapeHandler {
            auto handle(ox::KeyEvent const& event) -> ox::EventResponse
            { return {.quit = event.matches(esc::KeyCode::Escape) ? 0 : 2}; }
        } handler;
        std::cerr << "ready\n" << std::flush;
        return ox::process_events(loop, terminal, handler);
    }
    struct Handler {
    } handler;
    int iterations = 0;
    double latency = 0;
    auto worker = std::jthread{[&] {
        std::this_thread::sleep_for(1s);
        auto const posted = Clock::now();
        loop.post(ox::event::Custom{[&, posted] {
            latency = std::chrono::duration<double, std::micro>(Clock::now() - posted)
                          .count();
            return ox::EventResponse{.quit = 0};
        }});
    }};
    if (polling) {
        loop.post(esc::Resize{terminal.size()});
        while (!loop.exit_code()) {
            ++iterations;
            loop.step(handler);
            if (loop.paint(handler)) {
                terminal.commit_changes(loop.screen, loop.cursor);
            }
            if (loop.exit_code()) { break; }
            if (auto event = terminal.read(std::min(loop.wait_duration(), 16ms))) {
                loop.post(std::move(*event));
            }
        }
    }
    else {
        (void)ox::process_events(loop, terminal, handler, [&] {
            ++iterations;
            return ox::EventResponse{};
        });
    }
    worker.join();
    std::cerr << iterations << ',' << latency << '\n';
}
