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
                .brush = {.foreground = XColor::BrightBlack, .traits = Trait::None},
            },
        .decoration =
            Fade{
                .paint_fn = shape_gradient(shape::Box::brackets_round(), RGB{0x0a3f46},
                                           RGB{0x1ecbe1}),
                .fade_in = std::chrono::milliseconds{300},
                .fade_out = std::chrono::milliseconds{500},
            },
        .press_mod = [](Label& l) { l.brush.foreground = XColor::BrightWhite; },
        .focus_mod = [](Label& l) { l.brush.foreground = XColor::BrightCyan; },
    }};
}

template <typename WidgetType>
using Grid = Column<std::vector<Row<std::vector<WidgetType>>>>;

/**
 * A grid of buttons that emit their integer when pressed.
 */
class NumberPad : public Grid<Button> {
   public:
    sl::Signal<void(int)> on_press;

   public:
    NumberPad(int width, int height)
    {
        auto i = 1;
        for (auto y = 0; y < height; ++y) {
            auto& row = this->children.emplace_back();
            for (auto x = 0; x < width; ++x) {
                auto& child = row.children.emplace_back(int_button(i));
                Connection{
                    .signal = child.on_press,
                    .slot = [i](auto& parent) { parent.on_press(i); },
                }(*this);
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
                .size_policy = SizePolicy::fixed(1),
            }},
            IntegerLabel{{
                .value = 0,
                .align = Align::Right,
                .brush = {.traits = Trait::Bold},
                .locale = digit_separator_locale(1),
                .size_policy = SizePolicy::fixed(1),
            }},
            Divider{{
                .lines = Lines::bold(),
                .brush = {.foreground = XColor::BrightBlack},
            }},
            NumberPad{4, 4},
        } |
        Border{
            .box = shape::Box::bold(),
            .label = "Accumulator",
        };

    auto& [title_, total_label, div_, numberpad] = head.child.children;

    Connection{
        .signal = numberpad.on_press,
        .slot = [](int n, auto& total_label) { total_label.value += n; },
    }(total_label);

    return Application{head, Terminal{MouseMode::Move}}.run();
}