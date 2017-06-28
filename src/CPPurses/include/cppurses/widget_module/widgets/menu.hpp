#ifndef WIDGET_MODULE_WIDGETS_MENU_HPP
#define WIDGET_MODULE_WIDGETS_MENU_HPP

#include "painter_module/attribute.hpp"
#include "painter_module/color.hpp"
#include "painter_module/glyph_string.hpp"
#include "painter_module/painter.hpp"
#include "system_module/key.hpp"
#include "system_module/events/key_event.hpp"
#include "widget_module/widget.hpp"
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

struct Menu_item {
    Menu_item(Glyph_string title_, std::function<void()> action_)
        : title{std::move(title_)}, action{std::move(action_)} {}
    Glyph_string title;
    std::function<void()> action;
};

class Menu : public Widget {
   public:
    explicit Menu(Glyph_string title) : menu_title_{std::move(title)} {
        this->set_focus_policy(Focus_policy::Strong);
    }

    void add_item(Menu_item item) { items_.emplace_back(item); }

    void make_item(Glyph_string title, std::function<void()> action) {
        items_.emplace_back(title, action);
    }

    bool paint_event(const Paint_event& event) override {
        Painter p{this};
        p.put(menu_title_);
        int count{1};
        for (const auto& mi : items_) {
            Glyph_string display{Glyph_string{std::to_string(count) + ". "} +
                                 mi.title};
            if (count == highlight_) {
                display.add_attributes(Attribute::Bold);
            }
            p.put(display, 0, count);
            ++count;
        }
        return Widget::paint_event(event);
    }

    bool key_press_event(const Key_event& event) override {
        if (event.key_code() == Key::Arrow_down ||
            event.key_code() == Key::Arrow_left) {
            if (highlight_ != items_.size()) {
                ++highlight_;
            }
        } else if (event.key_code() == Key::Arrow_up ||
                   event.key_code() == Key::Arrow_right) {
            if (highlight_ != 1) {
                --highlight_;
            }
        } else if (event.key_code() == Key::Enter) {
            if (!items_.empty()) {
                items_.at(highlight_ - 1).action();
            }
        }
        this->update();
        return true;
    }

   private:
    std::vector<Menu_item> items_;
    int highlight_{1};
    Glyph_string menu_title_;
};

}  // namespace cppurses

#endif  // WIDGET_MODULE_WIDGETS_MENU_HPP
