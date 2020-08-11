#ifndef CPPURSES_WIDGET_WIDGETS_BANNER_HPP
#define CPPURSES_WIDGET_WIDGETS_BANNER_HPP
#include <chrono>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Single line status bar with temporary left to right animated message.
class Cascade_banner : public Widget {
   public:
    Cascade_banner(
        Animation_engine::Period_t framerate = std::chrono::milliseconds{100})
        : period_{framerate}
    {
        *this | pipe::fixed_height(1uL);
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
            p.put(message_[i], {i, 0});
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
    const Animation_engine::Period_t period_;

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

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_BANNER_HPP
