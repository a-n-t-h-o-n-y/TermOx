#include <array>
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

class ColorSelect : public Widget {
   public:
    using Widget::handle;
    sl::Signal<void(Color)> on_select;

    ColorSelect() : Widget{FocusPolicy::Strong, SizePolicy::flex(), SizePolicy::fixed(5)} {}

    auto paint(Canvas canvas) -> Cursor override
    {
        for (int i = 0; i < 8; ++i) {
            auto const bounds = swatch(i);
            auto tile = canvas.subcanvas(bounds.at, bounds.size);
            auto const style = Style{.background = palette_[i].color,
                                     .foreground = palette_[i].ink};
            paint_label(tile.subcanvas({1, 0}, {std::max(0, bounds.size.width - 2), 1}),
                        i == selected_ ? "✓" : "", Align::Center, style);
            if (is_focused() && i == current_ && bounds.size.width >= 3) {
                put(tile, {0, 0}, U'[');
                put(tile, {bounds.size.width - 1, 0}, U']');
            }
        }
        put_label(canvas.subcanvas({0, 4}, {canvas.size().width, 1}),
                  palette_[selected_].name, Align::Center, {});
        return {};
    }

    auto handle(KeyEvent const& event) -> EventResponse override
    {
        if (event.action == KeyAction::Release) return {};
        if (activates(event)) return select();
        auto next = current_;
        if (event.matches(KeyCode::ArrowLeft, {}, event.action)) {
            if (next % 4 != 0) --next;
        }
        else if (event.matches(KeyCode::ArrowRight, {}, event.action)) {
            if (next % 4 != 3) ++next;
        }
        else if (event.matches(KeyCode::ArrowUp, {}, event.action)) next = next % 4;
        else if (event.matches(KeyCode::ArrowDown, {}, event.action)) next = next % 4 + 4;
        else return {};
        auto const changed = next != current_;
        current_ = next;
        return {.handled = true, .repaint = changed};
    }

    auto handle(MousePress const& event) -> EventResponse override
    {
        if (event.button != MouseButton::Left) return {};
        for (int i = 0; i < 8; ++i) {
            if (contains(swatch(i), event.at)) {
                current_ = i;
                return select();
            }
        }
        return {};
    }

   private:
    struct Swatch { Color color; Color ink; std::string_view name; };
    std::array<Swatch, 8> palette_{{
        {colors::bright_blue, colors::black, "Bright blue"},
        {colors::blue, colors::bright_white, "Blue"},
        {colors::bright_red, colors::black, "Bright red"},
        {colors::red, colors::bright_white, "Red"},
        {colors::bright_green, colors::black, "Bright green"},
        {colors::green, colors::black, "Green"},
        {colors::white, colors::black, "White"},
        {colors::bright_black, colors::bright_white, "Gray"},
    }};
    int selected_ = 0;
    int current_ = 0;

    auto swatch(int index) const -> Rect
    {
        auto const column = index % 4;
        auto const left = column * size().width / 4;
        auto const right = (column + 1) * size().width / 4;
        return {{left, (index / 4) * 2}, {right - left, 1}};
    }

    auto select() -> EventResponse
    {
        selected_ = current_;
        request_repaint();
        on_select(palette_[selected_].color);
        return {.handled = true, .repaint = true};
    }
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
                    .text = "Color",
                    .align = Align::Center,
                    .style = {.traits = Trait::Bold},
                }},
                ColorSelect{},
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

    sidebar.width_policy = SizePolicy::fixed(20);
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
        if (!has_text(U"Color") || !has_text(U"Last Action") ||
            !has_text(U"Pin Count")) return 7;
        if (!has_text(U"Bright blue") || !has_text(U"✓")) return 3;
        app.set_focus(color_select);
        app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
        app.paint();
        if (!has_text(U"Bright blue")) return 9; // Focus alone keeps the selection.
        app.dispatch(KeyEvent{.key = KeyCode::Enter});
        app.paint();
        if (!has_text(U"Blue") || has_text(U"Bright blue")) return 10;
        color_select.handle(MousePress{.at = {12, 2}, .button = MouseButton::Left});
        app.paint();
        if (!has_text(U"White")) return 11;
        pinbox.handle(MousePress{.at = {3, 4}, .button = MouseButton::Left});
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
