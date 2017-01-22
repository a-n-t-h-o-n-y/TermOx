#ifndef COLOR_TEST_WIDGET_HPP
#define COLOR_TEST_WIDGET_HPP

#include "twidgets.hpp"
#include <random>

using namespace twf;

class Color_test_widget : public Widget {
   public:
    Color_test_widget() {
        this->set_cursor(false);
        this->enable_border();
        this->set_brush(
            Brush(background(Color::Light_blue), foreground(Color::Yellow)));
    }

    bool mouse_press_event(const Mouse_event& event) override {
        Painter p{this};
        // Generate two random colors
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<short> dist(240, 255);
        Color c_back = static_cast<Color>(dist(gen));
        Color c_fore = static_cast<Color>(dist(gen));

        // Display glyph
        Glyph g("Ʈ", background(c_back), foreground(c_fore));
        p.move(event.local_x(), event.local_y());
        p.put(g);

        // Translate Colors to English strings
        Glyph_string back_string = find_name(c_back);
        Glyph_string fore_string = find_name(c_fore);

        p.move(0, 0);
        p.put("                                    ");
        p.move(0, 0);
        p.put("Background Color: ");
        p.put(back_string);
        p.move(0, 1);
        p.put("                                    ");
        p.move(0, 1);
        p.put("Foreground Color: ");
        p.put(fore_string);
        return Widget::mouse_press_event(event);
    }

    bool paint_event(const Paint_event& event) override {
        return Widget::paint_event(event);
    }

   private:
    Glyph_string find_name(Color c) {
        Glyph_string ret;
        switch (static_cast<short>(c)) {
            case 240:
                ret = "Black";
                break;
            case 241:
                ret = "Dark Red";
                break;
            case 242:
                ret = "Dark Blue";
                break;
            case 243:
                ret = "Dark Grey";
                break;
            case 244:
                ret = "Brown";
                break;
            case 245:
                ret = "Green";
                break;
            case 246:
                ret = "Red";
                break;
            case 247:
                ret = "Grey";
                break;
            case 248:
                ret = "Blue";
                break;
            case 249:
                ret = "Orange";
                break;
            case 250:
                ret = "Light Gray";
                break;
            case 251:
                ret = "Light Green";
                break;
            case 252:
                ret = "Violet";
                break;
            case 253:
                ret = "Light Blue";
                break;
            case 254:
                ret = "Yellow";
                break;
            case 255:
                ret = "White";
                break;
        }
        return ret;
    }
};

#endif  // COLOR_TEST_WIDGET_HPP
