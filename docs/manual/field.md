# Field Widget Example

Randomly generates a `Glyph` for each cell; has interactive mouse actions on
those `Glyph`s.

```cpp
#include <array>
#include <random>

#include <termox/termox.hpp>

/// Random Field of pre-defined Glyphs, expands on larger resize event.
/** Inverts Glyph under mouse on left click and drag, generates new glyph under
 *  mouse on right click and drag. */
class Field : public ox::Matrix_display {
   public:
    Field() : Matrix_display{ox::Area{0, 0}} {}

   protected:
    auto mouse_press_event(ox::Mouse const& m) -> bool override
    {
        this->handle_mouse(m);
        return Matrix_display::mouse_press_event(m);
    }

    auto mouse_move_event(ox::Mouse const& m) -> bool override
    {
        this->handle_mouse(m);
        return Matrix_display::mouse_move_event(m);
    }

    auto resize_event(ox::Area new_size, ox::Area old) -> bool override
    {
        if (new_size.width > this->matrix.width() ||
            new_size.height > this->matrix.height()) {
            this->matrix.resize(new_size);
            for (auto x = old.width; x < this->matrix.width(); ++x) {
                for (auto y = 0; y < this->matrix.height(); ++y)
                    this->matrix({x, y}) = generate_random_glyph();
            }
            for (auto x = 0; x < this->matrix.width(); ++x) {
                for (auto y = old.height; y < this->matrix.height(); ++y)
                    this->matrix({x, y}) = generate_random_glyph();
            }
        }
        return Matrix_display::resize_event(new_size, old);
    }

   private:
    void handle_mouse(ox::Mouse const& m)
    {
        auto& g = this->matrix(m.at);
        switch (m.button) {
            case ox::Mouse::Button::Left: g = invert(g); break;
            case ox::Mouse::Button::Right: g = generate_random_glyph(); break;
            default: break;
        }
        this->update();
    }

   private:
    [[nodiscard]] static auto invert(ox::Glyph g) -> ox::Glyph
    {
        if (g.brush.traits.contains(ox::Trait::Inverse))
            g.brush.traits.remove(ox::Trait::Inverse);
        else
            g.brush.traits.insert(ox::Trait::Inverse);
        return g;
    }

    [[nodiscard]] static auto generate_random_glyph() -> ox::Glyph
    {
        static auto constexpr glyphs = std::array{
            U'🬀', U'🬁', U'🬂', U'🬃', U'🬄', U'🬅', U'🬆',
            U'🬇', U'🬈', U'🬉', U'🬊', U'🬋', U'🬌', U'🬍',
            U'🬎', U'🬏', U'🬐', U'🬑', U'🬒', U'🬓', U'🬔',
            U'🬕', U'🬖', U'🬗', U'🬘', U'🬙', U'🬚', U'🬛',
            U'🬜', U'🬝', U'🬞', U'🬟', U'🬠', U'🬡', U'🬢',
            U'🬣', U'🬤', U'🬥', U'🬦', U'🬧', U'🬨', U'🬩',
            U'🬪', U'🬫', U'🬬', U'🬭', U'🬮', U'🬯', U'🬰',
            U'🬱', U'🬲', U'🬳', U'🬴', U'🬵', U'🬶', U'🬷',
            U'🬸', U'🬹', U'🬺', U'🬻', U'█'};
        static auto rng = std::mt19937{std::random_device{}()};
        static auto dist =
            std::uniform_int_distribution<std::size_t>{0, glyphs.size() - 1};
        return glyphs[dist(rng)];
    }
};
```
