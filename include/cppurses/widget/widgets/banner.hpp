#ifndef CPPURSES_WIDGET_WIDGETS_BANNER_HPP
#define CPPURSES_WIDGET_WIDGETS_BANNER_HPP
#include <chrono>
#include <cstdint>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/palette/dawn_bringer16.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Single line status bar with temporary left to right animated message.
class Scan_banner : public Widget {
   public:
    Scan_banner(
        Animation_engine::Period_t framerate = std::chrono::milliseconds{100})
        : period_{framerate}
    {
        *this | pipe::fixed_height(1);
    }

   public:
    void set_message(Glyph_string message)
    {
        if (message.size() == 0uL)
            return;
        message_ = std::move(message);
        this->start();
    }

   protected:
    auto paint_event() -> bool override
    {
        auto p = Painter{*this};
        for (auto i = begin_; i != end_; ++i)
            p.put(message_[i], {i, 0uL});
        return Widget::paint_event();
    }

    auto timer_event() -> bool override
    {
        auto const m_length = message_.size();
        auto const hold_max = m_length * 3;
        if (begin_ == 0uL && end_ != m_length && hold_ == 0uL) {
            ++end_;
            this->update();
        }
        else if (begin_ == 0uL && end_ == m_length && hold_ != hold_max)
            ++hold_;
        else if (begin_ != m_length && end_ == m_length && hold_ == hold_max) {
            ++begin_;
            this->update();
        }
        else
            this->stop();
        return Widget::timer_event();
    }

   private:
    Glyph_string message_;
    std::size_t begin_;
    std::size_t end_;
    std::size_t hold_;
    Animation_engine::Period_t const period_;

   private:
    void start()
    {
        begin_ = 0uL;
        end_   = 0uL;
        hold_  = 0uL;
        this->enable_animation(period_);
    }

    void stop() { this->disable_animation(); }
};

/// This Widget takes ownership of the global color palette.
// class Rainbow_banner : public cppurses::Widget {
//    public:
//     Rainbow_banner(Animation_engine::Period_t period = std::chrono::seconds{2})
//         : period_{period}
//     {
//         // calculate framerate from period, 360 hues traversed over 2 seconds or
//         // something, be careful not to go too fast, or hold a constant
//         // framerate and instead pick a new increment interval.
//         *this | pipe::fixed_height(1);
//     }

//    public:
//     void set_message(Glyph_string message)
//     {
//         this->reset();
//         message_ = std::move(message | foreground(dynamic_color_.color));
//         this->update();
//     }

//    protected:
//     auto paint_event() -> bool override
//     {
//         Painter{*this}.put(message_, {0uL, 0uL});
//         return Widget::paint_event();
//     }

//     auto timer_event() -> bool override
//     {
//         auto pal = palette_;
//         pal.push_back({dynamic_color_, HSL{hue_, 50, 50}});
//         System::terminal.set_palette(pal);
//         if (++hue_ == 360)
//             hue_ = 0;
//         return Widget::timer_event();
//     }

//    private:
//     Glyph_string message_;
//     Animation_engine::Period_t const period_;
//     std::uint16_t hue_{0};

//     True_color_palette palette_;
//     ANSI_definition dynamic_color_;

//     inline static auto constexpr framerate_ = std::chrono::milliseconds{40};

//    private:
//     /// Grabs the current color palette and adds dynamic color to the end of it.
//     void reset()
//     {
//         this->disable_animation();
//         // TODO use dynamic color instead, then don't need a Widget, just a new
//         // color palette.
//         // palette_            = System::terminal.current_true_color_palette();
//         auto const pal_size = static_cast<Color::Value_t>(palette_.size());
//         dynamic_color_      = ANSI_definition{
//             Color{pal_size}, ANSI{static_cast<Color::Value_t>(pal_size + 16)}};
//         this->enable_animation(framerate_);
//     }
// };

// TODO
// random banner
// sliding banner
// pulse banner between two hsl colors

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_BANNER_HPP
