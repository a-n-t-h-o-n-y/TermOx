#include <termox/application.hpp>

int main()
{
    using namespace ox;

    class {
       public:
        // TODO maybe rename to mouse_press
        auto handle_mouse_press(Mouse const& m) -> EventResponse
        {
            if (m.button == Mouse::Button::Left) {
                // Painter{}[m.at] = {U'X',
                //                    {.foreground = ColorIndex::BrightBlue}};
                Painter{}[m.at] = {U'X', {.foreground = TrueColor{0x8bb14e}}};
                // Painter{}[m.at] = {U'X', {.foreground = DefaultColor{}}};
                // Painter{}[m.at] = U'X' | fg(Red);
                // TODO Color::Red?
                Terminal::cursor = m.at;
            }
            return {};
        }

        auto handle_key_press(Key) -> EventResponse { return QuitRequest{0}; }
    } w;

    return Application{}.run(w);
}