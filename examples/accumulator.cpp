#include <ox/ox.hpp>

#include <tuple>
#include <vector>

using namespace ox;

struct NumberButton : Button {
    NumberButton(int n = 0)
        : Button{{
              .text = std::to_string(n),
              .brush = {.foreground = XColor::BrightBlack, .traits = Trait::None},
              .on_press_brush =
                  [](Brush b) {
                      b.foreground = XColor::BrightMagenta;
                      b.traits = Trait::Bold;
                      return b;
                  },
              .on_hover_brush =
                  [](Brush b) {
                      b.foreground = XColor::White;
                      b.traits = Trait::Bold;
                      return b;
                  },
          }}
    {}

    auto paint(Canvas c) -> void override
    {
        Button::paint(c);
        // line across bottom
        Painter{c}[{
            .x = this->size.width > 1 ? 1 : 0,
            .y = std::max(this->size.height - 1, 0),
        }] << Painter::HLine{.length = this->size.width,
                             .glyph = U'─' | fg(XColor::BrightCyan)};

        // line down right side
        Painter{c}[{
            .x = std::max(this->size.width - 1, 0),
            .y = this->size.height > 1 ? 1 : 0,
        }] << Painter::VLine{.length = this->size.height,
                             .glyph = U'│' | fg(XColor::BrightCyan)};

        auto const bottom_right = Point{
            .x = std::max(this->size.width - 1, 0),
            .y = std::max(this->size.height - 1, 0),
        };
        c[bottom_right] = U'╯' | fg(XColor::BrightCyan);
    }
};

template <typename WidgetType>
using Grid = Column<std::vector<Row<std::vector<WidgetType>>>>;

struct NumberPad : Grid<NumberButton> {
    sl::Signal<void(int)> on_press;

    NumberPad()
    {
        auto const WIDTH = 4;
        auto const HEIGHT = 4;
        auto i = 1;
        for (auto y = 0; y < HEIGHT; ++y) {
            auto& row = this->children.emplace_back();
            for (auto x = 0; x < WIDTH; ++x) {
                row.children.push_back(i);
                row.children.back().on_press.connect(
                    tracked([i](auto& self) { self.on_press(i); }, *this));
                i += i;
            }
        }
    }
};

int main()
{
    auto head =
        Column{
            std::tuple{
                Label{{.text = "Total", .align = Align::Left}} | SizePolicy::fixed(1),
                IntegerLabel{{
                    .value = 0,
                    .align = Align::Right,
                    .brush = {.traits = Trait::Bold},
                    .locale = digit_separator_locale(1),
                }} | SizePolicy::fixed(1),
                Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                NumberPad{},
            },
            [](auto&, auto& total_label, auto&, auto& numberpad) {
                numberpad.on_press.connect(
                    tracked([](auto& total_label, int n) { total_label.value += n; },
                            total_label));
            }} |
        Border::bold("Accumulator");

    return Application{head, Terminal{MouseMode::Move}}.run();
}