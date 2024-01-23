#include <termox/application.hpp>

int main()
{
    using namespace ox;

    auto app = Application{};

    class {
       public:
        esc::Point coordinates{.x = 0, .y = 0};
        esc::Area size{.width = 20, .height = 10};

       public:
        auto handle_mouse_press(esc::Mouse const& m) -> EventResponse
        {
            if (m.button == esc::Mouse::Button::Left) {
                auto c  = Painter{*this};
                c[m.at] = {U'X', {.foreground = esc::Red}};
            }
            return {};
        }

        auto handle_key_press(esc::Key) -> EventResponse
        {
            return QuitRequest{};
        }

        auto handle_resize(esc::Area new_size) -> EventResponse
        {
            size = new_size;
            return {};
        }
    } w;

    return app.run(w);
}