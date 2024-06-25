#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <signals_light/signal.hpp>

#include <ox/widgets.hpp>

using namespace ox;

struct Clicker {
    Point at;
    Glyph display = U'X' | fg(XColor::Default);
};

void paint(Clicker const& c, Canvas canvas)
{
    if (c.at.x < canvas.size.width && c.at.y < canvas.size.height) {
        canvas[c.at] = c.display;
    }
}

void mouse_release(Clicker& c, Mouse m)
{
    if (m.button == Mouse::Button::Left) {
        c.at = m.at;
    }
}

void mouse_move(Clicker& c, Mouse m) { c.at = m.at; }

void mouse_wheel(Clicker& c, Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        c.display = c.display | bg(XColor::BrightBlue);
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        c.display = c.display | bg(XColor::BrightRed);
    }
}

void focus_in(Clicker& c) { c.display = c.display | fg(XColor::BrightYellow); }

void focus_out(Clicker& c) { c.display = c.display | fg(XColor::Default); }

// -------------------------------------------------------------------------------------

[[nodiscard]] auto message_source(sl::Slot<void(std::string)> on_message) -> VLayout
{
    auto head = VLayout{};

    auto& tb = append(head, text_box({.editable = true}), SizePolicy::flex(),
                      FocusPolicy::Strong);

    append_divider(head);

    append(head, Clicker{}, SizePolicy::flex(.5));

    append_divider(head);

    append(head,
           border({.child = button({
                       .text = "Send",
                       .brush = {.background = XColor::BrightBlue,
                                 .foreground = XColor::Black,
                                 .traits = Trait::Underline},
                       .on_press_brush = {{.background = XColor::Blue,
                                           .foreground = XColor::Black,
                                           .traits = Trait::Underline | Trait::Bold}},
                       .on_hover_brush = {{.background = XColor::BrightBlue,
                                           .foreground = XColor::Black,
                                           .traits = Trait::Underline | Trait::Bold}},
                       .on_release =
                           [on_message = on_message, &tb = tb] {
                               on_message(tb.text);
                               tb.text.clear();
                           },
                   })}),
           SizePolicy::fixed(4));

    return head;
}

[[nodiscard]] auto message_app() -> HLayout
{
    auto head = hlayout();

    append_divider(head, {U'┃'});

    auto& rhs = append(head, text_box({
                                 .text = "Right Hand Side",
                                 .editable = false,
                             }));

    insert_at(head, 0, message_source([&rhs = rhs](std::string msg) {
                  rhs.text = std::move(msg);
              }),
              SizePolicy::flex(2));

    return head;
}

// -------------------------------------------------------------------------------------

[[nodiscard]] auto message_app2() -> HLayout
{
    auto head = hlayout();

    auto& lay1 = append(head, vlayout());
    append_divider(head);
    auto& lay2 = append(head, vlayout());
    append_divider(head);
    auto& lay3 = append(head, vlayout());
    append_divider(head);
    auto& lay4 = append(head, vlayout());

    append(lay1, Clicker{});
    append_divider(lay1);
    append(lay1, Clicker{});
    append_divider(lay1);
    append(lay1, Clicker{});

    append(lay2, Clicker{});
    append_divider(lay2);
    append(lay2, Clicker{});

    append(lay3, Clicker{});
    append_divider(lay3);
    append(lay3, Clicker{});

    append(lay4, Clicker{});
    append_divider(lay4);
    append(lay4, border({Clicker{}}));
    append_divider(lay4);
    append(lay4, Clicker{});
    append_divider(lay4);
    append(lay4, Clicker{});

    return head;
}

int main()
{
    try {
        return Application{
            message_app2(),
            {MouseMode::Move},
        }
            .run();
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