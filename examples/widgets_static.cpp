#include <iostream>
#include <stdexcept>

#include <ox/core.hpp>
#include <ox/static/application.hpp>
#include <ox/static/layout.hpp>

using namespace ox;

struct Clicker {
    Point at{0, 0};
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

auto main() -> int
{
    try {
        return Application{
            HLayout{
                VLayout{Clicker{}, Clicker{}},
                VLayout{Clicker{}, Clicker{}},
            },
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