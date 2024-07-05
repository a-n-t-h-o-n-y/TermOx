#include <iostream>

#include <signals_light/signal.hpp>

#include <ox/ox.hpp>

using namespace ox;

struct Clicker : Widget {
    sl::Signal<void(char)> on_click;

    Point clicked_at = {0, 0};
    Glyph display = U'X' | fg(XColor::Default);

    Clicker() : Widget{FocusPolicy::Strong} {}

    void paint(Canvas canvas) override
    {
        if (clicked_at.x < canvas.size.width && clicked_at.y < canvas.size.height) {
            canvas[clicked_at] = display;
        }
    }

    void mouse_press(Mouse m) override
    {
        if (m.button == Mouse::Button::Left) {
            on_click('L');
        }
        else if (m.button == Mouse::Button::Right) {
            on_click('R');
        }
    }

    void mouse_release(Mouse m) override
    {
        if (m.button == Mouse::Button::Left) {
            clicked_at = m.at;
        }
    }

    void mouse_move(Mouse m) override { clicked_at = m.at; }

    void mouse_wheel(Mouse m) override
    {
        if (m.button == Mouse::Button::ScrollUp) {
            display = display | bg(XColor::BrightBlue);
        }
        else if (m.button == Mouse::Button::ScrollDown) {
            display = display | bg(XColor::BrightRed);
        }
    }

    void focus_in() override { display = display | fg(XColor::BrightYellow); }

    void focus_out() override { display = display | fg(XColor::Default); }

    // TODO remove
    void resize(Area new_size) override { Widget::size = new_size; }
};

struct ALayout : VLayout<Clicker, Clicker> {};

struct SignalTest : VLayout<Label, Button, Clicker, Label> {
    SignalTest()
        : VLayout<Label, Button, Clicker, Label>{
              {
                  "Label 1",
                  Label::Align::Center,
              },
              {
                  "Click me!",
                  {},
                  Brush{.background = XColor::Blue, .foreground = XColor::Black},
                  Brush{.background = XColor::BrightBlue, .foreground = XColor::Black},
              },
              {},
              {
                  "Label 2",
                  Label::Align::Right,
              },
          }
    {
        auto& [label1, button, clicker, label2] = this->children;

        button.on_press.connect(tracked(
            [count = 0](Label& label1, Label& label2) mutable {
                auto text = "Clicked! " + std::to_string(count++);
                label1.text = text;
                label2.text = std::move(text);
            },
            label1, label2));
    }
};

auto signal_test()
{
    auto head = VVector<Widget>{};

    // head.append(Label{
    //     "Hello, world! 1",
    //     Label::Align::Center,
    //     Brush{
    //         .background = XColor::BrightBlue,
    //         .foreground = XColor::Black,
    //         .traits = Trait::Bold,
    //     },
    // });

    // head.append(Label{
    //     "Hello, world! 2",
    //     Label::Align::Right,
    //     Brush{
    //         .background = XColor::BrightRed,
    //         .foreground = XColor::Black,
    //     },
    // });

    auto& tb = head.append(TextBox{
        U"Hello, world!\nThis is a new line!\n\nThis is another line after two "
        "newlines!"_gs,
        TextBox::Wrap::Word,
        TextBox::Align::Left,
    });
    tb.background = XColor::BrightGreen;
    tb.focus_policy = FocusPolicy::Strong;

    // head.insert_at(0, Label{
    //                       "Hello, world! 0",
    //                       Label::Align::Left,
    //                       Brush{
    //                           .background = XColor::BrightGreen,
    //                           .foreground = XColor::Black,
    //                       },
    //                   });

    return head;
}

int main()
{
    // auto head = SignalTest{};
    auto head = signal_test();
    auto head2 = std::move(head);
    auto head3 = std::move(head2);
    return Application{head3, {MouseMode::Move}}.run();

    // auto clicker = ALayout{};
    // auto clicker = Clicker{};
    // connect(clicker.on_click,
    // tracked([](char c) { std::cout << "Clicked: " << c << std::endl; }));
    // return Application{clicker, {MouseMode::Move}}.run();
}