#include <termox/termox.hpp>

class Linear : public ox::HArray<ox::HFixed<ox::Textbox>, 5> {
    using Base_t = ox::HArray<ox::HFixed<ox::Textbox>, 5>;

   public:
    Linear() : Base_t{2, 4, "Hello, World!"}
    {
        using namespace ox::pipe;
        *this | children() | for_each([](auto& child) {
            child.buffer_1 | bg(ox::Color::Orange);
            child.buffer_2 | bg(ox::Color::Orange);
        });
    }
};

class Two_dimensional : public ox::Fixed_2d<ox::Textbox> {
   public:
    Two_dimensional() : Fixed_2d{1, 2, 3, 4, U"Hello, world!"}
    {
        this->widget | bg(ox::Color::Orange);
    }
};

class Border_like : public ox::Fixed_2d<ox::Textbox> {
   public:
    Border_like() : Fixed_2d{1, 1, 2, 2, U"Hello, world!"}
    {
        this->widget | bg(ox::Color::Orange);
    }
};

class Test : public ox::Cycle_stack<> {
   public:
    Test()
    {
        this->make_page<Linear>(U"Linear");
        this->make_page<Two_dimensional>(U"Two_dimensional");
        this->make_page<ox::VArray<Border_like, 3>>(U"Border_like");
    }
};

int main() { ox::System{}.run<Test>(); }
