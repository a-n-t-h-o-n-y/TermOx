#ifndef TERMOX_WIDGET_WIDGETS_BANNER_HPP
#define TERMOX_WIDGET_WIDGETS_BANNER_HPP
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

struct Index_and_position {
    std::size_t index;
    int position;
};

auto operator++(Index_and_position& x) -> Index_and_position&;

class IP_range {
   public:
    using Iter_t = std::vector<Index_and_position>::const_iterator;

   public:
    IP_range(Iter_t begin, Iter_t end);

   public:
    [[nodiscard]] auto begin() const -> Iter_t;

    [[nodiscard]] auto end() const -> Iter_t;

   private:
    Iter_t begin_;
    Iter_t end_;
};

/// Animated Text Line/Label
template <typename Animator>
class Banner : public Widget {
   public:
    using Interval_t = std::chrono::milliseconds;

    struct Parameters {
        Glyph_string text   = U"";
        Interval_t interval = Interval_t{50};
        Animator animator   = Animator{};
    };

   public:
    explicit Banner(Glyph_string text   = U"",
                    Interval_t interval = std::chrono::milliseconds{50},
                    Animator animator   = Animator{})
        : text_{text}, interval_{interval}, animator_{animator}
    {
        *this | pipe::fixed_height(1);
        animator_.start.connect([this] { this->start(); });
        animator_.stop.connect([this] { this->stop(); });
    }

    explicit Banner(Parameters p)
        : Banner{std::move(p.text), std::move(p.interval),
                 std::move(p.animator)}
    {}

   public:
    void set_text(Glyph_string text)
    {
        this->stop();
        text_ = std::move(text);
        range_.reset();
        animator_.set_text_length(text_.size());
        this->update();
    }

    [[nodiscard]] auto text() const noexcept -> Glyph_string const&
    {
        return text_;
    }

    void set_interval(Interval_t interval)
    {
        auto const running = this->Widget::is_animated();
        this->stop();
        interval_ = interval;
        if (running)
            this->start();
    }

    [[nodiscard]] auto interval() const noexcept -> Interval_t
    {
        return interval_;
    }

    [[nodiscard]] auto animator() const noexcept -> Animator const&
    {
        return animator_;
    }

    [[nodiscard]] auto animator() noexcept -> Animator& { return animator_; }

   protected:
    auto paint_event(Painter& p) -> bool override
    {
        if (range_) {
            for (auto const& x : *range_)
                p.put(text_[x.index], {x.position, 0});
        }
        else
            p.put(text_, {0, 0});
        return Widget::paint_event(p);
    }

    auto timer_event() -> bool override
    {
        range_ = animator_();
        this->update();
        return Widget::timer_event();
    }

    auto resize_event(Area new_size, Area old_size) -> bool override
    {
        animator_.set_max_length(new_size.width);
        return Widget::resize_event(new_size, old_size);
    }

   private:
    Glyph_string text_;
    Interval_t interval_;
    Animator animator_;
    std::optional<IP_range> range_;

   private:
    void start()
    {
        if (text_.size() == 0)
            return;
        this->enable_animation(interval_);
    }

    void stop()
    {
        this->disable_animation();
        range_.reset();
    }
};

/// Helper function to create a Banner instance.
template <typename Animator>
[[nodiscard]] auto banner(Glyph_string text = U"",
                          typename Banner<Animator>::Interval_t interval =
                              std::chrono::milliseconds{50},
                          Animator animator = Animator{})
    -> std::unique_ptr<Banner<Animator>>
{
    return std::make_unique<Banner<Animator>>(std::move(text), interval,
                                              std::move(animator));
}

/// Helper function to create a Banner instance.
template <typename Animator>
[[nodiscard]] auto banner(typename Banner<Animator>::Parameters p)
    -> std::unique_ptr<Banner<Animator>>
{
    return std::make_unique<Banner<Animator>>(std::move(p));
}

}  // namespace ox

namespace ox::animator {

class Animator_base {
   public:
    sl::Signal<void()> start;
    sl::Signal<void()> stop;

   protected:
    Animator_base();

   public:
    void set_max_length(std::size_t x);

    void set_text_length(std::size_t x);

   protected:
    [[nodiscard]] auto data() const -> std::vector<Index_and_position> const&;

    [[nodiscard]] auto data() -> std::vector<Index_and_position>&;

    /// Incremented values, length of text.
    void initialize_data();

    [[nodiscard]] auto max_length() const -> std::size_t;

    [[nodiscard]] auto text_length() const -> std::size_t;

    [[nodiscard]] auto is_started() const -> bool;

    [[nodiscard]] auto begin() -> std::vector<Index_and_position>::iterator;

    [[nodiscard]] auto begin() const
        -> std::vector<Index_and_position>::const_iterator;

    [[nodiscard]] auto end() -> std::vector<Index_and_position>::iterator;

    [[nodiscard]] auto end() const
        -> std::vector<Index_and_position>::const_iterator;

   private:
    std::vector<Index_and_position> data_;
    std::size_t max_length_;
    std::size_t text_length_;
    bool started_ = false;
};

/* ------------------- Banner Animation Implementations ----------------------*/

/// Left to right reveal of text, hold, left to right clearning of text.
class Scan : public Animator_base {
   public:
    [[nodiscard]] auto operator()() -> IP_range;

    void set_text_length(std::size_t x);

   private:
    std::size_t begin_;
    std::size_t end_;
    std::size_t hold_;
};

/// Left to right reveal of text, then hold.
class Persistent_scan : public Animator_base {
   public:
    [[nodiscard]] auto operator()() -> IP_range;

    void set_text_length(std::size_t x);

   private:
    std::size_t end_;
};

class Random : public Animator_base {
   public:
    [[nodiscard]] auto operator()() -> IP_range;

    void set_text_length(std::size_t x);

   private:
    std::size_t end_;
};

class Scroll_base : public Animator_base {
   public:
    [[nodiscard]] auto operator()() -> IP_range;

    void set_text_length(std::size_t x);

    void set_max_length(std::size_t x);

   protected:
    std::size_t begin_ = 0;
    std::size_t hold_  = 0;

   private:
    void reset_hold_length();

   private:
    std::size_t hold_length_;
};

class Scroll : public Scroll_base {
   public:
    void set_text_length(std::size_t x);
};

class Conditional_scroll : public Scroll_base {
   public:
    void set_text_length(std::size_t x);

    void set_max_length(std::size_t x);

   private:
    [[nodiscard]] auto start_condition() const -> bool;

    [[nodiscard]] auto stop_condition() const -> bool;

    void stop_and_reset();
};

class Unscramble : public Animator_base {
   public:
    [[nodiscard]] auto operator()() -> IP_range;

    void set_text_length(std::size_t x);

   private:
    int sorted_to_;

   private:
    void initialize_data();
};

}  // namespace ox::animator

namespace ox {

using Scan_banner = Banner<animator::Scan>;

/// Helper function to create a Scan_banner instance.
[[nodiscard]] auto scan_banner(
    Glyph_string text                = U"",
    Scan_banner::Interval_t interval = std::chrono::milliseconds{50},
    animator::Scan animator = animator::Scan{}) -> std::unique_ptr<Scan_banner>;

/// Helper function to create a Scan_banner instance.
[[nodiscard]] auto scan_banner(Scan_banner::Parameters p)
    -> std::unique_ptr<Scan_banner>;

using Persistent_scan_banner = Banner<animator::Persistent_scan>;

/// Helper function to create a Persistent_scan_banner instance.
[[nodiscard]] auto persistent_scan_banner(
    Glyph_string text                           = U"",
    Persistent_scan_banner::Interval_t interval = std::chrono::milliseconds{50},
    animator::Persistent_scan animator          = animator::Persistent_scan{})
    -> std::unique_ptr<Persistent_scan_banner>;

/// Helper function to create a Persistent_scan_banner instance.
[[nodiscard]] auto persistent_scan_banner(Persistent_scan_banner::Parameters p)
    -> std::unique_ptr<Persistent_scan_banner>;

using Random_banner = Banner<animator::Random>;

/// Helper function to create a Random_banner instance.
[[nodiscard]] auto random_banner(
    Glyph_string text                  = U"",
    Random_banner::Interval_t interval = std::chrono::milliseconds{50},
    animator::Random animator          = animator::Random{})
    -> std::unique_ptr<Random_banner>;

/// Helper function to create a Random_banner instance.
[[nodiscard]] auto random_banner(Random_banner::Parameters p)
    -> std::unique_ptr<Random_banner>;

using Scroll_banner = Banner<animator::Scroll>;

/// Helper function to create a Scroll_banner instance.
[[nodiscard]] auto scroll_banner(
    Glyph_string text                  = U"",
    Scroll_banner::Interval_t interval = std::chrono::milliseconds{50},
    animator::Scroll animator          = animator::Scroll{})
    -> std::unique_ptr<Scroll_banner>;

/// Helper function to create a Scroll_banner instance.
[[nodiscard]] auto scroll_banner(Scroll_banner::Parameters p)
    -> std::unique_ptr<Scroll_banner>;

using Conditional_scroll_banner = Banner<animator::Conditional_scroll>;

/// Helper function to create a Conditional_scroll_banner instance.
[[nodiscard]] auto conditional_scroll_banner(
    Glyph_string text = U"",
    Conditional_scroll_banner::Interval_t interval =
        std::chrono::milliseconds{50},
    animator::Conditional_scroll animator = animator::Conditional_scroll{})
    -> std::unique_ptr<Conditional_scroll_banner>;

/// Helper function to create a Conditional_scroll_banner instance.
[[nodiscard]] auto conditional_scroll_banner(
    Conditional_scroll_banner::Parameters p)
    -> std::unique_ptr<Conditional_scroll_banner>;

using Unscramble_banner = Banner<animator::Unscramble>;

/// Helper function to create a Unscramble_banner instance.
[[nodiscard]] auto unscramble_banner(
    Glyph_string text                      = U"",
    Unscramble_banner::Interval_t interval = std::chrono::milliseconds{50},
    animator::Unscramble animator          = animator::Unscramble{})
    -> std::unique_ptr<Unscramble_banner>;

/// Helper function to create a Unscramble_banner instance.
[[nodiscard]] auto unscramble_banner(Unscramble_banner::Parameters p)
    -> std::unique_ptr<Unscramble_banner>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_BANNER_HPP
