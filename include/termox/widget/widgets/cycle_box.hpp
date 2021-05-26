#ifndef TERMOX_WIDGET_WIDGETS_CYCLE_BOX_HPP
#define TERMOX_WIDGET_WIDGETS_CYCLE_BOX_HPP
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/tuple.hpp>
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
    Cycle_box();

    /// Add an option/label to the Cycle_box.
    /** The returned Signal reference will be emitted every time this option is
     *  enabled. \p label's string representation is the identifying param. */
    auto add_option(Glyph_string label) -> sl::Signal<void()>&;

    /// Remove an option/label identified by its text.
    /** No-op if does no match found. Only removes the first option found if
     *  there are multiple options with identical text. */
    void remove_option(Glyph_string const& label);

    /// Move forward one option.
    void next();

    /// Move backward one option.
    void previous();

    /// Set the current option to the option with \p label identifier.
    /** Does not emit the option's signal. No-op if label is not an option. */
    void set_current_option(std::string const& label);

    /// Set the current option to the one at \p index.
    /** Does not emit the option's signal.
     * Input Constraints: \p index < options_.size() */
    void set_current_option(std::size_t index);

    /// Return the number of options in the Cycle_box.
    [[nodiscard]] auto size() const -> std::size_t;

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override;

    auto mouse_wheel_event(Mouse const& m) -> bool override;

    auto key_press_event(Key k) -> bool override;

   private:
    /// Holds an Option's Signal and name.
    struct Option {
        Glyph_string name;
        sl::Signal<void()> enabled = {};
    };

   private:
    std::vector<Option> options_;
    std::size_t index_ = 0;

   private:
    /// Return the current option's text.
    [[nodiscard]] auto current_option_label() const -> Glyph_string;

    /// Find iter into options_ that points to label, else end iter.
    [[nodiscard]] auto find(std::string const& label)
        -> decltype(options_)::iterator;

    /// Emit both the option_changed signal and the current option's signal.
    void emit_signals();
};

/// Helper function to create an instance.
[[nodiscard]] auto cycle_box(Cycle_box::Parameters = {})
    -> std::unique_ptr<Cycle_box>;

/// A label on the left and a Cycle_box on the right.
class Labeled_cycle_box : public HTuple<HLabel, Tile, Cycle_box> {
   public:
    struct Parameters {
        Glyph_string label = U"";
    };

   public:
    HLabel& label        = this->get<0>();
    Tile& div            = this->get<1>();
    Cycle_box& cycle_box = this->get<2>();

   public:
    explicit Labeled_cycle_box(Glyph_string title = U"");

    explicit Labeled_cycle_box(Parameters p);
};

/// Helper function to create a Labeled_cycle_box instance.
[[nodiscard]] auto labeled_cycle_box(Glyph_string label = U"")
    -> std::unique_ptr<Labeled_cycle_box>;

/// Helper function to create a Labeled_cycle_box instance.
[[nodiscard]] auto labeled_cycle_box(Labeled_cycle_box::Parameters p)
    -> std::unique_ptr<Labeled_cycle_box>;

}  // namespace ox

namespace ox::slot {

[[nodiscard]] auto add_option(Cycle_box& cb) -> sl::Slot<void(Glyph_string)>;

[[nodiscard]] auto add_option(Cycle_box& cb, Glyph_string const& label)
    -> sl::Slot<void()>;

[[nodiscard]] auto remove_option(Cycle_box& cb)
    -> sl::Slot<void(std::string const&)>;

[[nodiscard]] auto remove_option(Cycle_box& cb, std::string const& label)
    -> sl::Slot<void()>;

[[nodiscard]] auto next(Cycle_box& cb) -> sl::Slot<void()>;

[[nodiscard]] auto previous(Cycle_box& cb) -> sl::Slot<void()>;

}  // namespace ox::slot
#endif  // TERMOX_WIDGET_WIDGETS_CYCLE_BOX_HPP
