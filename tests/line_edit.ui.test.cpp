#include <termox/termox.hpp>

class Line_edit_test
    : public ox::VTuple<ox::Widget, ox::Line_edit, ox::Line_edit, ox::Widget> {
   public:
    Line_edit_test()
        : ox::VTuple<ox::Widget, ox::Line_edit, ox::Line_edit, ox::Widget>{
              {},
              {U"Left", ox::Align::Left, ox::Line_edit::Action::Clear},
              {U"Right", ox::Align::Right, ox::Line_edit::Action::Clear},
              {}}
    {}
};

int main() { return ox::System{}.run<Line_edit_test>(); }
