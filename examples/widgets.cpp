#include <iostream>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>

#include <ox/application.hpp>
#include <ox/button.hpp>
#include <ox/label.hpp>
#include <ox/layout.hpp>
#include <ox/widget.hpp>

using namespace ox;

struct Clicker : Widget {
    sl::Signal<void(char)> on_click;

    Point clicked_at = {0, 0};
    Glyph display = U'X' | fg(XColor::Default);

    Clicker() : Widget{FocusPolicy::Strong} {}

    void paint(Canvas canvas) const override
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

struct ALayout : LinearLayout<Clicker, Clicker> {};

struct SignalTest : HLayout<Label, Button, Clicker, Label> {
    SignalTest()
    {
        auto& [label, button, clicker, label2] = this->children;

        button.on_press.connect(tracked(
            [count = 0](Label& label, Label& label2, char c, int i) mutable {
                auto text = "Clicked! " + std::to_string(count++) + " " + c + " " +
                            std::to_string(i);
                label.text = text;
                label2.text = std::move(text);
            },
            label, label2));

        button.on_press.connect([count = 0, &label, &label2](char c, int i) mutable {
            auto text = "Clicked! " + std::to_string(count++) + " " + c + " " +
                        std::to_string(i);
            label.text = text;
            label2.text = std::move(text);
        });

        connect(button.on_press,
                tracked(
                    [count = 0](Label& label, Label& label2, char c, int i) mutable {
                        auto text = "Clicked! " + std::to_string(count++) + " " + c +
                                    " " + std::to_string(i);
                        label.text = text;
                        label2.text = std::move(text);
                    },
                    label, label2));
    }
};

int main()
{
    auto head = SignalTest{};
    auto head2 = std::move(head);
    auto head3 = std::move(head2);
    return Application{head3, {MouseMode::Move}}.run();

    // auto clicker = ALayout{};
    // auto clicker = Clicker{};
    // connect(clicker.on_click,
    // tracked([](char c) { std::cout << "Clicked: " << c << std::endl; }));
    // return Application{clicker, {MouseMode::Move}}.run();
}