#include <memory>
#include <string>

#include <termox/system/system.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/float.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/passive.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/line.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/notify_light.hpp>

class Count : public ox::HLabel {
   public:
    Count() : ox::HLabel{U"Count: 0"} { *this | ox::pipe::fixed_height(1); }

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

auto constexpr vcount = 20;
auto constexpr hcount = 40;

// Wall of Checkboxes - Update a count of checked boxes.
class Wall : public ox::Array<
                 ox::layout::Horizontal<
                     ox::Array<ox::layout::Vertical<ox::Checkbox1>, vcount>>,
                 hcount> {
   public:
    sl::Signal<void()> checked;
    sl::Signal<void()> unchecked;

   public:
    Wall()
    {
        using namespace ox::pipe;
        *this | fixed_height(vcount) | fixed_width(hcount);

        for (auto& row : this->get_children()) {
            for (auto& child : row.get_children()) {
                child.checked.connect([this] { checked(); });
                child.unchecked.connect([this] { unchecked(); });
            }
        }
    }
};

using Float_wall = ox::Float_2d<Wall>;

class Wall_widget : public ox::layout::Vertical<> {
   private:
    Wall& wall_   = this->make_child<Float_wall>().widget.widget;
    Count& count_ = this->make_child<Count>();

   public:
    Wall_widget()
    {
        wall_.checked.connect([this] { count_.increment(); });
        wall_.unchecked.connect([this] { count_.decrement(); });
    }
};

/// horizontal labeled checkboxes in all initial configurations
struct Horizontals : public ox::layout::Vertical<> {
    Horizontals()
    {
        using namespace ox;
        make_child<HCheckbox1_label>({U"Label!"});
        make_child<HCheckbox_label>({U"Label!"});
        make_child<HLabel_checkbox1>({U"Label!"});
        make_child<HLabel_checkbox>({U"Label!"});
        make_child<HCheckbox2_label>({U"Label!"});
        make_child<HLabel_checkbox2>({U"Label!"});
        make_child<HCheckbox3_label>({U"Label!"});
        make_child<HLabel_checkbox4>({U"Label!"});
        make_child<HCheckbox7_label>({U"Label!"});
        make_child<HLabel_checkbox8>({U"Label!"});
        make_child<HCheckbox11_label>({U"Label!"});
        make_child<HLabel_checkbox12>({U"Label!"});
        make_child<HCheckbox15_label>({U"Label!"});
        make_child<HLabel_checkbox16>({U"Label!"});
        make_child<HCheckbox19_label>({U"Label!"});
        make_child<HLabel_checkbox19>({U"Label!"});
        make_child<HCheckbox20_label>({U"Label!"});
        make_child<HLabel_checkbox20>({U"Label!"});
        make_child();
    }
};

/// vertical labeled checkboxes in all initial configurations
// use both h and v checkboxes
struct Verticals : public ox::layout::Horizontal<> {
    Verticals()
    {
        using namespace ox;
        this->make_child<VCheckbox1_label>({U"Label!"});
        this->make_child<VCheckbox_label>({U"Label!"});
        this->make_child<VLabel_checkbox1>({U"Label!"});
        this->make_child<VLabel_checkbox>({U"Label!"});
        this->make_child<VCheckbox2_label>({U"Label!"});
        this->make_child<VLabel_checkbox2>({U"Label!"});
        this->make_child<VCheckbox5_label>({U"Label!"});
        this->make_child<VLabel_checkbox6>({U"Label!"});
        this->make_child<VCheckbox9_label>({U"Label!"});
        this->make_child<VLabel_checkbox10>({U"Label!"});
        this->make_child<VCheckbox13_label>({U"Label!"});
        this->make_child<VLabel_checkbox14>({U"Label!"});
        this->make_child<VCheckbox17_label>({U"Label!"});
        this->make_child<VLabel_checkbox18>({U"Label!"});
        this->make_child<VCheckbox19_label>({U"Label!"});
        this->make_child<VLabel_checkbox19>({U"Label!"});
        this->make_child<VCheckbox20_label>({U"Label!"});
        this->make_child<VLabel_checkbox20>({U"Label!"});
        make_child();
    }
};

// Parameters box to modify look and lock and display running list of signals
// emitted, you only need a single checkbox for this. no need for a labeled cb
class Runtime : public ox::layout::Horizontal<> {
   private:
    class Checkbox_options : public ox::layout::Vertical<> {
       public:
        template <template <typename> typename Layout_t>
        Checkbox_options(ox::Checkbox<Layout_t>& cb)
        {
            using namespace ox::pipe;
            using namespace ox;

            *this | fixed_width(24);

            check_btn | fixed_height(1) | bg(Color::Dark_gray);
            uncheck_btn | fixed_height(1) | bg(Color::Dark_blue);
            toggle_btn | fixed_height(1) | bg(Color::Dark_gray);

            checked_display.wrapped | fixed_width(6) | bg(Color::Dark_gray);
            unchecked_display.wrapped | fixed_width(6) | bg(Color::Dark_gray);

            check_btn | on_press([&cb] { cb.check(); });
            uncheck_btn | on_press([&cb] { cb.uncheck(); });
            toggle_btn | on_press([&cb] { cb.toggle(); });

            lock_cb.checkbox | on_check([&cb] { cb.lock(); });
            lock_cb.checkbox | on_uncheck([&cb] { cb.unlock(); });

            unchecked_display.wrapped.text_modified.connect(
                [&cb](auto const& contents) {
                    cb.set_display({cb.get_display().checked, contents});
                });
            checked_display.wrapped.text_modified.connect(
                [&cb](auto const& contents) {
                    cb.set_display({contents, cb.get_display().unchecked});
                });

            cb | on_check([this] { checked_lb.wrapped.emit(); });
            cb | on_uncheck([this] { unchecked_lb.wrapped.emit(); });
            cb | on_toggle([this] { toggled_lb.wrapped.emit(); });
        }

       private:
        using Checkbox  = ox::HCheckbox20_label;
        using Edit      = ox::HLabel_left<ox::Line_edit>;
        using Light_box = ox::HLabel_right<ox::Notify_light>;

        struct Divider : public ox::HLine {
            Divider() { *this | fg(ox::Color::Light_gray); }
        };

       public:
        ox::Button& check_btn   = this->make_child<ox::Button>(U"Check");
        ox::Button& uncheck_btn = this->make_child<ox::Button>(U"Uncheck");
        ox::Button& toggle_btn  = this->make_child<ox::Button>(U"Toggle");

        Divider& div1     = this->make_child<Divider>();
        Checkbox& lock_cb = this->make_child<Checkbox>({U"Lock"});
        Divider& div2     = this->make_child<Divider>();

        Edit& unchecked_display = this->make_child<Edit>(
            Edit::Parameters{U"Unchecked Display", ox::Align::Left},
            U"☐");
        Edit& checked_display = this->make_child<Edit>(
            Edit::Parameters{U"Checked   Display", ox::Align::Left},
            U"☒");

        Divider& div3 = this->make_child<Divider>();

        Light_box& checked_lb = this->make_child<Light_box>(
            Light_box::Parameters{U"`checked`   Signal"},
            ox::Notify_light::Display{ox::Color::Yellow, ox::Color::Dark_gray},
            ox::Notify_light::Duration_t{350});
        Light_box& unchecked_lb = this->make_child<Light_box>(
            Light_box::Parameters{U"`unchecked` Signal"},
            ox::Notify_light::Display{ox::Color::Yellow, ox::Color::Dark_gray},
            ox::Notify_light::Duration_t{350});
        Light_box& toggled_lb = this->make_child<Light_box>(
            Light_box::Parameters{U"`toggled`   Signal"},
            ox::Notify_light::Display{ox::Color::Yellow, ox::Color::Dark_gray},
            ox::Notify_light::Duration_t{350});

        ox::Widget& buffer = make_child();
    };

   private:
    using Checkbox = ox::Float_2d<ox::Checkbox1>;

   private:
    Checkbox& checkbox = this->make_child<Checkbox>();
    ox::VLine& divider =
        this->make_child<ox::VLine>() | fg(ox::Color::Light_gray);
    Checkbox_options& options =
        this->make_child<Checkbox_options>(checkbox.widget.widget);
};

/// Adds a buffer widget as the second Widget in a Layout_t object.
template <typename Widget_t, template <typename> typename Layout_t>
class Buf : public Layout_t<ox::Widget> {
   private:
    using Base_t = Layout_t<ox::Widget>;

   public:
    template <typename... Args>
    Buf(std::unique_ptr<Widget_t> w_ptr) : Base_t{std::move(w_ptr)}
    {
        if constexpr (ox::layout::is_vertical_v<Layout_t<ox::Widget>>)
            this->width_policy = this->get_children().front().width_policy;
        else
            this->height_policy = this->get_children().front().height_policy;
    }

   public:
    ox::Widget& buffer = this->template make_child();
};

template <typename Widget_t>
using VBuf = Buf<Widget_t, ox::layout::Vertical>;

template <typename Widget_t>
auto vbuf(std::unique_ptr<Widget_t> w_ptr) -> std::unique_ptr<VBuf<Widget_t>>
{
    return std::make_unique<VBuf<Widget_t>>(std::move(w_ptr));
}

class Test : public ox::Cycle_stack<> {
   public:
    Test()
    {
        this->make_page<Wall_widget>(U"Wall");
        this->make_page<Horizontals>(U"Labeled Horizontals");
        this->make_page<Verticals>(U"Labeled Verticals");
        // clang-format off
        this->append_page(U"Checkbox() Helpers",
            ox::layout::vertical(
                ox::layout::horizontal(
                    vbuf(ox::checkbox1()),  vbuf(ox::checkbox2()),
                    vbuf(ox::checkbox3()),  vbuf(ox::checkbox4()),
                    vbuf(ox::checkbox5()),  vbuf(ox::checkbox6()),
                    vbuf(ox::checkbox7()),  vbuf(ox::checkbox8()), 
                    vbuf(ox::checkbox9()),  vbuf(ox::checkbox10()),
                    vbuf(ox::checkbox11()), vbuf(ox::checkbox12()),
                    vbuf(ox::checkbox13()), vbuf(ox::checkbox14()),
                    vbuf(ox::checkbox15()), vbuf(ox::checkbox16()),
                    vbuf(ox::checkbox17()), vbuf(ox::checkbox18()),
                    vbuf(ox::checkbox19()), vbuf(ox::checkbox20()),
                    ox::widget("buffer")
                ),
                ox::layout::horizontal(
                    vbuf(ox::checkbox1({})),  vbuf(ox::checkbox2({})),
                    vbuf(ox::checkbox3({})),  vbuf(ox::checkbox4({})),
                    vbuf(ox::checkbox5({})),  vbuf(ox::checkbox6({})),
                    vbuf(ox::checkbox7({})),  vbuf(ox::checkbox8({})),
                    vbuf(ox::checkbox9({})),  vbuf(ox::checkbox10({})),
                    vbuf(ox::checkbox11({})), vbuf(ox::checkbox12({})),
                    vbuf(ox::checkbox13({})), vbuf(ox::checkbox14({})),
                    vbuf(ox::checkbox15({})), vbuf(ox::checkbox16({})),
                    vbuf(ox::checkbox17({})), vbuf(ox::checkbox18({})),
                    vbuf(ox::checkbox19({})), vbuf(ox::checkbox20({})),
                    ox::widget("buffer")
                ),
                ox::layout::horizontal(
                    vbuf(ox::checkbox1({ox::Checkbox1::State::Checked, false})),
                    vbuf(ox::checkbox2({ox::Checkbox2::State::Checked, false})),
                    vbuf(ox::checkbox3({ox::Checkbox3::State::Checked, false})),
                    vbuf(ox::checkbox4({ox::Checkbox4::State::Checked, false})),
                    vbuf(ox::checkbox5({ox::Checkbox5::State::Checked, false})),
                    vbuf(ox::checkbox6({ox::Checkbox6::State::Checked, false})),
                    vbuf(ox::checkbox7({ox::Checkbox7::State::Checked, false})),
                    vbuf(ox::checkbox8({ox::Checkbox8::State::Checked, false})),
                    vbuf(ox::checkbox9({ox::Checkbox9::State::Checked, false})),
                    vbuf(ox::checkbox10({ox::Checkbox10::State::Checked, false})),
                    vbuf(ox::checkbox11({ox::Checkbox11::State::Checked, false})),
                    vbuf(ox::checkbox12({ox::Checkbox12::State::Checked, false})),
                    vbuf(ox::checkbox13({ox::Checkbox13::State::Checked, false})),
                    vbuf(ox::checkbox14({ox::Checkbox14::State::Checked, false})),
                    vbuf(ox::checkbox15({ox::Checkbox15::State::Checked, false})),
                    vbuf(ox::checkbox16({ox::Checkbox16::State::Checked, false})),
                    vbuf(ox::checkbox17({ox::Checkbox17::State::Checked, false})),
                    vbuf(ox::checkbox18({ox::Checkbox18::State::Checked, false})),
                    vbuf(ox::checkbox19({ox::Checkbox19::State::Checked, false})),
                    vbuf(ox::checkbox20({ox::Checkbox20::State::Checked, false})),
                    ox::widget("buffer")
                ),
                ox::layout::horizontal(
                    vbuf(ox::checkbox1(ox::Checkbox1::State::Unchecked, true)),
                    vbuf(ox::checkbox2(ox::Checkbox2::State::Unchecked, true)),
                    vbuf(ox::checkbox3(ox::Checkbox3::State::Unchecked, true)),
                    vbuf(ox::checkbox4(ox::Checkbox4::State::Unchecked, true)),
                    vbuf(ox::checkbox5(ox::Checkbox5::State::Unchecked, true)),
                    vbuf(ox::checkbox6(ox::Checkbox6::State::Unchecked, true)),
                    vbuf(ox::checkbox7(ox::Checkbox7::State::Unchecked, true)),
                    vbuf(ox::checkbox8(ox::Checkbox8::State::Unchecked, true)),
                    vbuf(ox::checkbox9(ox::Checkbox9::State::Unchecked, true)),
                    vbuf(ox::checkbox10(ox::Checkbox10::State::Unchecked, true)),
                    vbuf(ox::checkbox11(ox::Checkbox11::State::Unchecked, true)),
                    vbuf(ox::checkbox12(ox::Checkbox12::State::Unchecked, true)),
                    vbuf(ox::checkbox13(ox::Checkbox13::State::Unchecked, true)),
                    vbuf(ox::checkbox14(ox::Checkbox14::State::Unchecked, true)),
                    vbuf(ox::checkbox15(ox::Checkbox15::State::Unchecked, true)),
                    vbuf(ox::checkbox16(ox::Checkbox16::State::Unchecked, true)),
                    vbuf(ox::checkbox17(ox::Checkbox17::State::Unchecked, true)),
                    vbuf(ox::checkbox18(ox::Checkbox18::State::Unchecked, true)),
                    vbuf(ox::checkbox19(ox::Checkbox19::State::Unchecked, true)),
                    vbuf(ox::checkbox20(ox::Checkbox20::State::Unchecked, true)),
                    ox::widget("buffer")
                )
            )
        );
        this->append_page(U"Labeled Horizontal Helpers - No Parameters",
            ox::layout::vertical(
                ox::hcheckbox1_label(), ox::hcheckbox_label(),
                ox::hlabel_checkbox1(), ox::hlabel_checkbox(),
                ox::hcheckbox2_label(), ox::hlabel_checkbox2(),
                ox::hcheckbox3_label(), ox::hlabel_checkbox4(),
                ox::hcheckbox7_label(), ox::hlabel_checkbox8(),
                ox::hcheckbox11_label(), ox::hlabel_checkbox12(),
                ox::hcheckbox15_label(), ox::hlabel_checkbox16(),
                ox::hcheckbox19_label(), ox::hlabel_checkbox19(),
                ox::hcheckbox20_label(), ox::hlabel_checkbox20(),
                ox::widget("buffer")
            )
        );
        this->append_page(U"Labeled Horizontal Helpers - Label Parameters",
            ox::layout::vertical(
                ox::hcheckbox1_label({U"Label!", ox::Align::Right}),
                ox::hcheckbox_label({U"Label!", ox::Align::Left}),
                ox::hlabel_checkbox1({U"Label!", ox::Align::Center}),
                ox::hlabel_checkbox({U"Label!", ox::Align::Right}),
                ox::hcheckbox2_label({U"Label!", ox::Align::Left}),
                ox::hlabel_checkbox2({U"Label!", ox::Align::Center}),
                ox::hcheckbox3_label({U"Label!", ox::Align::Right}),
                ox::hlabel_checkbox4({U"Label!", ox::Align::Left}),
                ox::hcheckbox7_label({U"Label!", ox::Align::Center}),
                ox::hlabel_checkbox8({U"Label!", ox::Align::Right}),
                ox::hcheckbox11_label({U"Label!", ox::Align::Left}),
                ox::hlabel_checkbox12({U"Label!", ox::Align::Center}),
                ox::hcheckbox15_label({U"Label!", ox::Align::Right}),
                ox::hlabel_checkbox16({U"Label!", ox::Align::Left}),
                ox::hcheckbox19_label({U"Label!", ox::Align::Center}),
                ox::hlabel_checkbox19({U"Label!", ox::Align::Right}),
                ox::hcheckbox20_label({U"Label!", ox::Align::Left}),
                ox::hlabel_checkbox20({U"Label!", ox::Align::Center}),
                ox::widget("buffer")
            )
        );
        this->append_page(
            U"Labeled Horizontal Helpers - Label and Checkbox Parameters",
            ox::layout::vertical(
                ox::hcheckbox1_label({U"Label!", ox::Align::Right},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::hcheckbox_label({U"Label!", ox::Align::Left},
                                    {ox::Checkbox1::State::Unchecked, true}),
                ox::hlabel_checkbox1({U"Label!", ox::Align::Center},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::hlabel_checkbox({U"Label!", ox::Align::Right},
                                    {ox::Checkbox1::State::Unchecked, false}),
                ox::hcheckbox2_label({U"Label!", ox::Align::Left},
                                     {ox::Checkbox2::State::Checked, true}),
                ox::hlabel_checkbox2({U"Label!", ox::Align::Center},
                                     {ox::Checkbox2::State::Unchecked, false}),
                ox::hcheckbox3_label({U"Label!", ox::Align::Right},
                                     {ox::Checkbox3::State::Checked, false}),
                ox::hlabel_checkbox4({U"Label!", ox::Align::Left},
                                     {ox::Checkbox4::State::Unchecked, false}),
                ox::hcheckbox7_label({U"Label!", ox::Align::Center},
                                     {ox::Checkbox7::State::Checked, true}),
                ox::hlabel_checkbox8({U"Label!", ox::Align::Right},
                                     {ox::Checkbox8::State::Unchecked, false}),
                ox::hcheckbox11_label({U"Label!", ox::Align::Left},
                                      {ox::Checkbox11::State::Checked, false}),
                ox::hlabel_checkbox12({U"Label!", ox::Align::Center},
                                      {ox::Checkbox12::State::Unchecked, true}),
                ox::hcheckbox15_label({U"Label!", ox::Align::Right},
                                      {ox::Checkbox15::State::Checked, false}),
                ox::hlabel_checkbox16({U"Label!", ox::Align::Left},
                                      {ox::Checkbox16::State::Unchecked, true}),
                ox::hcheckbox19_label({U"Label!", ox::Align::Center},
                                      {ox::Checkbox19::State::Checked, false}),
                ox::hlabel_checkbox19({U"Label!", ox::Align::Right},
                                      {ox::Checkbox19::State::Unchecked, true}),
                ox::hcheckbox20_label({U"Label!", ox::Align::Left},
                                      {ox::Checkbox20::State::Checked, false}),
                ox::hlabel_checkbox20({U"Label!", ox::Align::Center},
                                      {ox::Checkbox20::State::Checked, true}),
                ox::widget("buffer")
            )
        );
        this->append_page(U"Labeled Vertical Helpers - No Parameters",
            ox::layout::horizontal(
                ox::vcheckbox1_label(), ox::vcheckbox_label(),
                ox::vlabel_checkbox1(), ox::vlabel_checkbox(),
                ox::vcheckbox2_label(), ox::vlabel_checkbox2(),
                ox::vcheckbox5_label(), ox::vlabel_checkbox6(),
                ox::vcheckbox9_label(), ox::vlabel_checkbox10(),
                ox::vcheckbox13_label(), ox::vlabel_checkbox14(),
                ox::vcheckbox17_label(), ox::vlabel_checkbox18(),
                ox::vcheckbox19_label(), ox::vlabel_checkbox19(),
                ox::vcheckbox20_label(), ox::vlabel_checkbox20(),
                ox::widget()
            )
        );
        this->append_page(U"Labeled Vertical Helpers - Label Parameters",
            ox::layout::horizontal(
                ox::vcheckbox1_label  ({U"Label!", ox::Align::Right}),
                ox::vcheckbox_label   ({U"Label!", ox::Align::Left}),
                ox::vlabel_checkbox1  ({U"Label!", ox::Align::Center}),
                ox::vlabel_checkbox   ({U"Label!", ox::Align::Right}),
                ox::vcheckbox2_label  ({U"Label!", ox::Align::Left}),
                ox::vlabel_checkbox2  ({U"Label!", ox::Align::Center}),
                ox::vcheckbox5_label  ({U"Label!", ox::Align::Right}),
                ox::vlabel_checkbox6  ({U"Label!", ox::Align::Left}),
                ox::vcheckbox9_label  ({U"Label!", ox::Align::Center}),
                ox::vlabel_checkbox10 ({U"Label!", ox::Align::Right}),
                ox::vcheckbox13_label ({U"Label!", ox::Align::Left}),
                ox::vlabel_checkbox14 ({U"Label!", ox::Align::Center}),
                ox::vcheckbox17_label ({U"Label!", ox::Align::Right}),
                ox::vlabel_checkbox18 ({U"Label!", ox::Align::Left}),
                ox::vcheckbox19_label ({U"Label!", ox::Align::Center}),
                ox::vlabel_checkbox19 ({U"Label!", ox::Align::Right}),
                ox::vcheckbox20_label ({U"Label!", ox::Align::Left}),
                ox::vlabel_checkbox20 ({U"Label!", ox::Align::Center}),
                ox::widget("buffer")
            )
        );
        this->append_page(
            U"Labeled Vertical Helpers - Label and Checkbox Parameters",
            ox::layout::horizontal(
                ox::vcheckbox1_label({U"Label!", ox::Align::Right},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::vcheckbox_label({U"Label!", ox::Align::Left},
                                    {ox::Checkbox1::State::Unchecked, true}),
                ox::vlabel_checkbox1({U"Label!", ox::Align::Center},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::vlabel_checkbox({U"Label!", ox::Align::Right},
                                    {ox::Checkbox1::State::Unchecked, false}),
                ox::vcheckbox2_label({U"Label!", ox::Align::Left},
                                     {ox::Checkbox2::State::Checked, true}),
                ox::vlabel_checkbox2({U"Label!", ox::Align::Center},
                                     {ox::Checkbox2::State::Unchecked, false}),
                ox::vcheckbox5_label({U"Label!", ox::Align::Right},
                                     {ox::Checkbox5::State::Checked, false}),
                ox::vlabel_checkbox6({U"Label!", ox::Align::Left},
                                     {ox::Checkbox6::State::Unchecked, false}),
                ox::vcheckbox9_label({U"Label!", ox::Align::Center},
                                     {ox::Checkbox9::State::Checked, true}),
                ox::vlabel_checkbox10({U"Label!", ox::Align::Right},
                                     {ox::Checkbox10::State::Unchecked, false}),
                ox::vcheckbox13_label({U"Label!", ox::Align::Left},
                                      {ox::Checkbox13::State::Checked, false}),
                ox::vlabel_checkbox14({U"Label!", ox::Align::Center},
                                      {ox::Checkbox14::State::Unchecked, true}),
                ox::vcheckbox17_label({U"Label!", ox::Align::Right},
                                      {ox::Checkbox17::State::Checked, false}),
                ox::vlabel_checkbox18({U"Label!", ox::Align::Left},
                                      {ox::Checkbox18::State::Unchecked, true}),
                ox::vcheckbox19_label({U"Label!", ox::Align::Center},
                                      {ox::Checkbox19::State::Checked, false}),
                ox::vlabel_checkbox19({U"Label!", ox::Align::Right},
                                      {ox::Checkbox19::State::Unchecked, true}),
                ox::vcheckbox20_label({U"Label!", ox::Align::Left},
                                      {ox::Checkbox20::State::Checked, false}),
                ox::vlabel_checkbox20({U"Label!", ox::Align::Center},
                                      {ox::Checkbox20::State::Checked, true}),
                ox::widget("buffer")
            )
        );
        // clang-format on
        this->make_page<Runtime>(U"Runtime");
    }
};

int main() { return ox::System{}.run<Test>(); }
