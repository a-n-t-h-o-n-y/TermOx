#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/widgets.hpp>

// struct Clicker {
//     ox::Point at;
//     ox::Glyph display = U'X' | fg(ox::XColor::Default);
// };

// void paint(Clicker const& c, ox::Canvas canvas) { canvas[c.at] = c.display; }

// void mouse_press(Clicker& c, ox::Mouse m)
// {
//     if (m.button == ox::Mouse::Button::Left) {
//         c.at = m.at;
//     }
// }

// void focus_in(Clicker& c)
// {
//     c.display = c.display | fg(ox::XColor::BrightYellow);
// }

// void focus_out(Clicker& c) { c.display = c.display | fg(ox::XColor::Default);
// }

// -----------------------------------------------------------------------------

using namespace ox::widgets;

[[nodiscard]] auto message_source(sl::Slot<void(std::string)> on_message)
    -> VLayout
{
    auto layout = VLayout{};

    auto& text_box = append(layout,
                            TextBox{
                                .editable = true,
                            },
                            SizePolicy::flex(), FocusPolicy::Strong);
    append(layout,
           Button{
               .label = {.text = "Send"},
               .clicked =
                   [on_message, &text_box] {
                       on_message(text_box.text);
                       text_box.text.clear();
                   },
           },
           SizePolicy::fixed(1));

    return layout;
}

[[nodiscard]] auto message_app() -> HLayout
{
    auto layout = HLayout{};

    auto& rhs = append(layout, TextBox{
                                   .text     = "Right Hand Side",
                                   .editable = false,
                               });

    // TODO editing .text directly leads to inconsistent state. Either a
    // function or something, but that is dangerous to leave it as a struct. Can
    // you have private data? Or .. no because text would have to be private as
    // well. You need some guarantee that the user cannot put it in an
    // inconsistent state easily.

    insert_at(layout, 0,
              message_source([&rhs](std::string msg) { rhs.text = msg; }));

    return layout;
}

int main()
{
    try {
        return Application{message_app()}.run();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error\n";
        return 1;
    }
}