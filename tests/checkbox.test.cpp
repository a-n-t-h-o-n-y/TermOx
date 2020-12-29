#include <string>

#include <termox/system/system.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/float.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>

// TODO move to float.hpp ?
template <typename Widget_t>
using Float_2d = ox::Float<ox::Float<Widget_t, ox::layout::Horizontal>,
                           ox::layout::Vertical>;

// Wall of Checkboxes - Update a count of checked boxes.
class Wall : public ox::layout::Vertical<> {
   private:
    static auto constexpr vcount = 20;
    static auto constexpr hcount = 40;

   public:
    using Checkboxes =
        ox::Array<ox::layout::Horizontal<
                      ox::Array<ox::layout::Vertical<ox::Checkbox1>, vcount>>,
                  hcount>;

    class Count : public ox::HLabel {
       public:
        Count() : ox::HLabel{L"Count: 0"} {}

       public:
        void increment()
        {
            ++count_;
            this->ox::HLabel::set_text("Count: " + std::to_string(count_));
        }

        void decrement()
        {
            --count_;
            this->ox::HLabel::set_text("Count: " + std::to_string(count_));
        }

       private:
        int count_ = 0;
    };

   public:
    Checkboxes& boxes_ = make_child<Checkboxes>(ox::Checkbox1::Parameters{
        ox::Checkbox<ox::layout::Horizontal>::State::Checked, false});
    Count& count       = make_child<Count>();

   public:
    Wall()
    {
        using namespace ox::pipe;
        count | fixed_height(1);

        boxes_ | children() | fixed_width(1);
        // boxes_ | fixed_width(hcount);
        for (auto& row : boxes_.get_children()) {
            for (auto& child : row.get_children()) {
                child.checked.connect([this] { count.increment(); });
                child.unchecked.connect([this] { count.decrement(); });
                // child | fixed_height(vcount);
            }
        }
    }
};

/// horizontal labeled checkboxes in all initial configurations
// use both h and v checkboxes
struct Horizontals : public ox::layout::Vertical<> {
    Horizontals()
    {
        using namespace ox;
        this->make_child<HCheckbox1_label>({L"Label!"});
        this->make_child<HCheckbox_label>({L"Label!"});
        this->make_child<HLabel_checkbox1>({L"Label!"});
        this->make_child<HLabel_checkbox>({L"Label!"});
        this->make_child<HCheckbox2_label>({L"Label!"});
        this->make_child<HLabel_checkbox2>({L"Label!"});
        this->make_child<HCheckbox3_label>({L"Label!"});
        this->make_child<HLabel_checkbox4>({L"Label!"});
        this->make_child<HCheckbox7_label>({L"Label!"});
        this->make_child<HLabel_checkbox8>({L"Label!"});
        this->make_child<HCheckbox11_label>({L"Label!"});
        this->make_child<HLabel_checkbox12>({L"Label!"});
        this->make_child<HCheckbox15_label>({L"Label!"});
        this->make_child<HLabel_checkbox16>({L"Label!"});
        this->make_child<HCheckbox19_label>({L"Label!"});
        this->make_child<HLabel_checkbox19>({L"Label!"});
        this->make_child<HCheckbox20_label>({L"Label!"});
        this->make_child<HLabel_checkbox20>({L"Label!"});
    }
};

/// vertical labeled checkboxes in all initial configurations
// use both h and v checkboxes
struct Verticals : public ox::layout::Horizontal<> {
    Verticals()
    {
        using namespace ox;
        this->make_child<VCheckbox1_label>({L"Label!"});
        this->make_child<VCheckbox_label>({L"Label!"});
        this->make_child<VLabel_checkbox1>({L"Label!"});
        this->make_child<VLabel_checkbox>({L"Label!"});
        this->make_child<VCheckbox2_label>({L"Label!"});
        this->make_child<VLabel_checkbox2>({L"Label!"});
        this->make_child<VCheckbox5_label>({L"Label!"});
        this->make_child<VLabel_checkbox6>({L"Label!"});
        this->make_child<VCheckbox9_label>({L"Label!"});
        this->make_child<VLabel_checkbox10>({L"Label!"});
        this->make_child<VCheckbox13_label>({L"Label!"});
        this->make_child<VLabel_checkbox14>({L"Label!"});
        this->make_child<VCheckbox17_label>({L"Label!"});
        this->make_child<VLabel_checkbox18>({L"Label!"});
        this->make_child<VCheckbox19_label>({L"Label!"});
        this->make_child<VLabel_checkbox19>({L"Label!"});
        this->make_child<VCheckbox20_label>({L"Label!"});
        this->make_child<VLabel_checkbox20>({L"Label!"});
    }
};

// Parameters box to modify look and lock and display running list of signals
// emitted, you only need a single checkbox for this. no need for a labeled cb
struct Runtime : public ox::layout::Vertical<> {};

class Test : public ox::Cycle_stack<> {
   public:
    Test()
    {
        this->make_page<Float_2d<Wall>>(L"Wall");
        this->make_page<Horizontals>(L"Labeled Horizontals");
        this->make_page<Verticals>(L"Labeled Verticals");
        this->make_page<Runtime>(L"Runtime");
    }
};

int main() { return ox::System{}.run<Test>(); }
