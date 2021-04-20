#include <map>
#include <string>

#include <termox/termox.hpp>

// Custom Widget - Mouse press to insert pins.
class Pinbox : public ox::Widget {
   public:
    // Emitted when a new pin is inserted.
    sl::Signal<void(ox::Point)> pin_inserted;

    // Emitted when an existing pin is removed.
    sl::Signal<void(ox::Point)> pin_removed;

   public:
    // Set the Color to be used for new pins inserted.
    void set_foreground(ox::Color c) { foreground_ = c; }

    // TODO single line function in README
    // Remove all pins from the screen.
    void clear_screen()
    {
        points_.clear();
        this->update();
    }

   protected:
    auto paint_event(ox::Painter& p) -> bool override
    {
        for (auto [xy, color] : points_)
            p.put(U'•' | fg(color), xy);
        return Widget::paint_event(p);
    }

    auto mouse_press_event(ox::Mouse const& m) -> bool override
    {
        this->handle_mouse(m);
        return Widget::mouse_press_event(m);
    }

    auto mouse_move_event(ox::Mouse const& m) -> bool override
    {
        this->handle_mouse(m);
        return Widget::mouse_move_event(m);
    }

   private:
    // Inserts pin at Point p, if p is empty; emits pin_inserted Signal.
    void insert_pin(ox::Point p)
    {
        auto const [_, inserted] = points_.insert({p, foreground_});
        if (inserted) {
            pin_inserted.emit(p);
            this->update();  // Post a Paint Event
        }
    }

    // Removes pin at Point p, if it exists; emits pin_removed Signal.
    void remove_pin(ox::Point p)
    {
        auto const count = points_.erase(p);
        if (count != 0) {
            pin_removed.emit(p);
            this->update();  // Post a Paint Event
        }
    }

    void handle_mouse(ox::Mouse const& m)
    {
        switch (m.button) {
            case ox::Mouse::Button::Left: this->insert_pin(m.at); break;
            case ox::Mouse::Button::Right: this->remove_pin(m.at); break;
            default: break;
        }
    }

   private:
    std::map<ox::Point, ox::Color> points_;
    ox::Color foreground_ = ox::Color::Light_blue;
};

// With Classes ----------------------------------------------------------------
class Side_pane : public ox::VTuple<ox::HLabel,
                                    ox::Color_select,
                                    ox::HLabel,
                                    ox::Text_display,
                                    ox::HLine,
                                    ox::HPair<ox::HLabel, ox::Int_display>,
                                    ox::HLine,
                                    ox::Confirm_button,
                                    ox::Widget> {
   public:
    ox::HLabel& color_label        = this->get<0>();
    ox::Color_select& color_select = this->get<1>();
    ox::HLabel& status_label       = this->get<2>();
    ox::Text_display& status_box   = this->get<3>();
    ox::HLabel& count_label        = this->get<5>().first;
    ox::Int_display& count_box     = this->get<5>().second;
    ox::Confirm_button& clear_btn  = this->get<7>();
    ox::Widget& empty_space        = this->get<8>();

   public:
    Side_pane()
    {
        using namespace ox::pipe;
        *this | fixed_width(17) | west_border();

        color_label | align_center() | text("- Color -" | ox::Trait::Bold);
        color_select | fixed_height(2);
        status_label | text("Status" | ox::Trait::Bold);
        status_box | fixed_height(1) | bg(ox::Color::Dark_blue);
        this->get<5>() | fixed_height(1);
        count_label | text("Pin Count" | ox::Trait::Bold) | fixed_width(10);
        count_box | value(0);
        clear_btn.main_btn | text("Clear") | bg(ox::Color::Dark_blue);
    }
};

struct Pinbox_app : ox::HPair<Pinbox, Side_pane> {
    Pinbox& pinbox       = this->first;
    Side_pane& side_pane = this->second;

    Pinbox_app()
    {
        using namespace ox::pipe;
        using std::to_string;
        auto& status_box = side_pane.status_box;
        auto& count_box  = side_pane.count_box;

        side_pane.color_select.color_selected.connect([&](ox::Color c) {
            pinbox.set_foreground(c);
            status_box | text(ox::color_to_string(c) + " Set");
        });

        side_pane.clear_btn.pressed.connect([&] {
            pinbox.clear_screen();
            count_box | value(0);
            status_box | text("Screen Cleared");
        });

        pinbox.pin_inserted.connect([&](ox::Point at) {
            count_box | value(count_box.value() + 1);
            status_box |
                text("Added x" + to_string(at.x) + " y" + to_string(at.y));
        });

        pinbox.pin_removed.connect([&](ox::Point at) {
            count_box | value(count_box.value() - 1);
            status_box |
                text("Removed x" + to_string(at.x) + " y" + to_string(at.y));
        });
    }
};

// int main() { return ox::System{ox::Mouse_mode::Drag}.run<Pinbox_app>(); }

// With Values and Pipes -------------------------------------------------------

auto pinbox_app()
{
    using namespace ox::pipe;
    // clang-format off
    auto pa =
        ox::hpair(
            std::make_unique<Pinbox>(),
            ox::vtuple(
                ox::hlabel("- Color -" | ox::Trait::Bold) | align_center(),
                ox::color_select() | fixed_height(2),
                ox::hlabel("Status" | ox::Trait::Bold),
                ox::text_display() | fixed_height(1) | bg(ox::Color::Dark_blue),
                ox::hline(),
                ox::hpair(
                    ox::hlabel("Pin Count" | ox::Trait::Bold) | fixed_width(10),
                    ox::int_display(0)
                ) | fixed_height(1),
                ox::hline(),
                ox::confirm_button("Clear"),
                ox::widget()
            ) | fixed_width(17) | west_border()
        );
    // clang-format on

    auto& pinbox       = pa->first;
    auto& color_select = pa->second.get<1>();
    auto& status_box   = pa->second.get<3>();
    auto& count_box    = pa->second.get<5>().second;
    auto& clear_btn    = pa->second.get<7>();

    clear_btn.main_btn | bg(ox::Color::Dark_blue);

    color_select.color_selected.connect([&](ox::Color c) {
        pinbox.set_foreground(c);
        status_box | text(color_to_string(c) + " Set");
    });

    clear_btn.pressed.connect([&] {
        pinbox.clear_screen();
        count_box | value(0);
        status_box | text("Screen Cleared");
    });

    using std::to_string;
    pinbox.pin_inserted.connect([&](ox::Point at) {
        count_box | value(count_box.value() + 1);
        status_box | text("Added x" + to_string(at.x) + " y" + to_string(at.y));
    });

    pinbox.pin_removed.connect([&](ox::Point at) {
        count_box | value(count_box.value() - 1);
        status_box |
            text("Removed x" + to_string(at.x) + " y" + to_string(at.y));
    });

    return pa;
}

int main() { return ox::System{ox::Mouse_mode::Drag}.run(*pinbox_app()); }
