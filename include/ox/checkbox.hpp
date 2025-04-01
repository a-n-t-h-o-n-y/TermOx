#pragma once

#include <vector>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

class CheckBox : public Widget {
   public:
    enum class State : bool { Checked, UnChecked };

    struct Display {
        std::vector<Glyph> checked;
        std::vector<Glyph> unchecked;
        Brush in_focus_brush;
    };

    struct Options {
        State state = State::UnChecked;
        Display display = {
            .checked = U"[X]"_gs,
            .unchecked = U"[ ]"_gs,
            .in_focus_brush = {.traits = Trait::Bold},
        };
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
    void key_press(Key k) override;

    void mouse_press(Mouse m) override;

    void paint(Canvas c) override;

    void focus_in() override;

    void focus_out() override;

   private:
    State state_;
    bool in_focus_ = false;
};

}  // namespace ox