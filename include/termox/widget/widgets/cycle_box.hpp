#ifndef TERMOX_WIDGET_WIDGETS_CYCLE_BOX_HPP
#define TERMOX_WIDGET_WIDGETS_CYCLE_BOX_HPP
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/tile.hpp>

// TODO Indicate in focus with color or trait change.
namespace ox {

/// A rotating set of labels, emitting a Signal when the label is changed.
/** Labels are cycled forwards by a left click or scroll up, cycled backwards
 *  by a scroll down button press, or arrow keys. */
class Cycle_box : public HLabel {
   public:
    /// Emitted when the option is changed, sends the new option's string rep.
    sl::Signal<void(std::string)> option_changed;

   public:
    Cycle_box() : HLabel{L""}
    {
        *this | pipe::align_center() | pipe::strong_focus();
    }

    /// Add an option/label to the Cycle_box.
    /** The returned Signal reference will be emitted every time this option is
     *  enabled. \p label's string representation is the identifying param. */
    auto add_option(Glyph_string label) -> sl::Signal<void()>&
    {
        options_.emplace_back(std::move(label));
        if (this->size() == 1)
            this->set_current_option(0);
        return options_.back().enabled;
    }

    /// Remove an option/label identified by its text.
    /** No-op if does no match found. Only removes the first option found if
     *  there are multiple options with identical text. */
    void remove_option(Glyph_string const& label)
    {
        auto const& label_str = label.str();
        if (label_str == this->current_option_label().str())
            this->set_current_option(0);
        auto const iter = this->find(label_str);
        if (iter != std::end(options_))
            options_.erase(iter);
    }

    /// Move forward one option.
    void next();

    /// Move backward one option.
    void previous();

    /// Set the current option to the option with \p label identifier.
    /** Does not emit the option's signal. No-op if label is not an option. */
    void set_current_option(std::string const& label)
    {
        auto const iter = this->find(label);
        if (iter != std::end(options_))
            this->set_current_option(std::distance(std::begin(options_), iter));
    }

    /// Set the current option to the one at \p index.
    /** Does not emit the option's signal.
     * Input Constraints: \p index < options_.size() */
    void set_current_option(std::size_t index)
    {
        index_ = index;
        *this | pipe::text(options_[index_].name);
    }

    /// Return the number of options in the Cycle_box.
    auto size() const -> std::size_t { return options_.size(); }

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override
    {
        switch (m.button) {
            case Mouse::Button::Left: this->next(); break;
            case Mouse::Button::Right: this->previous(); break;
            default: break;
        }
        return HLabel::mouse_press_event(m);
    }

    auto mouse_wheel_event(Mouse const& m) -> bool override
    {
        switch (m.button) {
            case Mouse::Button::ScrollDown: this->next(); break;
            case Mouse::Button::ScrollUp: this->previous(); break;
            default: break;
        }
        return HLabel::mouse_wheel_event(m);
    }

    auto key_press_event(Key k) -> bool override
    {
        switch (k) {
            case Key::Arrow_up:
            case Key::k: this->previous(); break;
            case Key::Arrow_down:
            case Key::j: this->next(); break;
            default: break;
        }
        return HLabel::key_press_event(k);
    }

   private:
    /// Holds an Option's Signal and name.
    struct Option {
        explicit Option(Glyph_string name_) : name{std::move(name_)} {}
        Glyph_string name;
        sl::Signal<void()> enabled;
    };
    std::vector<Option> options_;
    std::size_t index_ = 0uL;

   private:
    /// Return the current option's text.
    auto current_option_label() const -> Glyph_string
    {
        if (options_.empty())
            return "";
        return options_[index_].name;
    }

    /// Find iter into options_ that points to label, else end iter.
    auto find(std::string const& label) -> decltype(options_)::iterator
    {
        auto const begin = std::begin(options_);
        auto const end   = std::end(options_);
        return std::find_if(begin, end, [&label](Option const& opt) {
            return opt.name == label;
        });
    }

    /// Emit both the option_changed signal and the current option's signal.
    void emit_signals()
    {
        this->option_changed(this->current_option_label().str());
        options_[index_].enabled();  // This is a signal.
    }
};

/// Helper function to create an instance.
inline auto cycle_box() -> std::unique_ptr<Cycle_box>
{
    return std::make_unique<Cycle_box>();
}

/// A label on the left and a Cycle_box on the right.
class Labeled_cycle_box : public layout::Horizontal<> {
   public:
    HLabel& label = make_child<HLabel>(L"");

    Tile& div            = make_child<Tile>(L'├');
    Cycle_box& cycle_box = make_child<Cycle_box>();

   public:
    explicit Labeled_cycle_box(Glyph_string title = "")
    {
        *this | pipe::fixed_height(1) | pipe::label(std::move(title));
    }

    void set_label(Glyph_string title)
    {
        label.set_text(std::move(title));
        this->resize_label();
    }

    void set_divider(Glyph divider) { div.set(divider); }

   private:
    // TODO remove and set label as dynamic
    void resize_label()
    {
        label | pipe::fixed_width(label.get_text().size() + 1);
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto labeled_cycle_box(Args&&... args) -> std::unique_ptr<Labeled_cycle_box>
{
    return std::make_unique<Labeled_cycle_box>(std::forward<Args>(args)...);
}

}  // namespace ox

namespace ox::slot {

inline auto add_option(Cycle_box& cb) -> sl::Slot<void(Glyph_string)>
{
    return link_lifetimes(
        [&cb](Glyph_string label) { cb.add_option(std::move(label)); }, cb);
}

inline auto add_option(Cycle_box& cb, Glyph_string const& label)
    -> sl::Slot<void()>
{
    return link_lifetimes([&cb, label] { cb.add_option(label); }, cb);
}

inline auto remove_option(Cycle_box& cb) -> sl::Slot<void(std::string const&)>
{
    return link_lifetimes(
        [&cb](std::string const& label) { cb.remove_option(label); }, cb);
}

inline auto remove_option(Cycle_box& cb, std::string const& label)
    -> sl::Slot<void()>
{
    return link_lifetimes([&cb, label] { cb.remove_option(label); }, cb);
}

inline auto next(Cycle_box& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb]() { cb.next(); }, cb);
}

inline auto previous(Cycle_box& cb) -> sl::Slot<void()>
{
    return link_lifetimes([&cb]() { cb.previous(); }, cb);
}

}  // namespace ox::slot
#endif  // TERMOX_WIDGET_WIDGETS_CYCLE_BOX_HPP
