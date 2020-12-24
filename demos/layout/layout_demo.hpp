#ifndef TERMOX_DEMOS_LAYOUT_LAYOUT_DEMO_HPP
#define TERMOX_DEMOS_LAYOUT_LAYOUT_DEMO_HPP
#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/size_policy.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/cycle_box.hpp>
#include <termox/widget/widgets/number_edit.hpp>
#include <termox/widget/widgets/text_display.hpp>

namespace layout_demo {

class Meta_widget : public ox::Widget {
   public:
    Meta_widget() {}

   public:
    void select()
    {
        using namespace ox;
        *this | bg(Color::White) | fg(Color::Black);
        this->update();
    }

    void unselect()
    {
        using namespace ox;
        *this | bg(Color::Black) | fg(Color::White);
        this->update();
    }

   protected:
    auto paint_event() -> bool override
    {
        auto const w = std::to_string(this->width());
        auto const h = std::to_string(this->height());
        auto const x = std::to_string(this->parent()->x());
        auto const y = std::to_string(this->parent()->y());

        auto p = ox::Painter{*this};
        p.put("X " + x, 0, 0);
        p.put("Y " + y, 0, 1);
        p.put("W " + w, 0, 2);
        p.put("H " + h, 0, 3);
        return Widget::paint_event();
    }
};

class Workspace : public ox::layout::Horizontal<Meta_widget> {
   public:
    sl::Signal<void(Meta_widget*)> selected;

   public:
    auto add_widget() -> Meta_widget*
    {
        auto& child = this->make_child();
        child.mouse_pressed.connect(
            [&child, this](auto const&) { this->selected(&child); });
        return &child;
    }
};

struct Size_policy_settings : ox::layout::Vertical<> {
    // private:
    //  struct Policy_type_box : ox::Labeled_cycle_box {
    //      Policy_type_box() : Labeled_cycle_box{"Policy"}
    //      {
    //          using ox::Size_policy;
    //          this->add_option(Size_policy::Ignored);
    //          this->add_option(Size_policy::Preferred);
    //          this->add_option(Size_policy::Fixed);
    //          this->add_option(Size_policy::Minimum);
    //          this->add_option(Size_policy::Maximum);
    //          this->add_option(Size_policy::Expanding);
    //          this->add_option(Size_policy::MinimumExpanding);
    //      }

    //      sl::Signal<void(ox::Size_policy::Type)> type_updated;

    //     private:
    //      void add_option(ox::Size_policy::Type type)
    //      {
    //          this->cycle_box.add_option(to_string(type)).connect([this, type]
    //          {
    //              type_updated(type);
    //          });
    //      }
    //  };

   public:
    Size_policy_settings()
    {
        // this->height_policy.fixed(6);

        title.height_policy.fixed(1);
        title.brush.background = ox::Color::Dark_gray;

        // types_box.type_updated.connect([this](auto type) {
        //     size_policy_.type(type);
        //     this->notify();
        // });
        // types_box.label.brush.background     = ox::Color::Dark_gray;
        // types_box.cycle_box.brush.background = ox::Color::Dark_gray;

        stretch.value_set.connect([this](double value) {
            size_policy_.stretch(value);
            this->notify();
        });
        stretch.label.brush.background = ox::Color::Dark_gray;

        hint.value_set.connect([this](std::size_t value) {
            size_policy_.hint(value);
            this->notify();
        });
        hint.label.brush.background = ox::Color::Dark_gray;

        min.value_set.connect([this](std::size_t value) {
            size_policy_.min(value);
            this->notify();
        });
        min.label.brush.background = ox::Color::Dark_gray;

        max.value_set.connect([this](std::size_t value) {
            size_policy_.max(value);
            this->notify();
        });
        max.label.brush.background = ox::Color::Dark_gray;
    }

    ox::Text_display& title{
        this->make_child<ox::Text_display>("[-] Width Policy")};

    // Policy_type_box& types_box{this->make_child<Policy_type_box>()};

    ox::Labeled_number_edit<double>& stretch{
        this->make_child<ox::Labeled_number_edit<double>>("Stretch ", 1.)};

    ox::Labeled_number_edit<std::size_t>& hint{
        this->make_child<ox::Labeled_number_edit<std::size_t>>("Hint    ", 0)};

    ox::Labeled_number_edit<std::size_t>& min{
        this->make_child<ox::Labeled_number_edit<std::size_t>>("Min     ", 0)};

    ox::Labeled_number_edit<std::size_t>& max{
        this->make_child<ox::Labeled_number_edit<std::size_t>>("Max     ", 0)};

    /// set the internally held size_policy that is emitted to \p policy.
    void reset(ox::Size_policy const& policy)
    {
        size_policy_ = policy;
        // types_box.cycle_box.set_current_to(to_string(policy.type()));
        stretch.set_value(policy.stretch());
        hint.set_value(policy.hint());
        min.set_value(policy.min());
        max.set_value(policy.max());
    }

    sl::Signal<void(ox::Size_policy const&)> policy_updated;

   private:
    ox::Size_policy size_policy_;

    void notify() { this->policy_updated(size_policy_); }

    // static auto to_string(ox::Size_policy::Type type) -> std::string
    // {
    //     using ox::Size_policy;
    //     switch (type) {
    //         case Size_policy::Fixed: return "Fixed";
    //         case Size_policy::Minimum: return "Minimum";
    //         case Size_policy::Maximum: return "Maximum";
    //         case Size_policy::Preferred: return "Preferred";
    //         case Size_policy::Expanding: return "Expanding";
    //         case Size_policy::MinimumExpanding: return "MinExpanding";
    //         case Size_policy::Ignored: return "Ignored";
    //     }
    // }
};

struct Settings : ox::layout::Vertical<> {
    ox::Text_display& title{this->make_child<ox::Text_display>(
        ox::Glyph_string{"Settings", ox::Trait::Bold})};
    Size_policy_settings& width_policy_settings{
        this->make_child<Size_policy_settings>()};
    ox::Button& add_btn{this->make_child<ox::Button>("Add Widget")};
    ox::Button& remove_btn{this->make_child<ox::Button>("Remove Selected")};

    Settings()
    {
        // this->width_policy.maximum(20);
        this->width_policy.fixed(20);

        title.height_policy.fixed(1);
        title.set_alignment(ox::Align::Center);
        title.brush.background = ox::Color::Dark_gray;

        width_policy_settings.policy_updated.connect(
            [this](ox::Size_policy const& p) {
                this->width_policy_updated(p);
            });

        add_btn.pressed.connect([this] { this->add_widget(); });
        add_btn.height_policy.fixed(3);
        add_btn.brush.background = ox::Color::Light_blue;
        add_btn.brush.foreground = ox::Color::Black;

        remove_btn.pressed.connect([this] { this->remove_selected(); });
        remove_btn.height_policy.fixed(3);
        remove_btn.brush.background = ox::Color::Violet;
        remove_btn.brush.foreground = ox::Color::Black;
    }

    /// update size_policy internals to selected widget's internals
    void new_selected(Meta_widget* w)
    {
        if (w == nullptr)
            return;
        width_policy_settings.reset(w->width_policy);
    }

    sl::Signal<void()> add_widget;
    sl::Signal<void()> remove_selected;
    sl::Signal<void(ox::Size_policy const&)> width_policy_updated;
};

struct Layout_demo : ox::layout::Horizontal<> {
    Workspace& workspace{this->make_child<Workspace>()};
    Settings& settings{this->make_child<Settings>()};

    Layout_demo()
    {
        workspace.selected.connect([this](Meta_widget* w) {
            this->set_selected(w);
            settings.new_selected(w);
        });
        settings.add_widget.connect([this] { this->add_widget(); });
        settings.remove_selected.connect([this] { this->remove_selected(); });
        settings.width_policy_updated.connect(
            [this](ox::Size_policy const& p) { this->update_width_policy(p); });
    }

    void add_widget()
    {
        auto* added = workspace.add_widget();
        if (selected_ == nullptr)
            this->set_selected(added);
    }

    void remove_selected()
    {
        if (selected_ == nullptr)
            return;
        workspace.remove_and_delete_child(selected_);
        selected_ = nullptr;
    }

    void set_selected(Meta_widget* w)
    {
        if (selected_ != nullptr)
            selected_->unselect();
        selected_ = w;
        if (selected_ != nullptr)
            selected_->select();
    }

    void update_width_policy(ox::Size_policy const& p)
    {
        if (selected_ == nullptr)
            return;
        selected_->width_policy = p;
        ox::System::post_event(ox::Child_polished_event{workspace, *selected_});
    }

   private:
    Meta_widget* selected_{nullptr};
};

}  // namespace layout_demo
#endif  // TERMOX_DEMOS_LAYOUT_LAYOUT_DEMO_HPP
