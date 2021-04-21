#include <string>

#include <termox/termox.hpp>
#include "termox/widget/size_policy.hpp"

class Numbered_box : public ox::Text_view {
   public:
    void set_value(int i)
    {
        value_ = i;
        this->set_contents(std::to_string(value_));
    }

    auto get_value() const -> int { return value_; }

   private:
    int value_ = 0;
};

struct Boxes : ox::HArray<Numbered_box, 10> {
    Boxes()
    {
        auto i = 0;
        for (auto& child : this->get_children())
            child.set_value(++i);
    }
};

struct Sort_btn : ox::Fixed_height<1, ox::Toggle_button> {
    Sort_btn() : Fixed_height{U"Sort a", U"Sort b"}
    {
        *this | ox::pipe::children() | bg(ox::Color::Blue);
    }
};

struct Test : ox::VPair<Boxes, Sort_btn> {
    Test()
    {
        this->second.top | ox::pipe::on_press([this] {
            this->first.sort([](auto const& a, auto const& b) {
                return a.get_value() < b.get_value();
            });
        });
        this->second.bottom | ox::pipe::on_press([this] {
            this->first.sort([](auto const& a, auto const& b) {
                return a.get_value() > b.get_value();
            });
        });
    }
};

int main() { return ox::System{}.run<Test>(); }
