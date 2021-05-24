#ifndef TERMOX_DEMOS_SNAKE_SNAKE_HPP
#define TERMOX_DEMOS_SNAKE_SNAKE_HPP
#include <array>
#include <chrono>
#include <cstddef>
#include <set>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/dynamic_colors.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/painter/palette/stormy6.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/layouts/float.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/number_view.hpp>
#include <termox/widget/widgets/text_view.hpp>
#include <termox/widget/widgets/toggle_button.hpp>

namespace snake {

class Snake;

/// A field of apples, stored as Points to their location within a given Area.
class Apple_field {
   public:
    /// Initialize the field with some % of cells as apples.
    void initialize(ox::Area a, Snake const& mask);

    /// Add a new apple at a random point within the area.
    /** Will not add the apple over an existing apple, or over any point within
     *  the given \p mask. */
    void create_apple(Snake const& mask);

    /// Remove apple at point \p if one exists
    void remove(ox::Point p);

    /// Return true if there is an apple at \p p.
    auto contains(ox::Point p) const -> bool;

    /// Return iterator to the first apple Point in the field.
    auto begin() const -> std::set<ox::Point>::const_iterator;

    /// Return iterator to one past-the-last apple Point in the field.
    auto end() const -> std::set<ox::Point>::const_iterator;

    /// Return the number of apples in the field.
    auto size() const -> std::size_t;

    /// Return the field's bounding Area.
    auto area() const -> ox::Area;

    /// Resize the apple field, remove apples outside of the new boundary.
    void resize(ox::Area a);

    /// Removes all apples, does not resize.
    void clear();

   private:
    std::set<ox::Point> points_;
    ox::Area area_;
};

class Snake {
   public:
    enum class Direction { Left, Right, Up, Down };

   public:
    /// Clear existing snake, create a new snake at \p p with Direction::Right.
    void initialize(ox::Point p);

    /// Allow direction changes.
    void enable_input();

    /// Disallow direction changes.
    void disable_input();

    /// Checks the direction isn't 180 degrees from the current before setting.
    void set_direction(Direction d);

    auto get_direction() const -> Direction;

    /// Adds a Point to the Snake's head in the current direction.
    void increment_head();

    /// Remove the tail Point from the Snake.
    void decrement_tail();

    /// Return the Point where the Snake's head is located.
    auto head() const -> ox::Point;

    /// Returns iterator to the tail of the snake.
    auto begin() const -> std::vector<ox::Point>::const_iterator;

    /// Returns iterator to one past the head of the snake.
    auto end() const -> std::vector<ox::Point>::const_iterator;

    /// Return the size of the snake, in cells.
    auto size() const -> std::size_t;

    /// Return the point at snake segment \p i, where 0 is the tail.
    auto operator[](std::size_t i) const -> ox::Point;

   private:
    // First point is the tail, last point is the head.
    std::vector<ox::Point> points_;
    Direction next_direction_ = Direction::Right;
    Direction direction_      = Direction::Right;
    bool has_input_           = false;
};

class Engine {
   public:
    enum class State { Dead, Apple, No_apple };

   public:
    Apple_field apples;
    Snake snake;

   public:
    sl::Signal<void()> game_over;
    sl::Signal<void(unsigned)> score;

   public:
    /// Set up the state for a new game.
    void reset(ox::Area a);

    /// Increment the game state by one step.
    void increment();

    /// Return true is the game is over.
    auto is_game_over() const -> bool;

    /// Return the current score.
    auto get_score() const -> unsigned;

   private:
    unsigned score_ = 0;
    unsigned step_  = 0;

   private:
    static auto constexpr add_apple_at_ = 20;
};

namespace color {

inline auto constexpr Fade    = ox::Color{16};
inline auto constexpr Rainbow = ox::Color{17};

inline auto constexpr Apple            = Fade;
inline auto constexpr Snake            = Rainbow;
inline auto constexpr Border           = ox::stormy6::Teal;
inline auto constexpr Instruction_fg   = ox::stormy6::White;
inline auto constexpr Instruction_bg   = ox::stormy6::Teal;
inline auto constexpr Start_bg         = ox::stormy6::Green;
inline auto constexpr Pause_bg         = ox::stormy6::Red;
inline auto constexpr Size_bg          = ox::stormy6::White;
inline auto constexpr Size_fg          = ox::stormy6::Black;
inline auto constexpr Score_bg         = ox::stormy6::White;
inline auto constexpr Score_fg         = ox::stormy6::Black;
inline auto constexpr Instruction_text = ox::stormy6::Orange;

}  // namespace color

inline auto const snake_palette = [] {
    auto pal = ox::stormy6::palette;
    pal.push_back({color::Fade, ox::dynamic::fade<ox::dynamic::Sine>(
                                    ox::RGB{0x7f9860}, ox::RGB{0xa95a3f})});
    pal.push_back({color::Rainbow, ox::dynamic::rainbow()});
    return pal;
}();

/// Main Game Widget
class Game_space : public ox::Widget {
   public:
    Game_space();

   public:
    /// Start the game from the current state.
    /** If the game is over, will reset the game state and start a new game.
     */
    void start();

    /// Stop/pause the game.
    void stop();

    /// Start the game if it is stopped, stop the game it started.
    void toggle();

    /// Resize the playing surface, also resets the game state.
    void resize(ox::Area a);

    void reset();

    /// Set the amount of time between game steps.
    void set_speed(std::chrono::milliseconds period);

   protected:
    auto paint_event(ox::Painter& p) -> bool override;

    /// Change the direction of the snake.
    auto key_press_event(ox::Key k) -> bool override;

    /// Increment the game state.
    auto timer_event() -> bool override;

    /// Checks if the screen is too small to display the game state.
    auto resize_event(ox::Area new_size, ox::Area old_size) -> bool override;

   private:
    inline static auto const initial_period_ = std::chrono::milliseconds{210};

    Engine engine_;
    std::chrono::milliseconds period_{initial_period_};
    bool too_small_ = false;

    static auto constexpr apple = U'@' | ox::Trait::Bold | fg(color::Apple);
    static auto constexpr snake_body = U'█' | fg(color::Snake);
    static auto constexpr tail       = std::array<ox::Glyph, 3>{
        U'░' | fg(color::Snake), U'▒' | fg(color::Snake),
        U'▓' | fg(color::Snake)};

   public:
    sl::Signal<void()>& game_over     = engine_.game_over;
    sl::Signal<void(unsigned)>& score = engine_.score;
    sl::Signal<void()> started;
    sl::Signal<void()> stopped;

   private:
    [[nodiscard]] auto is_too_small() -> bool;

    void paint_size_message(ox::Painter& painter);

    void paint_game(ox::Painter& painter);
};

class Instructions : public ox::Text_view {
   public:
    explicit Instructions(Parameters = {});
};

class Button_bar : public ox::HTuple<ox::Toggle_button,
                                     ox::Labeled_cycle_box,
                                     Instructions> {
   private:
    ox::Toggle_button& start_pause_btns_ = this->get<0>();
    ox::Labeled_cycle_box& sizes_        = this->get<1>();
    Instructions& instructions_          = this->get<2>();

   public:
    sl::Signal<void()>& start = start_pause_btns_.top_pressed;
    sl::Signal<void()>& pause = start_pause_btns_.bottom_pressed;
    sl::Signal<void(char)> size_change;

   public:
    Button_bar();

   public:
    void show_start_button();
    void show_pause_button();
};

class Score : public ox::HPair<ox::HLabel, ox::Int_view> {
   public:
    Score();

   public:
    void set(unsigned score);

   private:
    ox::Int_view& score_ = this->second;
};

class Bottom_bar : public ox::HPair<Button_bar, Score> {
   public:
    Button_bar& buttons = this->first;
    Score& score        = this->second;
};

class Snake_game : public ox::layout::Vertical<> {
   private:
    using Floating_game = ox::Float_2d<Game_space>;

   public:
    Snake_game();

   protected:
    auto focus_in_event() -> bool override;

   private:
    Floating_game& floating_game = make_child<Floating_game>();
    Game_space& game_space_      = floating_game.widget.widget;
    Bottom_bar& bottom_          = make_child<Bottom_bar>();
};

}  // namespace snake
#endif  // TERMOX_DEMOS_SNAKE_SNAKE_HPP
