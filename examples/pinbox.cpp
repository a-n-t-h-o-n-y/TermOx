#include <map>
#include <string>
#include <vector>

#include <ox/ox.hpp>

using namespace ox;

class PinBox : public Widget {
   public:
    sl::Signal<void(Point)> pin_inserted;
    sl::Signal<void(Point)> pin_removed;

   public:
    PinBox() : Widget{FocusPolicy::None, SizePolicy::flex()} {}

   public:
    void set_color(Color c) { current_color_ = c; }

    void clear() { points_.clear(); }

   public:
    void mouse_press(Mouse m) override
    {
        switch (m.button) {
            case Mouse::Button::Left: this->insert_pin(m.at); break;
            case Mouse::Button::Right: this->remove_pin(m.at); break;
            default: break;
        }
    }

    void mouse_move(Mouse m) override
    {
        switch (m.button) {
            case Mouse::Button::Left: this->insert_pin(m.at); break;
            case Mouse::Button::Right: this->remove_pin(m.at); break;
            default: break;
        }
    }

    void paint(Canvas c) override
    {
        for (auto [at, color] : points_) {
            put(c, at, U'•' | fg(color));
        }
    }

   private:
    void insert_pin(Point p)
    {
        auto const [_, inserted] = points_.emplace(p, current_color_);
        if (inserted) { pin_inserted.emit(p); }
    }

    void remove_pin(Point p)
    {
        auto const count = points_.erase(p);
        if (count != 0) { pin_removed.emit(p); }
    }

   private:
    std::map<Point, Color> points_;
    Color current_color_ = XColor::BrightBlue;
};

class ColorSelect : public Row<std::vector<Button>> {
   public:
    sl::Signal<void(Color)> on_select;

   public:
    ColorSelect(std::vector<Color> colors = {})
    {
        for (auto const color : colors) {
            children.push_back({{.label = {.brush = {.background = color}}}});
            Connection{
                .signal = children.back().on_press,
                .slot = [color](auto& parent) { parent.on_select(color); },
            }(*this);
        }
    }
};

int main()
{
    auto head =
        Row{
            PinBox{},

            Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),

            Column{
                Label{{
                    .text = "Color Select",
                    .align = Align::Center,
                    .brush = {.traits = Trait::Bold},
                }} | SizePolicy::fixed(1),
                ColorSelect{{
                    XColor::BrightBlue,
                    XColor::Blue,
                    XColor::BrightRed,
                    XColor::Red,
                    XColor::BrightGreen,
                    XColor::Green,
                    XColor::White,
                    XColor::BrightBlack,
                }} | SizePolicy::fixed(1),
                Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                Label{{
                    .text = "Last Action",
                    .align = Align::Center,
                    .brush = {.traits = Trait::Bold},
                    .size_policy = SizePolicy::fixed(1),
                }},
                Label{{.align = Align::Left}} | SizePolicy::fixed(1),
                Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                Row{
                    Label{{
                        .text = "Pin Count",
                        .align = Align::Left,
                        .brush = {.traits = Trait::Bold},
                        .size_policy = SizePolicy::fixed(10),
                    }},
                    IntegerLabel{{
                        .value = 0,
                        .align = Align::Left,
                    }},
                } | SizePolicy::fixed(1),
                Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                Button{{
                    .label =
                        {
                            .text = "Clear Board",
                            .brush = {.background = XColor::BrightBlue,
                                      .foreground = XColor::Black,
                                      .traits = Trait::Bold},
                        },
                    .size_policy = SizePolicy::fixed(1),
                }},
            } | SizePolicy::fixed(16),
        } |
        Border::round("PinBox");

    auto& [pinbox, div, sidebar] = head.child.children;

    auto& color_select = get_child<1>(sidebar);
    auto& status = get_child<4>(sidebar);
    auto& clear_btn = get_child<8>(sidebar);
    auto& count = get_child<1>(get_child<6>(sidebar));

    Connection{
        .signal = color_select.on_select,
        .slot = [](Color c, auto& pinbox) { pinbox.set_color(c); },
    }(pinbox);

    Connection{
        .signal = pinbox.pin_inserted,
        .slot =
            [](Point at, auto& status_bar, auto& count_label) {
                auto pt_str = std::to_string(at.x) + ", " + std::to_string(at.y);
                status_bar.text = "Added (" + std::move(pt_str) + ")";
                count_label.value += 1;
            },
    }(status, count);

    Connection{
        .signal = pinbox.pin_removed,
        .slot =
            [](Point at, auto& status_bar, auto& count_label) {
                auto pt_str = std::to_string(at.x) + ", " + std::to_string(at.y);
                status_bar.text = "Removed (" + std::move(pt_str) + ")";
                count_label.value -= 1;
            },
    }(status, count);

    Connection{
        .signal = clear_btn.on_press,
        .slot =
            [](auto& pinbox, auto& status_bar, auto& count_label) {
                pinbox.clear();
                status_bar.text = "Board Cleared";
                count_label.value = 0;
            },
    }(pinbox, status, count);

    return Application{head, {MouseMode::Drag}}.run();
}