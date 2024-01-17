#include <termox/widget/widgets/banner.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/event.hpp>

namespace {

/// Randomly shuffle container contents.
template <typename Container>
void shuffle(Container& container)
{
    static auto gen_ = std::mt19937{std::random_device{}()};
    std::shuffle(std::begin(container), std::end(container), gen_);
}

template <typename Pair_t, typename Container1, typename Container2>
[[nodiscard]] auto zip(Container1 const& x, Container2 const& y)
    -> std::vector<Pair_t>
{
    auto zipped = std::vector<Pair_t>{};
    for (auto i = 0uL; i < x.size(); ++i)
        zipped.push_back({x[i], y[i]});
    return zipped;
}

}  // namespace

namespace ox {

auto operator++(Index_and_position& x) -> Index_and_position&
{
    ++x.index;
    ++x.position;
    return x;
}

IP_range::IP_range(Iter_t begin, Iter_t end) : begin_{begin}, end_{end} {}

auto IP_range::begin() const -> Iter_t { return begin_; }

auto IP_range::end() const -> Iter_t { return end_; }

}  // namespace ox

namespace ox::animator {

Animator_base::Animator_base()
{
    start.connect([this] { started_ = true; });
    stop.connect([this] { started_ = false; });
}

void Animator_base::set_max_length(std::size_t x) { max_length_ = x; }

void Animator_base::set_text_length(std::size_t x) { text_length_ = x; }

auto Animator_base::data() const -> std::vector<Index_and_position> const&
{
    return data_;
}

auto Animator_base::data() -> std::vector<Index_and_position>& { return data_; }

void Animator_base::initialize_data()
{
    this->data().resize(this->text_length());
    std::iota(this->begin(), this->end(), Index_and_position{0, 0});
}

auto Animator_base::max_length() const -> std::size_t { return max_length_; }

auto Animator_base::text_length() const -> std::size_t { return text_length_; }

auto Animator_base::is_started() const -> bool { return started_; }

auto Animator_base::begin() -> std::vector<Index_and_position>::iterator
{
    return std::begin(data_);
}

auto Animator_base::begin() const
    -> std::vector<Index_and_position>::const_iterator
{
    return std::cbegin(data_);
}

auto Animator_base::end() -> std::vector<Index_and_position>::iterator
{
    return std::end(data_);
}

auto Animator_base::end() const
    -> std::vector<Index_and_position>::const_iterator
{
    return std::cend(data_);
}

auto Scan::operator()() -> IP_range
{
    auto const t_length = this->text_length();
    auto const hold_max = t_length * 3;
    if (begin_ == 0 && end_ != t_length && hold_ == 0)
        ++end_;
    else if (begin_ == 0 && end_ == t_length && hold_ != hold_max)
        ++hold_;
    else if (begin_ != t_length && end_ == t_length && hold_ == hold_max)
        ++begin_;
    else
        stop();

    return {this->begin() + begin_, this->begin() + end_};
}

void Scan::set_text_length(std::size_t x)
{
    Animator_base::set_text_length(x);
    if (this->text_length() == 0)
        return;
    this->Animator_base::initialize_data();
    begin_ = 0;
    end_   = 0;
    hold_  = 0;
    start();
}

auto Persistent_scan::operator()() -> IP_range
{
    if (end_ == this->text_length()) {
        stop();
        return {this->begin(), this->end()};
    }
    return {this->begin(), this->begin() + end_++};
}

void Persistent_scan::set_text_length(std::size_t x)
{
    Animator_base::set_text_length(x);
    if (this->text_length() == 0)
        return;
    this->Animator_base::initialize_data();
    end_ = 0;
    start();
}

auto Random::operator()() -> IP_range
{
    if (end_ == this->text_length()) {
        stop();
        return {this->begin(), this->end()};
    }
    return {this->begin(), this->begin() + end_++};
}

void Random::set_text_length(std::size_t x)
{
    Animator_base::set_text_length(x);
    if (this->text_length() == 0)
        return;
    this->Animator_base::initialize_data();
    ::shuffle(this->data());
    end_ = 0;
    start();
}

auto Scroll_base::operator()() -> IP_range
{
    if (hold_ < hold_length_)
        ++hold_;
    else if (begin_ == this->text_length()) {
        this->Animator_base::initialize_data();
        begin_ = 0;
        hold_  = 0;
    }
    else {
        ++begin_;
        std::for_each(this->begin() + begin_, this->end(),
                      [](auto& x) { --x.position; });
    }
    return {this->begin() + begin_, this->end()};
}

void Scroll_base::set_text_length(std::size_t x)
{
    Animator_base::set_text_length(x);
    this->reset_hold_length();
    this->Animator_base::initialize_data();
    begin_ = 0;
    hold_  = 0;
}

void Scroll_base::set_max_length(std::size_t x)
{
    Animator_base::set_max_length(x);
    this->reset_hold_length();
}

void Scroll_base::reset_hold_length()
{
    // Heuristic
    hold_length_ = std::min({this->max_length(), this->text_length(), 20uL});
    hold_length_ = std::max(hold_length_, 20uL);
}

void Scroll::set_text_length(std::size_t x)
{
    Scroll_base::set_text_length(x);
    if (this->text_length() != 0)
        start();
}

void Conditional_scroll::set_text_length(std::size_t x)
{
    Scroll_base::set_text_length(x);
    if (this->start_condition())
        start();
    else if (this->stop_condition())
        this->stop_and_reset();
}

void Conditional_scroll::set_max_length(std::size_t x)
{
    Scroll_base::set_max_length(x);
    if (this->start_condition())
        start();
    else if (this->stop_condition())
        this->stop_and_reset();
}

auto Conditional_scroll::start_condition() const -> bool
{
    return !this->is_started() && this->text_length() > this->max_length() &&
           this->text_length() != 0;
}

auto Conditional_scroll::stop_condition() const -> bool
{
    return this->is_started() && this->text_length() <= this->max_length();
}

void Conditional_scroll::stop_and_reset()
{
    this->Scroll_base::initialize_data();
    hold_  = 0;
    begin_ = 0;
    stop();
}

auto Unscramble::operator()() -> IP_range
{
    if (static_cast<std::size_t>(sorted_to_) == this->text_length()) {
        stop();
        return {this->begin(), this->end()};
    }
    else {
        // TODO Make more concise.
        for (std::size_t i = sorted_to_; i < this->data().size(); ++i) {
            auto& x = this->data()[i];
            if (x.position == sorted_to_) {
                std::swap(x.position, this->data()[sorted_to_].position);
                break;
            }
        }
        ++sorted_to_;
    }
    return {this->begin(), this->end()};
}

void Unscramble::set_text_length(std::size_t x)
{
    Animator_base::set_text_length(x);
    if (this->text_length() == 0)
        return;
    this->initialize_data();
    sorted_to_ = 0;
    start();
}

void Unscramble::initialize_data()
{
    auto indices   = std::vector<std::size_t>(this->text_length());
    auto positions = std::vector<int>(this->text_length());
    std::iota(std::begin(indices), std::end(indices), 0uL);
    std::iota(std::begin(positions), std::end(positions), 0);
    ::shuffle(positions);
    this->data() = ::zip<Index_and_position>(indices, positions);
}

}  // namespace ox::animator

namespace ox {

auto scan_banner(Glyph_string text,
                 Scan_banner::Duration_t interval,
                 animator::Scan animator) -> std::unique_ptr<Scan_banner>
{
    return std::make_unique<Scan_banner>(std::move(text), interval,
                                         std::move(animator));
}

auto scan_banner(Scan_banner::Parameters p) -> std::unique_ptr<Scan_banner>
{
    return std::make_unique<Scan_banner>(std::move(p));
}

auto persistent_scan_banner(Glyph_string text,
                            Persistent_scan_banner::Duration_t interval,
                            animator::Persistent_scan animator)
    -> std::unique_ptr<Persistent_scan_banner>
{
    return std::make_unique<Persistent_scan_banner>(std::move(text), interval,
                                                    std::move(animator));
}

auto persistent_scan_banner(Persistent_scan_banner::Parameters p)
    -> std::unique_ptr<Persistent_scan_banner>
{
    return std::make_unique<Persistent_scan_banner>(std::move(p));
}

auto random_banner(Glyph_string text,
                   Random_banner::Duration_t interval,
                   animator::Random animator) -> std::unique_ptr<Random_banner>
{
    return std::make_unique<Random_banner>(std::move(text), interval,
                                           std::move(animator));
}

auto random_banner(Random_banner::Parameters p)
    -> std::unique_ptr<Random_banner>
{
    return std::make_unique<Random_banner>(std::move(p));
}

auto scroll_banner(Glyph_string text,
                   Scroll_banner::Duration_t interval,
                   animator::Scroll animator) -> std::unique_ptr<Scroll_banner>
{
    return std::make_unique<Scroll_banner>(std::move(text), interval,
                                           std::move(animator));
}

auto scroll_banner(Scroll_banner::Parameters p)
    -> std::unique_ptr<Scroll_banner>
{
    return std::make_unique<Scroll_banner>(std::move(p));
}

auto conditional_scroll_banner(Glyph_string text,
                               Conditional_scroll_banner::Duration_t interval,
                               animator::Conditional_scroll animator)
    -> std::unique_ptr<Conditional_scroll_banner>
{
    return std::make_unique<Conditional_scroll_banner>(
        std::move(text), interval, std::move(animator));
}

auto conditional_scroll_banner(Conditional_scroll_banner::Parameters p)
    -> std::unique_ptr<Conditional_scroll_banner>
{
    return std::make_unique<Conditional_scroll_banner>(std::move(p));
}

auto unscramble_banner(Glyph_string text,
                       Unscramble_banner::Duration_t interval,
                       animator::Unscramble animator)
    -> std::unique_ptr<Unscramble_banner>
{
    return std::make_unique<Unscramble_banner>(std::move(text), interval,
                                               std::move(animator));
}

/// Helper function to create a Unscramble_banner instance.
auto unscramble_banner(Unscramble_banner::Parameters p)
    -> std::unique_ptr<Unscramble_banner>
{
    return std::make_unique<Unscramble_banner>(std::move(p));
}

}  // namespace ox
