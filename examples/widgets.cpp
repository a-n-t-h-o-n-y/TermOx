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

struct MessageSource : VLayout {
    sl::Slot<void(std::string)> on_message;

    TextBox& text_box = append(*this,
                               TextBox{
                                   .editable = true,
                               },
                               SizePolicy::flex(),
                               FocusPolicy::Strong);

    Divider& div = append_divider(*this);

    Button& send_button =
        append(*this,
               button({
                   .text           = "Send",
                   .brush          = {.background = ox::XColor::BrightBlue,
                                      .foreground = ox::XColor::Black,
                                      .traits     = ox::Trait::Underline},
                   .on_press_brush = {{.background = ox::XColor::Blue,
                                       .foreground = ox::XColor::White,
                                       .traits     = ox::Trait::Underline}},
                   .on_release =
                       [on_message = on_message, &text_box = text_box] {
                           on_message(text_box.text);
                           text_box.text.clear();
                       },
               }),
               SizePolicy::fixed(3));
};

struct MessageApp : HLayout {
    Divider& div = append_divider(*this, {U'┃'});

    TextBox& rhs = append(*this,
                          TextBox{
                              .text     = "Right Hand Side",
                              .editable = false,
                          });

    MessageSource& lhs = insert_at(
        *this,
        0,
        MessageSource{
            .on_message = [&rhs = rhs](std::string msg) { rhs.text = std::move(msg); },
        });
};

// -----------------------------------------------------------------------------

int main()
{
    try {
        return Application{MessageApp{}}.run();
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