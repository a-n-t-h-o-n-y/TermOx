#include <ox/widget.hpp>

#include <vector>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>

namespace ox {

class CheckBox : public Widget {
   public:
    enum class State : bool { Checked, UnChecked };

    struct Display {
        std::vector<Glyph> checked;
        std::vector<Glyph> unchecked;
    } display;

   public:
    sl::Signal<void()> on_check;
    sl::Signal<void()> on_uncheck;

   public:
    CheckBox(State init = State::UnChecked,
             Display display_ = {.checked = U"[X]"_gs, .unchecked = U"[ ]"_gs});

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

   private:
    State state_;
};

}  // namespace ox