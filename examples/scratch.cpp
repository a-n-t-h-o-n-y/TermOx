#include <termox/core.hpp>

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
            }
            return {};
        }

        auto handle_key_press(Key) -> EventResponse { return QuitRequest{0}; }
    } w;

    return Application{}.run(w);
}