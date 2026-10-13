#include <iostream>

#include <ox/core/terminal.hpp>

// Driven by smoke_examples.py through a pseudo-terminal. Separate acknowledgments
// let the test distinguish terminal output from its own synchronization messages.
int main()
{
    {
        auto terminal = ox::Terminal{};
        auto frame = ox::Frame{{8, 2}};
        frame.write({0, 0}, U"x", {.foreground = ox::colors::red});
        std::cerr << "ready\n" << std::flush;
        while (true) {
            auto const event = terminal.read(std::chrono::milliseconds{100});
            if (!event) { continue; }
            auto const key = std::get_if<ox::KeyEvent>(&*event);
            if (!key) { continue; }
            auto const command = ox::input_text(*key);
            if (command == U"q") { break; }
            if (command == U"e") {
                frame.write({7, 1}, U"y", {.foreground = ox::colors::blue});
            }
            terminal.commit_changes(
                frame, command == U"v" ? ox::Cursor{ox::Point{0, 0}} : ox::Cursor{});
            std::cerr << "done\n" << std::flush;
        }
    }
    std::cerr << "closed\n" << std::flush;
}
