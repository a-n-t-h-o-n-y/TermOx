#include <termox/painter/color.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/opposite.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>

class Orange_widget : public ox::Widget {
   public:
    Orange_widget() { *this | bg(ox::Color::Orange); }
};

class Blue_widget : public ox::Widget {
   public:
    Blue_widget() { *this | bg(ox::Color::Blue); }
};

template <typename Layout_t>
struct Label_label;

template <typename Child_t>
struct Label_label<ox::layout::Vertical<Child_t>>
    : ox::Label<ox::layout::Vertical> {
    Label_label() : Label{U"Vertical Label", ox::Align::Center} {}
};

template <typename Child_t>
struct Label_label<ox::layout::Horizontal<Child_t>>
    : ox::Label<ox::layout::Horizontal> {
    Label_label() : Label{U"Horizontal Label", ox::Align::Center} {}
};

template <typename Layout_t>
class Line : public ox::Widget {
   private:
    static auto constexpr is_vertical = ox::layout::is_vertical_v<Layout_t>;

   public:
    Line()
    {
        if constexpr (is_vertical)
            *this | ox::pipe::fixed_width(1) | ox::pipe::wallpaper(U'│');
        else
            *this | ox::pipe::fixed_height(1) | ox::pipe::wallpaper(U'─');
    }
};

template <template <typename> typename Layout_t>
using Labels = ox::Passive<ox::Array<
    Layout_t<Label_label<ox::layout::Opposite_t<Layout_t<ox::Widget>>>>,
    5>>;

template <template <typename> typename Layout_t>
class Bordered_labels : public Labels<Layout_t> {
   public:
    Bordered_labels() { *this | ox::pipe::bordered(); }
};

template <template <typename> typename Layout_t>
using Passives =
    ox::Passive<ox::Tuple<Layout_t<ox::Widget>,
                          Labels<Layout_t>,
                          Line<ox::layout::Opposite_t<Layout_t<ox::Widget>>>,
                          Labels<Layout_t>,
                          Line<ox::layout::Opposite_t<Layout_t<ox::Widget>>>,
                          Labels<Layout_t>>>;

template <template <typename> typename Layout_t>
using Passives_with_borders =
    ox::Passive<ox::Tuple<Layout_t<ox::Widget>,
                          Bordered_labels<Layout_t>,
                          Line<ox::layout::Opposite_t<Layout_t<ox::Widget>>>,
                          Bordered_labels<Layout_t>,
                          Line<ox::layout::Opposite_t<Layout_t<ox::Widget>>>,
                          Bordered_labels<Layout_t>>>;

template <template <typename> typename Layout_t>
class Bordered_passives : public Passives_with_borders<Layout_t> {
   public:
    Bordered_passives() { *this | ox::pipe::bordered(); }
};

template <template <typename> typename Layout_t>
using Nested = ox::Tuple<Layout_t<ox::Widget>,
                         Orange_widget,
                         Passives<Layout_t>,
                         Orange_widget>;

using HNested = Nested<ox::layout::Horizontal>;
using VNested = Nested<ox::layout::Vertical>;

template <template <typename> typename Layout_t>
using Single_passive = ox::
    Tuple<Layout_t<ox::Widget>, Orange_widget, Labels<Layout_t>, Orange_widget>;

using HSingle_passive = Single_passive<ox::layout::Horizontal>;
using VSingle_passive = Single_passive<ox::layout::Vertical>;

template <template <typename> typename Layout_t>
using Single_bordered = ox::Tuple<Layout_t<ox::Widget>,
                                  Orange_widget,
                                  Bordered_labels<Layout_t>,
                                  Orange_widget>;

using HSingle_bordered = Single_bordered<ox::layout::Horizontal>;
using VSingle_bordered = Single_bordered<ox::layout::Vertical>;

template <template <typename> typename Layout_t>
using Nested_bordered = ox::Tuple<Layout_t<ox::Widget>,
                                  Orange_widget,
                                  Bordered_passives<Layout_t>,
                                  Orange_widget>;

using HNested_bordered = Nested_bordered<ox::layout::Horizontal>;
using VNested_bordered = Nested_bordered<ox::layout::Vertical>;

class Test : public ox::Cycle_stack<> {
   public:
    Test()
    {
        this->make_page<VSingle_passive>(
            U"Vertical Single Passive Layout(5 Elements)");
        this->make_page<HSingle_passive>(
            U"Horizontal Single Passive Layout(5 Elements)");

        this->make_page<VNested>(
            U"Vertical Nested Passive Layouts(3 Groups of 5 Elements, 2 "
            U"Lines)");
        this->make_page<HNested>(
            U"Horizontal Nested Passive Layouts(3 Groups of 5 Elements, 2 "
            U"Lines)");

        this->make_page<HSingle_bordered>(
            U"Bordered Horizontal Single Passive(5 Elements)");
        this->make_page<VSingle_bordered>(
            U"Bordered Vertical Single Passive(5 Elements)");

        this->make_page<VNested_bordered>(
            U"Vertical Nested Bordered Passive Layouts(3 Groups of 5 Elements, "
            U"2 Lines)");
        this->make_page<HNested_bordered>(
            U"Horizontal Nested Bordered Passive Layouts(3 Groups of 5 "
            U"Elements, 2 Lines)");
    }
};

int main() { return ox::System{}.run<Test>(); }
