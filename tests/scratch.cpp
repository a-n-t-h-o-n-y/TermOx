#include <termox/application.hpp>

int main()
{
    using namespace ox;

    auto app = Application{};

    struct {
        esc::Point coordinates{.x = 0, .y = 0};
        esc::Area size{.width = 20, .height = 10};

        auto handle_mouse_press(esc::Mouse const& m) -> EventResponse
        {
            if (m.button == esc::Mouse::Button::Left) {
                auto c  = ox::Canvas{*this};
                c[m.at] = {U'X'};
                return c;
            }
            return std::nullopt;
        }

        auto handle_key_press(esc::Key) -> EventResponse
        {
            return QuitRequest{};
        }
    } w;

    app.run(w);

    return 0;
}