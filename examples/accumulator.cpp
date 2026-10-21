#include <ox/ox.hpp>

#include <vector>

using namespace ox;

/**
 * Create a Button with an integer display and a fade decoration.
 */
[[nodiscard]] auto int_button(int n) -> Button
{
    return {{
        .label =
            {
                .text = std::to_string(n),
                .style = {.foreground = colors::bright_black, .traits = Trait::None},
            },
        .decoration =
            Fade{
                .paint_fn = shape_gradient(shape::Box::brackets_round(), RGB{0x0a3f46},
                                           RGB{0x1ecbe1}),
                .fade_in = std::chrono::milliseconds{300},
                .fade_out = std::chrono::milliseconds{500},
            },
        .pressed_style = {.background = {},
                          .foreground = colors::bright_white,
                          .enabled = {},
                          .disabled = {}},
        .focused_style = {.background = {},
                          .foreground = colors::bright_cyan,
                          .enabled = {},
                          .disabled = {}},
        .height_policy = SizePolicy::flex(),
    }};
}

template <typename WidgetType>
using Grid = Column<std::vector<Row<std::vector<WidgetType>>>>;

/**
 * A grid of buttons that emit their integer when pressed.
 */
class NumberPad : public Grid<Button> {
   public:
    sl::Signal<void(int)> on_activate;

   public:
    NumberPad(int width, int height)
    {
        auto i = 1;
        for (auto y = 0; y < height; ++y) {
            auto& row = this->children.emplace_back();
            for (auto x = 0; x < width; ++x) {
                auto& child = row.children.emplace_back(int_button(i));
                Connection{.signal = child.on_activate,
                           .slot = [i](auto& parent) { parent.on_activate(i); }}(*this);
                i += i;
            }
        }
    }
};

int main()
{
    auto head =
        Column{
            Label{{
                .text = "Total",
                .align = Align::Left,
                .height_policy = SizePolicy::fixed(1),
            }},
            IntegerLabel{{
                .value = 0,
                .align = Align::Right,
                .style = {.traits = Trait::Bold},
                .locale = digit_separator_locale(1),
                .height_policy = SizePolicy::fixed(1),
            }},
            Divider{{
                .lines = Lines::bold(),
                .style = {.foreground = colors::bright_black},
            }},
            NumberPad{4, 4},
        } |
        Border{
            .box = shape::Box::bold(),
            .label = {.text = "Accumulator"},
        };

    auto& [title_, total_label, div_, numberpad] = head.child.children;

    Connection{.signal = numberpad.on_activate, .slot = [](int n, auto& total_label) {
                   total_label.set_value(total_label.value + n);
               }}(total_label);

    return Application{head}.run({.mouse_mode = MouseMode::Move});
}
