#include <termox/painter/color.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>

// tuple of 5 checkboxes, with a passive<horizontal> layout
class Buffer : public ox::Widget {
   public:
    Buffer() { *this | bg(ox::Color::Blue); }
};

template <typename Check>
using Boxes = ox::Array<ox::layout::Passive<ox::layout::Horizontal<Check>>, 15>;
using Boxes1 = Boxes<ox::Checkbox1>;
using Boxes2 = Boxes<ox::Checkbox2>;

// buffer | tuple | buffer
class Test_1 : public ox::layout::Horizontal<> {
   public:
    Buffer& buf_1   = this->make_child<Buffer>();
    Boxes1& boxes_1 = this->make_child<Boxes1>();
    Buffer& buf_2   = this->make_child<Buffer>();
    Boxes2& boxes_2 = this->make_child<Boxes2>();
    Buffer& buf_3   = this->make_child<Buffer>();
};

// Solution: if you are trying to make it passive two layers deep, you need to
// be explicit about that, you need a separate passive type that handles that by
// setting both. Unless you want that to be a single type that always does that,
// but you'll have to work out if that is expected or not.

//          buffer
// buffer | Box2D | buffer   :   Inner
//          buffer
using Line =
    ox::Array<ox::layout::Passive<ox::layout::Horizontal<ox::Checkbox1>, 0>,
              15>;
// class Line : public ox::Array<
//                  ox::layout::Passive<ox::layout::Horizontal<ox::Checkbox1>>,
//                  15> {
//    public:
//     Line() { *this | ox::pipe::fixed_height(1); }
// };

using Box2D = ox::Array<ox::layout::Passive<ox::layout::Vertical<Line>, 0>, 5>;

// using Inner = ox::Tuple<ox::layout::Horizontal<>, Buffer, Box2D, Buffer>;

// using Test_2 = ox::Tuple<ox::layout::Vertical<>, Buffer, Inner, Buffer>;

// you don't want this to be passive horizontally, just vertically... but its a
// horizontal layout. maybe you don't need the index, you want horizontal to be
// normal layout, while the vertical non-dominant dimension to be passive to the
// middle widget... NDPassive<Layout_t, index>
// you probably need Float2D to be a special case, its own class.. but it is
// implementing a lot of Passive, if it is to follow a wrapped size policy.
// Instead of doing the sum of the dominant policy, it just grabs the
// non-dominant policy of the second child and assigns it to itself. But yeah,
// it has to duplicate part of passive, not sure if passive needs that now.. but
// it is nice for the array of boxes though. And that is part of passive.
template <typename T>
using Inner = ox::
    Tuple<ox::layout::Passive<ox::layout::Horizontal<>, 1>, Buffer, T, Buffer>;

template <typename T>
using Float2D = ox::VTuple<Buffer, Inner<T>, Buffer>;

class Test : public ox::Cycle_stack<> {
   public:
    Test()
    {
        this->make_page<Float2D<Box2D>>("Two Layers Deep");
        this->make_page<Test_1>("One Layer Deep, 2-15 Boxes");
    }
};

int main() { return ox::System{}.run<Test>(); }
