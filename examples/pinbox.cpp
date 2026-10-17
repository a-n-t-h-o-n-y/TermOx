#include <iostream>
#include <map>
#include <string>
#include <string_view>
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

    void clear()
    {
        points_.clear();
        request_repaint();
    }

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

class ColorSwatch : public Button {
   public:
    Color color;
    Color ink;
    bool selected = false;

    ColorSwatch(Color color_, Color ink_) : color{color_}, ink{ink_} {}
    auto paint(Canvas canvas) -> Cursor override
    {
        auto style = Style{.background = color, .foreground = ink};
        fill(canvas, style);
        if (selected && canvas.size().width > 0)
            canvas.write({canvas.size().width / 2, 0}, U"*", style);
        if (is_focused() && canvas.size().width >= 3) {
            canvas.write({0, 0}, U"[", style);
            canvas.write({canvas.size().width - 1, 0}, U"]", style);
        }
        return {};
    }
};

using SwatchRow = Row<std::vector<ColorSwatch>>;
using PaletteLayout = Column<std::tuple<SwatchRow, SwatchRow, Label>>;
class ColorSelect : public PaletteLayout {
   public:
    struct Choice { Color color; std::string name; Color ink; };
    sl::Signal<void(Color)> on_select;

    explicit ColorSelect(std::vector<Choice> choices)
        : PaletteLayout{SwatchRow{}, SwatchRow{}, Label{{.align = Align::Center}}},
          choices_{std::move(choices)}
    {
        height_policy = SizePolicy::fixed(3);
        auto& first = get_child<0>(*this);
        auto& second = get_child<1>(*this);
        first.gap = second.gap = 1;
        first.height_policy = second.height_policy = SizePolicy::fixed(1);
        for (std::size_t i = 0; i < choices_.size(); ++i) {
            auto& row = i < 4 ? first : second;
            row.children.emplace_back(choices_[i].color, choices_[i].ink);
            Connection{
                .signal = row.children.back().on_activate,
                .slot = [i](auto& parent) { parent.select(i); }}(*this);
        }
        select(0);
    }
    auto swatch(std::size_t i) -> ColorSwatch&
    {
        return i < 4 ? get_child<0>(*this).children[i]
                     : get_child<1>(*this).children[i - 4];
    }
    void select(std::size_t index)
    {
        for (std::size_t i = 0; i < choices_.size(); ++i) {
            swatch(i).selected = i == index;
            swatch(i).refresh();
        }
        get_child<2>(*this).set_text(choices_[index].name);
        on_select(choices_[index].color);
    }

   private:
    std::vector<Choice> choices_;
};

int main(int argc, char** argv)
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
                }},
                ColorSelect{{
                    {colors::bright_blue, "Bright blue", colors::black},
                    {colors::blue, "Blue", colors::white},
                    {colors::bright_red, "Bright red", colors::black},
                    {colors::red, "Red", colors::white},
                    {colors::bright_green, "Bright green", colors::black},
                    {colors::green, "Green", colors::white},
                    {colors::white, "White", colors::black},
                    {colors::bright_black, "Gray", colors::white},
                }},
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
                Label{{.align = Align::Left}},
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
                },
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
            },
        } |
        Border{
            .box = shape::Box::round(),
            .label = {.text = "PinBox"},
        };

    auto& [pinbox, div, sidebar] = head.child.children;

    sidebar.width_policy = SizePolicy::fixed(22);
    get_child<6>(sidebar).height_policy = SizePolicy::fixed(1);
    auto& color_select = get_child<1>(sidebar);
    auto& status = get_child<4>(sidebar);
    auto& clear_btn = get_child<8>(sidebar);
    auto& count = get_child<1>(get_child<6>(sidebar));

    Connection{.signal = color_select.on_select,
               .slot = [](Color c, auto& pinbox) { pinbox.set_color(c); }}(pinbox);

    Connection{.signal = pinbox.pin_inserted,
               .slot = [](Point at, auto& status_bar, auto& count_label) {
                   auto pt_str = std::to_string(at.x) + ", " + std::to_string(at.y);
                   status_bar.set_text("Added (" + std::move(pt_str) + ")");
                   count_label.set_value(count_label.value + 1);
               }}(status, count);

    Connection{.signal = pinbox.pin_removed,
               .slot = [](Point at, auto& status_bar, auto& count_label) {
                   auto pt_str = std::to_string(at.x) + ", " + std::to_string(at.y);
                   status_bar.set_text("Removed (" + std::move(pt_str) + ")");
                   count_label.set_value(count_label.value - 1);
               }}(status, count);

    Connection{.signal = clear_btn.on_activate,
               .slot = [](auto& pinbox, auto& status_bar, auto& count_label) {
                   pinbox.clear();
                   status_bar.set_text("Board Cleared");
                   count_label.set_value(0);
               }}(pinbox, status, count);

    auto app = Application{head};
    if (argc == 2 && std::string_view{argv[1]} == "--check") {
        for (auto size : {Area{80, 24}, Area{40, 16}, Area{20, 10}, Area{1, 1}, Area{0, 0}}) {
            app.dispatch(esc::Resize{size});
            app.paint();
            if (size.width >= 40 && get_child<0>(sidebar).size().width < 12) return 2;
        }
        app.dispatch(esc::Resize{{80, 24}});
        app.paint();
        auto has_text = [&](std::u32string_view text) {
            for (int y = 0; y < app.loop().screen.size().height; ++y) {
                auto row = std::u32string{};
                for (int x = 0; x < app.loop().screen.size().width; ++x)
                    row += app.loop().screen[{x, y}].text;
                if (row.find(text) != std::u32string::npos) return true;
            }
            return false;
        };
        if (!has_text(U"Color Select") || !has_text(U"Last Action") ||
            !has_text(U"Pin Count")) return 7;
        for (std::size_t i = 0; i < 8; ++i) {
            auto& button = color_select.swatch(i);
            auto frame = Frame{button.size()};
            button.paint(Canvas{frame});
            if (frame[{0, 0}].style.background != button.color) return 3;
        }
        color_select.swatch(3).activate();
        app.paint();
        if (!has_text(U"Red") || !color_select.swatch(3).selected ||
            color_select.swatch(0).selected) return 9;
        app.set_focus(color_select.swatch(3));
        auto focused = Frame{color_select.swatch(3).size()};
        color_select.swatch(3).paint(Canvas{focused});
        if (focused[{0, 0}].text != U"[") return 10;
        pinbox.handle(MousePress{.at = {3, 4}, .button = MouseButton::Left});
        auto board = Frame{pinbox.size()};
        pinbox.paint(Canvas{board});
        if (board[{3, 4}].style.foreground != Color{colors::red}) return 11;
        if (status.text != "Added (3, 4)" || count.value != 1) return 4;
        pinbox.handle(MousePress{.at = {3, 4}, .button = MouseButton::Right});
        if (status.text != "Removed (3, 4)" || count.value != 0) return 5;
        pinbox.handle(MousePress{.at = {3, 4}, .button = MouseButton::Left});
        clear_btn.activate();
        if (status.text != "Board Cleared" || count.value != 0) return 6;
        app.paint();
        if (!has_text(U"Board Cleared")) return 8;
        std::cout << "PinBox: sidebar layout, swatches, pin actions, and clearing passed.\n";
        return 0;
    }
    if (argc != 1) {
        std::cerr << "Usage: pinbox [--check]\n";
        return 1;
    }
    return app.run({.mouse_mode = MouseMode::Drag});
}
