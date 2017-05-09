#include <twidgets.hpp>
#include <thread>
#include <chrono>

class Animated_widget : public twf::Widget {
   public:
    Animated_widget(int fps) : fps_{fps} {
        this->enable_border();
        std::thread([this]() { this->launch_animation(); }).detach();
    }

   protected:
    bool paint_event(const twf::Paint_event& event) override {
        twf::Painter p{this};
        if (state_1_) {
            p.put_at(0, 0,
                     twf::Glyph_string("* * * * * * * * * * * * * * * * * * *",
                                       twf::foreground(twf::Color::White)));
            this->set_background(twf::Color::Violet);
        } else {
            p.put_at(0, 0,
                     twf::Glyph_string("* * * * * * * * * * * * * * * * * * *",
                                       twf::foreground(twf::Color::Light_gray)));
            this->set_background(twf::Color::Light_green);
        }
        return Widget::paint_event(event);
    }

   private:
    void launch_animation() {
        while (true) {
            // wait on timer
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<int>((1 / static_cast<float>(fps_)) * 1000)));

            // update state
            state_1_ = !state_1_;

            // paint
            this->update_now();
        }
    }

    // State
    bool state_1_{true};
    int fps_;
};

class Animated_layout : public twf::Horizontal_layout {
   public:
    Animated_layout() { this->make_child<Animated_widget>(10); }
};

int main() {
    twf::System s;
    // Animated_layout w;
    twf::Horizontal_layout w;
    w.make_child<Animated_widget>(5);
    w.make_child<Animated_widget>(2);
    w.make_child<twf::Textbox>().enable_border();
    w.make_child<Animated_widget>(1);

    s.set_head(&w);
    return s.run();
}
