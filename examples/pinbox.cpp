#include <map>
#include <string>
#include <vector>

#include <ox/ox.hpp>

using namespace ox;

class PinBox : public Widget {
   public:
    using Widget::handle;

   public:
    sl::Signal<void(Point)> pin_inserted;
    sl::Signal<void(Point)> pin_removed;

   public:
    PinBox() : Widget{FocusPolicy::None, SizePolicy::flex()} {}

   public:
    void set_color(Color c) { current_color_ = c; }

    void clear() { points_.clear(); }

   public:
    auto handle(MousePress const& m) -> EventResponse override
    {
        switch (m.button) {
            case MouseButton::Left:
                return {.handled = true, .repaint = insert_pin(m.at)};
            case MouseButton::Right:
                return {.handled = true, .repaint = remove_pin(m.at)};
            default: return {};
        }
    }

    auto handle(MouseMove const& m) -> EventResponse override
    {
        if (m.held_button == MouseButton::Left) {
            return {.handled = true, .repaint = insert_pin(m.at)};
        }
        if (m.held_button == MouseButton::Right) {
            return {.handled = true, .repaint = remove_pin(m.at)};
        }
        return {};
    }

    auto paint(Canvas c) -> Cursor override
    {
        for (auto [at, color] : points_) {
            put(c, at, U'•' | fg(color));
        }

        return {};
    }

   private:
    auto insert_pin(Point p) -> bool
    {
        auto const [_, inserted] = points_.emplace(p, current_color_);
        if (inserted) { pin_inserted.emit(p); }
        return inserted;
    }

    auto remove_pin(Point p) -> bool
    {
        auto const count = points_.erase(p);
        if (count != 0) { pin_removed.emit(p); }
        return count != 0;
    }

   private:
    std::map<Point, Color> points_;
    Color current_color_ = colors::bright_blue;
};

class ColorSelect : public Row<std::vector<Button>> {
   public:
    sl::Signal<void(Color)> on_select;

   public:
    ColorSelect(std::vector<Color> colors = {})
    {
        for (auto const color : colors) {
            children.push_back({{.label = {.style = {.background = color}}}});
            Connection{
                .signal = children.back().on_activate,
                .slot = [color](auto& parent) { parent.on_select(color); }}(*this);
        }
    }
};

int main()
{
    auto head =
        Row{
            PinBox{},

            Divider{{
                .vertical = true,
                .lines = Lines::bold(),
                .style = {.foreground = colors::bright_black},
            }},

            Column{
                Label{{
                    .text = "Color Select",
                    .align = Align::Center,
                    .style = {.traits = Trait::Bold},
                }} | SizePolicy::fixed(1),
                ColorSelect{{
                    colors::bright_blue,
                    colors::blue,
                    colors::bright_red,
                    colors::red,
                    colors::bright_green,
                    colors::green,
                    colors::white,
                    colors::bright_black,
                }} | SizePolicy::fixed(1),
                Divider{{
                    .lines = Lines::bold(),
                    .style = {.foreground = colors::bright_black},
                }},
                Label{{
                    .text = "Last Action",
                    .align = Align::Center,
                    .style = {.traits = Trait::Bold},
                    .height_policy = SizePolicy::fixed(1),
                }},
                Label{{.align = Align::Left}} | SizePolicy::fixed(1),
                Divider{{
                    .lines = Lines::bold(),
                    .style = {.foreground = colors::bright_black},
                }},
                Row{
                    Label{{
                        .text = "Pin Count",
                        .align = Align::Left,
                        .style = {.traits = Trait::Bold},
                        .width_policy = SizePolicy::fixed(10),
                    }},
                    IntegerLabel{{
                        .value = 0,
                        .align = Align::Left,
                    }},
                } | SizePolicy::fixed(1),
                Divider{{
                    .lines = Lines::bold(),
                    .style = {.foreground = colors::bright_black},
                }},
                Button{{
                    .label =
                        {
                            .text = "Clear Board",
                            .style = {.background = colors::bright_blue,
                                      .foreground = colors::black,
                                      .traits = Trait::Bold},
                        },
                    .height_policy = SizePolicy::fixed(1),
                }},
            } | SizePolicy::fixed(16),
        } |
        Border{
            .box = shape::Box::round(),
            .label = {.text = "PinBox"},
        };

    auto& [pinbox, div, sidebar] = head.child.children;

    auto& color_select = get_child<1>(sidebar);
    auto& status = get_child<4>(sidebar);
    auto& clear_btn = get_child<8>(sidebar);
    auto& count = get_child<1>(get_child<6>(sidebar));

    Connection{.signal = color_select.on_select,
               .slot = [](Color c, auto& pinbox) { pinbox.set_color(c); }}(pinbox);

    Connection{.signal = pinbox.pin_inserted,
               .slot = [](Point at, auto& status_bar, auto& count_label) {
                   auto pt_str = std::to_string(at.x) + ", " + std::to_string(at.y);
                   status_bar.text = "Added (" + std::move(pt_str) + ")";
                   count_label.value += 1;
               }}(status, count);

    Connection{.signal = pinbox.pin_removed,
               .slot = [](Point at, auto& status_bar, auto& count_label) {
                   auto pt_str = std::to_string(at.x) + ", " + std::to_string(at.y);
                   status_bar.text = "Removed (" + std::move(pt_str) + ")";
                   count_label.value -= 1;
               }}(status, count);

    Connection{.signal = clear_btn.on_activate,
               .slot = [](auto& pinbox, auto& status_bar, auto& count_label) {
                   pinbox.clear();
                   status_bar.text = "Board Cleared";
                   count_label.value = 0;
               }}(pinbox, status, count);

    return Application{head}.run({.mouse_mode = MouseMode::Drag});
}