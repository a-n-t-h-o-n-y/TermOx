#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <ox/ox.hpp>
#include <signals_light/signal.hpp>

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

    void paint(Canvas c)
    {
        for (auto [at, color] : points_) {
            if (at.x < this->size.width && at.y < this->size.height) {
                Painter{c}[at] << (U'•' | Brush{.foreground = color});
            }
        }
    }

   private:
    void insert_pin(Point p)
    {
        auto const [_, inserted] = points_.emplace(p, current_color_);
        if (inserted) {
            pin_inserted.emit(p);
        }
    }

    void remove_pin(Point p)
    {
        auto const count = points_.erase(p);
        if (count != 0) {
            pin_removed.emit(p);
        }
    }

   private:
    std::map<Point, Color> points_;
    Color current_color_ = XColor::BrightBlue;
};

class ColorSelect : public Row<std::vector<Button>> {
   public:
    sl::Signal<void(Color)> on_select;

   public:
    ColorSelect(std::vector<Color> colors)
    {
        for (auto const color : colors) {
            children.push_back({{.brush = {.background = color}}});
            children.back().on_press.connect(
                tracked([color](auto& self) { self.on_select(color); }, *this));
        }
    }
};

int main()
{
    auto head =
        Row{
            std::tuple{
                PinBox{},
                Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                Column{std::tuple{
                    Label{{
                        .text = "Color Select",
                        .align = Label::Align::Center,
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
                        .align = Label::Align::Center,
                        .brush = {.traits = Trait::Bold},
                    }} | SizePolicy::fixed(1),
                    Label{{.align = Label::Align::Left}} | SizePolicy::fixed(1),
                    Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                    Row{std::tuple{
                        Label{{
                            .text = "Pin Count",
                            .align = Label::Align::Left,
                            .brush = {.traits = Trait::Bold},
                        }} | SizePolicy::fixed(10),
                        Label{{
                            .text = "0",
                            .align = Label::Align::Left,
                        }},
                    }} | SizePolicy::fixed(1),
                    Divider::bold({.brush = {.foreground = XColor::BrightBlack}}),
                    Button{{
                        .text = "Clear Board",
                        .brush = {.background = XColor::BrightBlue,
                                  .foreground = XColor::Black,
                                  .traits = Trait::Bold},
                    }} | SizePolicy::fixed(1),
                }} | SizePolicy::fixed(16),
            },
            [](auto& pb, auto& /*div*/, auto& sb) {
                std::get<ColorSelect>(sb.children)
                    .on_select.connect(
                        tracked([](auto& pb, Color c) { pb.set_color(c); }, pb));

                pb.pin_inserted.connect(tracked(
                    [](auto& status_bar, auto& count_label, Point at) {
                        status_bar.text = "Added (" + std::to_string(at.x) + ", " +
                                          std::to_string(at.y) + ")";
                        count_label.text =
                            std::to_string(std::stoi(count_label.text) + 1);
                    },
                    std::get<4>(sb.children),
                    std::get<1>(std::get<6>(sb.children).children)));

                pb.pin_removed.connect(tracked(
                    [](auto& status_bar, auto& count_label, Point at) {
                        status_bar.text = "Removed (" + std::to_string(at.x) + ", " +
                                          std::to_string(at.y) + ")";
                        count_label.text =
                            std::to_string(std::stoi(count_label.text) - 1);
                    },
                    std::get<4>(sb.children),
                    std::get<1>(std::get<6>(sb.children).children)));

                std::get<Button>(sb.children)
                    .on_press.connect(tracked(
                        [](auto& pb, auto& status_bar) {
                            pb.clear();
                            status_bar.text = "Board Cleared";
                        },
                        pb, std::get<4>(sb.children)));
            },
        } |
        Border::round("PinBox");

    return Application{head, {MouseMode::Drag}}.run();
}