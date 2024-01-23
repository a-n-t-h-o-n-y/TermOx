#include <termox/application.hpp>

int main()
{
    using namespace ox;

    auto app = Application{};

    struct {
        esc::Point coordinates{.x = 0, .y = 0};
        esc::Area size{.width = 20, .height = 10};

        Glyph fill_glyph{U'-'};

        auto handle_mouse_press(esc::Mouse const& m) -> EventResponse
        {
            if (m.button == esc::Mouse::Button::Left) {
                auto c  = ox::Canvas{*this};
                c[m.at] = {U'X', {.foreground = esc::Red}};
            }
            return std::nullopt;
        }

        auto handle_key_press(esc::Key) -> EventResponse
        {
            return QuitRequest{};
        }

        auto handle_resize(esc::Area new_size) -> EventResponse
        {
            // if this was a layout would need to reposition children
            // and directly call handle_resize on them? and what do you do with
            // the return value?
            size = new_size;
            Canvas{*this};
            return {};
        }
    } w;

    return app.run(w);
}