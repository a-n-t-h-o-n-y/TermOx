#include <twidgets.hpp>
// #include <thread>
// #include <chrono>
#include <functional>

// #include <boost/asio.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>

// class Animated_widget : public twf::Widget {
//    protected:
//     bool paint_event(const twf::Paint_event& event) override {
//         if (!started_) {
//             started_ = true;
//             t_.async_wait(
//                 std::bind(std::bind(&Animated_widget::launch_animation, this)));
//         }
//         twf::Painter p{this};
//         if (state_1_) {
//             p.put_at(0, 0, twf::Glyph("*", twf::foreground(twf::Color::Green)));
//         } else {
//             p.put_at(0, 0, " ");
//         }
//         return Widget::paint_event(event);
//     }

//    private:
//     void launch_animation() {
//         // update state
//         state_1_ = !state_1_;

//         // update widget
//         this->update();

//         // call another timer
//         t_.async_wait(std::bind(&Animated_widget::launch_animation, this));
//     }

//     // State
//     bool state_1_{true};
//     bool started_{false};

//     boost::asio::io_service io_;
//     boost::asio::deadline_timer t_{io_, boost::posix_time::seconds(1)};
// };

// class Animated_layout : public twf::Horizontal_layout {
//    public:
//     Animated_layout() { this->make_child<Animated_widget>(); }
// };

int main() {
    twf::System s;
    // Animated_layout w;
    twf::Widget w;

    s.set_head(&w);
    return s.run();
}
