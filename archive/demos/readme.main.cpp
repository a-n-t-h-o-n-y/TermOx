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
    Pinbox()
    {
        using namespace ox::pipe;
        *this | on_mouse_press([&](auto const& m) { this->handle_mouse(m); }) |
            on_mouse_move([&](auto const& m) { this->handle_mouse(m); }) |
            on_paint([&](ox::Painter& p) {
                for (auto [xy, color] : points_)
                    p.put(U'•' | fg(color), xy);
            });
    }

   public:
    // Set the Color for newly inserted pins.
    void set_foreground(ox::Color c) { foreground_ = c; }

    // Remove all pins from the screen.
    void clear_screen()
    {
        points_.clear();
        this->update();
    }

   private:
    std::map<ox::Point, ox::Color> points_;
    ox::Color foreground_ = ox::Color::Light_blue;

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
};

// With Classes ----------------------------------------------------------------
class Side_pane : public ox::VTuple<ox::HLabel,
                                    ox::Color_select,
                                    ox::HLabel,
                                    ox::Text_view,
                                    ox::HLine,
                                    ox::HPair<ox::HLabel, ox::Int_view>,
                                    ox::HLine,
                                    ox::Confirm_button,
                                    ox::Widget> {
   public:
    ox::HLabel& color_label        = this->get<0>();
    ox::Color_select& color_select = this->get<1>();
    ox::HLabel& status_label       = this->get<2>();
    ox::Text_view& status_box      = this->get<3>();
    ox::HLabel& count_label        = this->get<5>().first;
    ox::Int_view& count_box        = this->get<5>().second;
    ox::Confirm_button& clear_btn  = this->get<7>();
    ox::Widget& empty_space        = this->get<8>();

   public:
    Side_pane()
    {
        using namespace ox::pipe;

        *this | fixed_width(16);

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

struct Pinbox_app : ox::HPair<Pinbox, ox::Passive<ox::Bordered<Side_pane>>> {
    Pinbox& pinbox = this->first;
    Side_pane& side_pane =
        this->second | ox::pipe::take_west() | ox::pipe::wrapped();

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

int main() { return ox::System{ox::Mouse_mode::Drag}.run<Pinbox_app>(); }

// With Values and Pipes -------------------------------------------------------

auto pinbox_app()
{
    using namespace ox::pipe;
    // clang-format off
    auto pa =
        ox::hpair(
            std::make_unique<Pinbox>(),
            ox::bordered(ox::vtuple(
                ox::hlabel("- Color -" | ox::Trait::Bold) | align_center(),
                ox::color_select() | fixed_height(2),
                ox::hlabel("Status" | ox::Trait::Bold),
                ox::text_view() | fixed_height(1) | bg(ox::Color::Dark_blue),
                ox::hline(),
                ox::hpair(
                    ox::hlabel("Pin Count" | ox::Trait::Bold) | fixed_width(10),
                    ox::int_view(0)
                ) | fixed_height(1),
                ox::hline(),
                ox::confirm_button("Clear"),
                ox::widget()
            ) | fixed_width(17)) | take_west()
        );
    // clang-format on

    auto& pinbox       = pa->first;
    auto& color_select = pa->second.wrapped.get<1>();
    auto& status_box   = pa->second.wrapped.get<3>();
    auto& count_box    = pa->second.wrapped.get<5>().second;
    auto& clear_btn    = pa->second.wrapped.get<7>();

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

// int main() { return ox::System{ox::Mouse_mode::Drag}.run(*pinbox_app()); }
