#include <termox/core.hpp>

#include <thread>
#include <chrono>

using namespace ox;

int main()
{
    class {
       public:
        auto handle_mouse_press(Mouse const& m) -> EventResponse
        {
            if (m.button == Mouse::Button::Left) {
                Painter{}[m.at] = U'X' | Trait::Bold | Trait::Italic |
                                  fg(ColorIndex::Red) | bg(TrueColor{0x8bb14e});
                Terminal::cursor = m.at;
                std::this_thread::sleep_for(std::chrono::seconds{10});
            }
            return {};
        }

        auto handle_key_press(Key) -> EventResponse { return QuitRequest{0}; }
    } w;

    auto term = Terminal{};

    return process_events(term, w);
}