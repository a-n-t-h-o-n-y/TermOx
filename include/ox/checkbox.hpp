#pragma once

#include <vector>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

/**
 *  Displays a Checkbox with checked and unchecked states emitted. Doesn't have a Label.
 */
class CheckBox : public Widget {
   public:
    using Widget::handle;

    enum class State : bool { Checked, UnChecked };

    struct Display {
        std::vector<Glyph> checked;
        std::vector<Glyph> unchecked;
        Style in_focus_style;
    };

    struct Options {
        State state = State::UnChecked;
        Display display = {
            .checked = U"[X]"_gs,
            .unchecked = U"[ ]"_gs,
            .in_focus_style = {.traits = Trait::Bold},
        };
        FocusPolicy focus_policy = FocusPolicy::Strong;
    } static const init;  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88165

   public:
    Display display;

    sl::Signal<void()> on_check;
    sl::Signal<void()> on_uncheck;

    CheckBox(Options x = init);

   public:
    void toggle();

    void check();

    void uncheck();

    void set_state(State s);

    [[nodiscard]] auto get_state() const -> State;

   public:
    auto handle(KeyEvent const& k) -> EventResponse override;

    auto handle(MousePress const& m) -> EventResponse override;

    auto paint(Canvas c) -> Cursor override;

    auto focus_in() -> EventResponse override;

    auto focus_out() -> EventResponse override;

   private:
    State state_;
    bool in_focus_ = false;
};

}  // namespace ox