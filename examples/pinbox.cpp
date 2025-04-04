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
            children.back().on_press.connect(
                tracked([color](auto& self) { self.on_select(color); }, *this));
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
                }} | SizePolicy::fixed(1),
                Label{{.align = Align::Left}} | SizePolicy::fixed(1),
                Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                Row{
                    Label{{
                        .text = "Pin Count",
                        .align = Align::Left,
                        .brush = {.traits = Trait::Bold},
                    }} | SizePolicy::fixed(10),
                    Label{{
                        .text = "0",
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
                }} | SizePolicy::fixed(1),
            } | SizePolicy::fixed(16),
        }
            .init([](auto& pinbox, auto& /*div*/, auto& sidebar) {
                auto& [l1, color_select, div1, l2, status, div2, count, div3,
                       clear_btn] = sidebar.children;

                color_select.on_select.connect(tracked(
                    [](auto& pinbox, Color c) { pinbox.set_color(c); }, pinbox));

                pinbox.pin_inserted.connect(tracked(
                    [](auto& status_bar, auto& count_label, Point at) {
                        status_bar.text = "Added (" + std::to_string(at.x) + ", " +
                                          std::to_string(at.y) + ")";
                        count_label.text =
                            std::to_string(std::stoi(count_label.text) + 1);
                    },
                    status, std::get<1>(count.children)));

                pinbox.pin_removed.connect(tracked(
                    [](auto& status_bar, auto& count_label, Point at) {
                        status_bar.text = "Removed (" + std::to_string(at.x) + ", " +
                                          std::to_string(at.y) + ")";
                        count_label.text =
                            std::to_string(std::stoi(count_label.text) - 1);
                    },
                    status, std::get<1>(count.children)));

                std::get<Button>(sidebar.children)
                    .on_press.connect(tracked(
                        [](auto& pinbox, auto& status_bar, auto& count_label) {
                            pinbox.clear();
                            status_bar.text = "Board Cleared";
                            count_label.text = "0";
                        },
                        pinbox, status, std::get<1>(count.children)));
            }) |
        Border::round("PinBox");

    return Application{head, {MouseMode::Drag}}.run();
}