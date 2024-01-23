#include <termox/application.hpp>

int main()
{
    using namespace ox;

    class {
       public:
        // TODO remove esc::
        // TODO maybe rename to mouse_press
        auto handle_mouse_press(esc::Mouse const& m) -> EventResponse
        {
            if (m.button == esc::Mouse::Button::Left) {
                Painter{}[m.at] = {U'X', {.foreground = esc::Red}};
                // Painter{}[m.at] = U'X' | fg(Red);
                // TODO Color::Red?
                Terminal::cursor = m.at;
            }
            return {};
        }

        auto handle_key_press(esc::Key) -> EventResponse
        {
            return QuitRequest{0};
        }
    } w;

    return Application{}.run(w);
}