# Spinner Widget

[`<termox/widget/widgets/spinner.hpp>`](../../../include/termox/widget/widgets/spinner.hpp)

Single cell animated Spinner Widget. Given a `Glyph_string`, will animate by
taking each `Glyph` as a single frame.

```cpp
class Spinner : public Widget {
   public:
    using Duration_t = std::chrono::milliseconds;

    struct Parameters {
        Glyph_string frames;
        Duration_t period = Duration_t{100};
        int width         = 1;
        int offset        = 0;
    };

   public:
    // Each glyph in frames is a frame, offset is starting index into frames.
    Spinner(Glyph_string frames, Duration_t period = Duration_t{100}, int width = 1, int offset = 0);

    Spinner(Parameters);

    void set_frames(Glyph_string frames);

    auto frames() const -> Glyph_string const&;

    void set_period(Duration_t period);

    auto period() const -> Duration_t;

    void set_width(int width);

    auto spinner_width() const -> int;

    void set_offset(int offset);

    void start();

    void stop();
};

// Specific Spinners -----------------------------------------------------------

struct Spinner_cycle;
struct Spinner_cycle_ccw;
struct Spinner_fall;
struct Spinner_fall_two;
struct Spinner_fall_three;
struct Spinner_rise;
struct Spinner_rise_two;
struct Spinner_rise_three;
struct Spinner_fill;
struct Spinner_top_fill;
struct Spinner_tail;
struct Spinner_switch;
struct Spinner_chase;
struct Spinner_line;
struct Spinner_block_cycle;
struct Spinner_fade;
struct Spinner_fade_trail;
struct Spinner_quarter_circles;
struct Spinner_triangles;
struct Spinner_empty_triangles;
struct Spinner_clock;
struct Spinner_box;
struct Spinner_cross;
struct Spinner_vertical_pass;
struct Spinner_horizontal_pass;
struct Spinner_bump;
```
