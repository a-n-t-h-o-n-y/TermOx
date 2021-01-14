#include <string>

#include <termox/system/system.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/float.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/checkbox.hpp>
#include <termox/widget/widgets/cycle_stack.hpp>
#include <termox/widget/widgets/label.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/notify_light.hpp>

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
    Checkboxes& boxes_ = make_child<Checkboxes>();
    Count& count       = make_child<Count>();

   public:
    Wall()
    {
        using namespace ox::pipe;
        count | fixed_height(1);

        boxes_ | children() | fixed_width(1);
        for (auto& row : boxes_.get_children()) {
            for (auto& child : row.get_children()) {
                child.checked.connect([this] { count.increment(); });
                child.unchecked.connect([this] { count.decrement(); });
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

            unchecked_display.wrapped.contents_modified.connect(
                [&cb](auto const& contents) {
                    cb.set_display({cb.get_display().checked, contents});
                });
            checked_display.wrapped.contents_modified.connect(
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

        // TODO make a Widget with template template layout parameter
        struct Divider : public Widget {
            Divider()
            {
                using namespace ox::pipe;
                *this | fixed_height(1) |
                    wallpaper(L'─' | fg(ox::Color::Light_gray));
            }
        };

       public:
        ox::Button& check_btn   = this->make_child<ox::Button>(L"Check");
        ox::Button& uncheck_btn = this->make_child<ox::Button>(L"Uncheck");
        ox::Button& toggle_btn  = this->make_child<ox::Button>(L"Toggle");

        Divider& div1     = this->make_child<Divider>();
        Checkbox& lock_cb = this->make_child<Checkbox>({L"Lock"});
        Divider& div2     = this->make_child<Divider>();

        Edit& unchecked_display = this->make_child<Edit>(
            Edit::Parameters{L"Unchecked Display", ox::Align::Left},
            L"☐");
        Edit& checked_display = this->make_child<Edit>(
            Edit::Parameters{L"Checked   Display", ox::Align::Left},
            L"☒");

        Divider& div3 = this->make_child<Divider>();

        Light_box& checked_lb = this->make_child<Light_box>(
            Light_box::Parameters{L"`checked`   Signal"},
            ox::Notify_light::Display{ox::Color::Yellow, ox::Color::Dark_gray},
            ox::Notify_light::Duration_t{350});
        Light_box& unchecked_lb = this->make_child<Light_box>(
            Light_box::Parameters{L"`unchecked` Signal"},
            ox::Notify_light::Display{ox::Color::Yellow, ox::Color::Dark_gray},
            ox::Notify_light::Duration_t{350});
        Light_box& toggled_lb = this->make_child<Light_box>(
            Light_box::Parameters{L"`toggled`   Signal"},
            ox::Notify_light::Display{ox::Color::Yellow, ox::Color::Dark_gray},
            ox::Notify_light::Duration_t{350});
    };

   public:
    Runtime()
    {
        using namespace ox::pipe;
        divider | fixed_width(1) | wallpaper(L'│' | fg(ox::Color::Light_gray));
    }

   private:
    using Checkbox = ox::Float_2d<ox::Checkbox1>;

   private:
    Checkbox& checkbox = this->make_child<Checkbox>();
    Widget& divider    = this->make_child();
    Checkbox_options& options =
        this->make_child<Checkbox_options>(checkbox.widget.widget);
};

class Test : public ox::Cycle_stack<> {
   public:
    Test()
    {
        this->make_page<Wall>(L"Wall");
        this->make_page<Horizontals>(L"Labeled Horizontals");
        this->make_page<Verticals>(L"Labeled Verticals");
        // clang-format off
        this->append_page(L"Checkbox() Helpers",
            ox::layout::vertical(
                ox::layout::horizontal(
                    ox::checkbox1(), ox::checkbox2(), ox::checkbox3(),
                    ox::checkbox4(), ox::checkbox5(), ox::checkbox6(),
                    ox::checkbox7(), ox::checkbox8(), ox::checkbox9(),
                    ox::checkbox10(), ox::checkbox11(), ox::checkbox12(),
                    ox::checkbox13(), ox::checkbox14(), ox::checkbox15(),
                    ox::checkbox16(), ox::checkbox17(), ox::checkbox18(),
                    ox::checkbox19(), ox::checkbox20()
                ),
                ox::layout::horizontal(
                    ox::checkbox1({}), ox::checkbox2({}), ox::checkbox3({}),
                    ox::checkbox4({}), ox::checkbox5({}), ox::checkbox6({}),
                    ox::checkbox7({}), ox::checkbox8({}), ox::checkbox9({}),
                    ox::checkbox10({}), ox::checkbox11({}), ox::checkbox12({}),
                    ox::checkbox13({}), ox::checkbox14({}), ox::checkbox15({}),
                    ox::checkbox16({}), ox::checkbox17({}), ox::checkbox18(),
                    ox::checkbox19({}), ox::checkbox20({})
                ),
                ox::layout::horizontal(
                    ox::checkbox1({ox::Checkbox1::State::Checked, false}),
                    ox::checkbox2({ox::Checkbox2::State::Checked, false}),
                    ox::checkbox3({ox::Checkbox3::State::Checked, false}),
                    ox::checkbox4({ox::Checkbox4::State::Checked, false}),
                    ox::checkbox5({ox::Checkbox5::State::Checked, false}),
                    ox::checkbox6({ox::Checkbox6::State::Checked, false}),
                    ox::checkbox7({ox::Checkbox7::State::Checked, false}),
                    ox::checkbox8({ox::Checkbox8::State::Checked, false}),
                    ox::checkbox9({ox::Checkbox9::State::Checked, false}),
                    ox::checkbox10({ox::Checkbox10::State::Checked, false}),
                    ox::checkbox11({ox::Checkbox11::State::Checked, false}),
                    ox::checkbox12({ox::Checkbox12::State::Checked, false}),
                    ox::checkbox13({ox::Checkbox13::State::Checked, false}),
                    ox::checkbox14({ox::Checkbox14::State::Checked, false}),
                    ox::checkbox15({ox::Checkbox15::State::Checked, false}),
                    ox::checkbox16({ox::Checkbox16::State::Checked, false}),
                    ox::checkbox17({ox::Checkbox17::State::Checked, false}),
                    ox::checkbox18({ox::Checkbox18::State::Checked, false}),
                    ox::checkbox19({ox::Checkbox19::State::Checked, false}),
                    ox::checkbox20({ox::Checkbox20::State::Checked, false})
                ),
                ox::layout::horizontal(
                    ox::checkbox1(ox::Checkbox1::State::Unchecked, true),
                    ox::checkbox2(ox::Checkbox2::State::Unchecked, true),
                    ox::checkbox3(ox::Checkbox3::State::Unchecked, true),
                    ox::checkbox4(ox::Checkbox4::State::Unchecked, true),
                    ox::checkbox5(ox::Checkbox5::State::Unchecked, true),
                    ox::checkbox6(ox::Checkbox6::State::Unchecked, true),
                    ox::checkbox7(ox::Checkbox7::State::Unchecked, true),
                    ox::checkbox8(ox::Checkbox8::State::Unchecked, true),
                    ox::checkbox9(ox::Checkbox9::State::Unchecked, true),
                    ox::checkbox10(ox::Checkbox10::State::Unchecked, true),
                    ox::checkbox11(ox::Checkbox11::State::Unchecked, true),
                    ox::checkbox12(ox::Checkbox12::State::Unchecked, true),
                    ox::checkbox13(ox::Checkbox13::State::Unchecked, true),
                    ox::checkbox14(ox::Checkbox14::State::Unchecked, true),
                    ox::checkbox15(ox::Checkbox15::State::Unchecked, true),
                    ox::checkbox16(ox::Checkbox16::State::Unchecked, true),
                    ox::checkbox17(ox::Checkbox17::State::Unchecked, true),
                    ox::checkbox18(ox::Checkbox18::State::Unchecked, true),
                    ox::checkbox19(ox::Checkbox19::State::Unchecked, true),
                    ox::checkbox20(ox::Checkbox20::State::Unchecked, true)
                )
            )
        );
        this->append_page(L"Labeled Horizontal Helpers - No Parameters",
            ox::layout::vertical(
                ox::hcheckbox1_label(),
                ox::hcheckbox_label(),
                ox::hlabel_checkbox1(),
                ox::hlabel_checkbox(),
                ox::hcheckbox2_label(),
                ox::hlabel_checkbox2(),
                ox::hcheckbox3_label(),
                ox::hlabel_checkbox4(),
                ox::hcheckbox7_label(),
                ox::hlabel_checkbox8(),
                ox::hcheckbox11_label(),
                ox::hlabel_checkbox12(),
                ox::hcheckbox15_label(),
                ox::hlabel_checkbox16(),
                ox::hcheckbox19_label(),
                ox::hlabel_checkbox19(),
                ox::hcheckbox20_label(),
                ox::hlabel_checkbox20()
            )
        );
        this->append_page(L"Labeled Horizontal Helpers - Label Parameters",
            ox::layout::vertical(
                ox::hcheckbox1_label({L"Label!", ox::Align::Right}),
                ox::hcheckbox_label({L"Label!", ox::Align::Left}),
                ox::hlabel_checkbox1({L"Label!", ox::Align::Center}),
                ox::hlabel_checkbox({L"Label!", ox::Align::Right}),
                ox::hcheckbox2_label({L"Label!", ox::Align::Left}),
                ox::hlabel_checkbox2({L"Label!", ox::Align::Center}),
                ox::hcheckbox3_label({L"Label!", ox::Align::Right}),
                ox::hlabel_checkbox4({L"Label!", ox::Align::Left}),
                ox::hcheckbox7_label({L"Label!", ox::Align::Center}),
                ox::hlabel_checkbox8({L"Label!", ox::Align::Right}),
                ox::hcheckbox11_label({L"Label!", ox::Align::Left}),
                ox::hlabel_checkbox12({L"Label!", ox::Align::Center}),
                ox::hcheckbox15_label({L"Label!", ox::Align::Right}),
                ox::hlabel_checkbox16({L"Label!", ox::Align::Left}),
                ox::hcheckbox19_label({L"Label!", ox::Align::Center}),
                ox::hlabel_checkbox19({L"Label!", ox::Align::Right}),
                ox::hcheckbox20_label({L"Label!", ox::Align::Left}),
                ox::hlabel_checkbox20({L"Label!", ox::Align::Center})
            )
        );
        this->append_page(
            L"Labeled Horizontal Helpers - Label and Checkbox Parameters",
            ox::layout::vertical(
                ox::hcheckbox1_label({L"Label!", ox::Align::Right},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::hcheckbox_label({L"Label!", ox::Align::Left},
                                    {ox::Checkbox1::State::Unchecked, true}),
                ox::hlabel_checkbox1({L"Label!", ox::Align::Center},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::hlabel_checkbox({L"Label!", ox::Align::Right},
                                    {ox::Checkbox1::State::Unchecked, false}),
                ox::hcheckbox2_label({L"Label!", ox::Align::Left},
                                     {ox::Checkbox2::State::Checked, true}),
                ox::hlabel_checkbox2({L"Label!", ox::Align::Center},
                                     {ox::Checkbox2::State::Unchecked, false}),
                ox::hcheckbox3_label({L"Label!", ox::Align::Right},
                                     {ox::Checkbox3::State::Checked, false}),
                ox::hlabel_checkbox4({L"Label!", ox::Align::Left},
                                     {ox::Checkbox4::State::Unchecked, false}),
                ox::hcheckbox7_label({L"Label!", ox::Align::Center},
                                     {ox::Checkbox7::State::Checked, true}),
                ox::hlabel_checkbox8({L"Label!", ox::Align::Right},
                                     {ox::Checkbox8::State::Unchecked, false}),
                ox::hcheckbox11_label({L"Label!", ox::Align::Left},
                                      {ox::Checkbox11::State::Checked, false}),
                ox::hlabel_checkbox12({L"Label!", ox::Align::Center},
                                      {ox::Checkbox12::State::Unchecked, true}),
                ox::hcheckbox15_label({L"Label!", ox::Align::Right},
                                      {ox::Checkbox15::State::Checked, false}),
                ox::hlabel_checkbox16({L"Label!", ox::Align::Left},
                                      {ox::Checkbox16::State::Unchecked, true}),
                ox::hcheckbox19_label({L"Label!", ox::Align::Center},
                                      {ox::Checkbox19::State::Checked, false}),
                ox::hlabel_checkbox19({L"Label!", ox::Align::Right},
                                      {ox::Checkbox19::State::Unchecked, true}),
                ox::hcheckbox20_label({L"Label!", ox::Align::Left},
                                      {ox::Checkbox20::State::Checked, false}),
                ox::hlabel_checkbox20({L"Label!", ox::Align::Center},
                                      {ox::Checkbox20::State::Checked, true})
            )
        );
        this->append_page(L"Labeled Vertical Helpers - No Parameters",
            ox::layout::horizontal(
                ox::vcheckbox1_label(),
                ox::vcheckbox_label(),
                ox::vlabel_checkbox1(),
                ox::vlabel_checkbox(),
                ox::vcheckbox2_label(),
                ox::vlabel_checkbox2(),
                ox::vcheckbox5_label(),
                ox::vlabel_checkbox6(),
                ox::vcheckbox9_label(),
                ox::vlabel_checkbox10(),
                ox::vcheckbox13_label(),
                ox::vlabel_checkbox14(),
                ox::vcheckbox17_label(),
                ox::vlabel_checkbox18(),
                ox::vcheckbox19_label(),
                ox::vlabel_checkbox19(),
                ox::vcheckbox20_label(),
                ox::vlabel_checkbox20()
            )
        );
        this->append_page(L"Labeled Vertical Helpers - Label Parameters",
            ox::layout::horizontal(
                ox::vcheckbox1_label  ({L"Label!", ox::Align::Right}),
                ox::vcheckbox_label   ({L"Label!", ox::Align::Left}),
                ox::vlabel_checkbox1  ({L"Label!", ox::Align::Center}),
                ox::vlabel_checkbox   ({L"Label!", ox::Align::Right}),
                ox::vcheckbox2_label  ({L"Label!", ox::Align::Left}),
                ox::vlabel_checkbox2  ({L"Label!", ox::Align::Center}),
                ox::vcheckbox5_label  ({L"Label!", ox::Align::Right}),
                ox::vlabel_checkbox6  ({L"Label!", ox::Align::Left}),
                ox::vcheckbox9_label  ({L"Label!", ox::Align::Center}),
                ox::vlabel_checkbox10 ({L"Label!", ox::Align::Right}),
                ox::vcheckbox13_label ({L"Label!", ox::Align::Left}),
                ox::vlabel_checkbox14 ({L"Label!", ox::Align::Center}),
                ox::vcheckbox17_label ({L"Label!", ox::Align::Right}),
                ox::vlabel_checkbox18 ({L"Label!", ox::Align::Left}),
                ox::vcheckbox19_label ({L"Label!", ox::Align::Center}),
                ox::vlabel_checkbox19 ({L"Label!", ox::Align::Right}),
                ox::vcheckbox20_label ({L"Label!", ox::Align::Left}),
                ox::vlabel_checkbox20 ({L"Label!", ox::Align::Center})
            )
        );
        this->append_page(
            L"Labeled Vertical Helpers - Label and Checkbox Parameters",
            ox::layout::horizontal(
                ox::vcheckbox1_label({L"Label!", ox::Align::Right},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::vcheckbox_label({L"Label!", ox::Align::Left},
                                    {ox::Checkbox1::State::Unchecked, true}),
                ox::vlabel_checkbox1({L"Label!", ox::Align::Center},
                                     {ox::Checkbox1::State::Checked, false}),
                ox::vlabel_checkbox({L"Label!", ox::Align::Right},
                                    {ox::Checkbox1::State::Unchecked, false}),
                ox::vcheckbox2_label({L"Label!", ox::Align::Left},
                                     {ox::Checkbox2::State::Checked, true}),
                ox::vlabel_checkbox2({L"Label!", ox::Align::Center},
                                     {ox::Checkbox2::State::Unchecked, false}),
                ox::vcheckbox5_label({L"Label!", ox::Align::Right},
                                     {ox::Checkbox5::State::Checked, false}),
                ox::vlabel_checkbox6({L"Label!", ox::Align::Left},
                                     {ox::Checkbox6::State::Unchecked, false}),
                ox::vcheckbox9_label({L"Label!", ox::Align::Center},
                                     {ox::Checkbox9::State::Checked, true}),
                ox::vlabel_checkbox10({L"Label!", ox::Align::Right},
                                     {ox::Checkbox10::State::Unchecked, false}),
                ox::vcheckbox13_label({L"Label!", ox::Align::Left},
                                      {ox::Checkbox13::State::Checked, false}),
                ox::vlabel_checkbox14({L"Label!", ox::Align::Center},
                                      {ox::Checkbox14::State::Unchecked, true}),
                ox::vcheckbox17_label({L"Label!", ox::Align::Right},
                                      {ox::Checkbox17::State::Checked, false}),
                ox::vlabel_checkbox18({L"Label!", ox::Align::Left},
                                      {ox::Checkbox18::State::Unchecked, true}),
                ox::vcheckbox19_label({L"Label!", ox::Align::Center},
                                      {ox::Checkbox19::State::Checked, false}),
                ox::vlabel_checkbox19({L"Label!", ox::Align::Right},
                                      {ox::Checkbox19::State::Unchecked, true}),
                ox::vcheckbox20_label({L"Label!", ox::Align::Left},
                                      {ox::Checkbox20::State::Checked, false}),
                ox::vlabel_checkbox20({L"Label!", ox::Align::Center},
                                      {ox::Checkbox20::State::Checked, true})
            )
        );
        // clang-format on
        this->make_page<Runtime>(L"Runtime");
    }
};

int main() { return ox::System{}.run<Test>(); }
