#include <utility>

#include <termox/termox.hpp>

template <typename... Spinners>
class Spinner_hline
    : public ox::Tuple<ox::layout::Horizontal<>, Spinners..., ox::Widget> {
   public:
    Spinner_hline(ox::Spinner::Duration_t period = ox::Spinner::Duration_t{100},
                  int width                      = 1,
                  int offset                     = 0)
    {
        *this | ox::pipe::fixed_height(1);
        this->initialize(period, width, offset,
                         std::index_sequence_for<Spinners...>{});
    }

   private:
    template <std::size_t... Is>
    void initialize(ox::Spinner::Duration_t period,
                    int width,
                    int offset,
                    std::index_sequence<Is...>)
    {
        (this->template get<Is>().start(), ...);
        (this->template get<Is>().set_period(period), ...);
        (this->template get<Is>().set_width(width), ...);
        (this->template get<Is>().set_offset(offset), ...);
    }
};

int main()
{
    using namespace ox;
    using All_spinners = Spinner_hline<
        Spinner_cycle, Spinner_cycle_ccw, Spinner_fall, Spinner_fall_two,
        Spinner_fall_three, Spinner_rise, Spinner_rise_two, Spinner_rise_three,
        Spinner_fill, Spinner_top_fill, Spinner_tail, Spinner_switch,
        Spinner_chase, Spinner_line, Spinner_block_cycle, Spinner_fade,
        Spinner_fade_trail, Spinner_quarter_circles, Spinner_triangles,
        Spinner_empty_triangles, Spinner_clock, Spinner_box, Spinner_cross,
        Spinner_vertical_pass, Spinner_horizontal_pass, Spinner_bump>;

    using Interval = Spinner::Duration_t;

    auto app = vertical(make<All_spinners>(Interval{160}, 1, 0),
                        make<All_spinners>(Interval{160}, 1, 1),
                        make<All_spinners>(Interval{160}, 1, 2),
                        make<All_spinners>(Interval{160}, 1, 3),
                        make<All_spinners>(Interval{80}, 2, 0),
                        make<All_spinners>(Interval{80}, 2, 1),
                        make<All_spinners>(Interval{80}, 2, 2),
                        make<All_spinners>(Interval{80}, 2, 3),
                        make<All_spinners>(Interval{40}, 3, 0),
                        make<All_spinners>(Interval{40}, 3, 1),
                        make<All_spinners>(Interval{40}, 3, 2),
                        make<All_spinners>(Interval{40}, 3, 3), widget());

    return ox::System{}.run(*app);
}
