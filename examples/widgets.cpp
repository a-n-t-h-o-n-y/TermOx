#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/widgets.hpp>

struct Clicker {
    ox::Point at;
    ox::Glyph display = U'X' | fg(ox::XColor::Default);
};

void paint(Clicker const& c, ox::Canvas canvas)
{
    if (c.at.x < canvas.size.width && c.at.y < canvas.size.height) {
        canvas[c.at] = c.display;
    }
}

void mouse_release(Clicker& c, ox::Mouse m)
{
    if (m.button == ox::Mouse::Button::Left) {
        c.at = m.at;
    }
}

void mouse_move(Clicker& c, ox::Mouse m) { c.at = m.at; }

void mouse_wheel(Clicker& c, ox::Mouse m)
{
    if (m.button == ox::Mouse::Button::ScrollUp) {
        c.display = c.display | bg(ox::XColor::BrightBlue);
    }
    else if (m.button == ox::Mouse::Button::ScrollDown) {
        c.display = c.display | bg(ox::XColor::BrightRed);
    }
}

void focus_in(Clicker& c) { c.display = c.display | fg(ox::XColor::BrightYellow); }

void focus_out(Clicker& c) { c.display = c.display | fg(ox::XColor::Default); }

// -------------------------------------------------------------------------------------

using namespace ox::widgets;

struct MessageSource : VLayout {
    sl::Slot<void(std::string)> on_message;

    TextBox& text_box = append(*this,
                               TextBox{.editable = true},
                               SizePolicy::flex(),
                               FocusPolicy::Strong);

    Divider& div = append_divider(*this);

    Clicker& clicker = append(*this, Clicker{}, SizePolicy::flex(.5));

    Divider& div2 = append_divider(*this);

    Border& send_button = append(
        *this,
        Border{button({
            .text = "Send",
            .brush = {.background = ox::XColor::BrightBlue,
                      .foreground = ox::XColor::Black,
                      .traits = ox::Trait::Underline},
            .on_press_brush = {{.background = ox::XColor::Blue,
                                .foreground = ox::XColor::Black,
                                .traits = ox::Trait::Underline | ox::Trait::Bold}},
            .on_hover_brush = {{.background = ox::XColor::BrightBlue,
                                .foreground = ox::XColor::Black,
                                .traits = ox::Trait::Underline | ox::Trait::Bold}},
            .on_release =
                [on_message = on_message, &text_box = text_box] {
                    on_message(text_box.text);
                    text_box.text.clear();
                },
        })},
        SizePolicy::fixed(4));
};

struct MessageApp : HLayout {
    Divider& div = append_divider(*this, {U'┃'});

    TextBox& rhs = append(*this,
                          TextBox{
                              .text = "Right Hand Side",
                              .editable = false,
                          });

    MessageSource& lhs = insert_at(
        *this,
        0,
        MessageSource{
            .on_message = [&rhs = rhs](std::string msg) { rhs.text = std::move(msg); },
        },
        SizePolicy::flex(2));
};

// -------------------------------------------------------------------------------------

struct MessageApp2 : HLayout {
    MessageApp2()
    {
        auto& lay1 = append(*this, VLayout{});
        append_divider(*this);
        auto& lay2 = append(*this, VLayout{});
        append_divider(*this);
        auto& lay3 = append(*this, VLayout{});
        append_divider(*this);
        auto& lay4 = append(*this, VLayout{});

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
        append(lay4, Border{Clicker{}});
        append_divider(lay4);
        append(lay4, Clicker{});
        append_divider(lay4);
        append(lay4, Clicker{});
    }
};

int main()
{
    try {
        return Application{
            MessageApp{},
            {ox::MouseMode::Move},
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