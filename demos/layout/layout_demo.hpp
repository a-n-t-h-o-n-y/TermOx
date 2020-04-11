#ifndef CPPURSES_DEMOS_LAYOUT_LAYOUT_DEMO_HPP
#define CPPURSES_DEMOS_LAYOUT_LAYOUT_DEMO_HPP
#include <signals/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/labeled_cycle_box.hpp>
#include <cppurses/widget/widgets/labeled_number_edit.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

#include <cppurses/widget/widgets/vertical_scrollbar.hpp>

namespace layout_demo {

struct Meta_widget : cppurses::Widget {
    Meta_widget() {}

    void select()
    {
        this->brush.set_background(cppurses::Color::White);
        this->brush.set_foreground(cppurses::Color::Black);
        this->update();
    }

    void unselect()
    {
        this->brush.set_background(cppurses::Color::Black);
        this->brush.set_foreground(cppurses::Color::White);
        this->update();
    }

   protected:
    auto paint_event() -> bool override
    {
        auto const w = std::to_string(this->width());
        auto const h = std::to_string(this->height());
        auto const x = std::to_string(this->parent()->x());
        auto const y = std::to_string(this->parent()->y());
        cppurses::Painter p{*this};
        p.put("X " + x, 0, 0);
        p.put("Y " + y, 0, 1);
        p.put("W " + w, 0, 2);
        p.put("H " + h, 0, 3);
        return Widget::paint_event();
    }
};

struct Workspace : cppurses::layout::Horizontal<Meta_widget> {
    sig::Signal<void(Meta_widget*)> selected;

    auto add_widget() -> Meta_widget*
    {
        auto& child = this->make_child();
        child.mouse_pressed.connect(
            [&child, this](auto const&) { this->selected(&child); });
        return &child;
    }
};

struct Size_policy_settings : cppurses::layout::Vertical<> {
    // private:
    //  struct Policy_type_box : cppurses::Labeled_cycle_box {
    //      Policy_type_box() : Labeled_cycle_box{"Policy"}
    //      {
    //          using cppurses::Size_policy;
    //          this->add_option(Size_policy::Ignored);
    //          this->add_option(Size_policy::Preferred);
    //          this->add_option(Size_policy::Fixed);
    //          this->add_option(Size_policy::Minimum);
    //          this->add_option(Size_policy::Maximum);
    //          this->add_option(Size_policy::Expanding);
    //          this->add_option(Size_policy::MinimumExpanding);
    //      }

    //      sig::Signal<void(cppurses::Size_policy::Type)> type_updated;

    //     private:
    //      void add_option(cppurses::Size_policy::Type type)
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
        title.brush.set_background(cppurses::Color::Dark_gray);

        // types_box.type_updated.connect([this](auto type) {
        //     size_policy_.type(type);
        //     this->notify();
        // });
        // types_box.label.brush.set_background(cppurses::Color::Dark_gray);
        // types_box.cycle_box.brush.set_background(cppurses::Color::Dark_gray);

        stretch.value_set.connect([this](double value) {
            size_policy_.stretch(value);
            this->notify();
        });
        stretch.label.brush.set_background(cppurses::Color::Dark_gray);

        hint.value_set.connect([this](std::size_t value) {
            size_policy_.hint(value);
            this->notify();
        });
        hint.label.brush.set_background(cppurses::Color::Dark_gray);

        min.value_set.connect([this](std::size_t value) {
            size_policy_.min(value);
            this->notify();
        });
        min.label.brush.set_background(cppurses::Color::Dark_gray);

        max.value_set.connect([this](std::size_t value) {
            size_policy_.max(value);
            this->notify();
        });
        max.label.brush.set_background(cppurses::Color::Dark_gray);
    }

    cppurses::Text_display& title{
        this->make_child<cppurses::Text_display>("[-] Width Policy")};

    // Policy_type_box& types_box{this->make_child<Policy_type_box>()};

    cppurses::Labeled_number_edit<double>& stretch{
        this->make_child<cppurses::Labeled_number_edit<double>>("Stretch ",
                                                                1.)};

    cppurses::Labeled_number_edit<std::size_t>& hint{
        this->make_child<cppurses::Labeled_number_edit<std::size_t>>("Hint    ",
                                                                     0)};

    cppurses::Labeled_number_edit<std::size_t>& min{
        this->make_child<cppurses::Labeled_number_edit<std::size_t>>("Min     ",
                                                                     0)};

    cppurses::Labeled_number_edit<std::size_t>& max{
        this->make_child<cppurses::Labeled_number_edit<std::size_t>>("Max     ",
                                                                     0)};

    /// set the internally held size_policy that is emitted to \p policy.
    void reset(cppurses::Size_policy const& policy)
    {
        size_policy_ = policy;
        // types_box.cycle_box.set_current_to(to_string(policy.type()));
        stretch.set_value(policy.stretch());
        hint.set_value(policy.hint());
        min.set_value(policy.min());
        max.set_value(policy.max());
    }

    sig::Signal<void(cppurses::Size_policy const&)> policy_updated;

   private:
    cppurses::Size_policy size_policy_;

    void notify() { this->policy_updated(size_policy_); }

    // static auto to_string(cppurses::Size_policy::Type type) -> std::string
    // {
    //     using cppurses::Size_policy;
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

struct Settings : cppurses::layout::Vertical<> {
    cppurses::Text_display& title{this->make_child<cppurses::Text_display>(
        cppurses::Glyph_string{"Settings", cppurses::Attribute::Bold})};
    Size_policy_settings& width_policy_settings{
        this->make_child<Size_policy_settings>()};
    cppurses::Button& add_btn{this->make_child<cppurses::Button>("Add Widget")};
    cppurses::Button& remove_btn{
        this->make_child<cppurses::Button>("Remove Selected")};

    Settings()
    {
        // this->width_policy.maximum(20);
        this->width_policy.fixed(20);

        title.height_policy.fixed(1);
        title.set_alignment(cppurses::Alignment::Center);
        title.brush.set_background(cppurses::Color::Dark_gray);

        width_policy_settings.policy_updated.connect(
            [this](cppurses::Size_policy const& p) {
                this->width_policy_updated(p);
            });

        add_btn.clicked.connect([this] { this->add_widget(); });
        add_btn.height_policy.fixed(3);
        add_btn.brush.set_background(cppurses::Color::Light_blue);
        add_btn.brush.set_foreground(cppurses::Color::Black);

        remove_btn.clicked.connect([this] { this->remove_selected(); });
        remove_btn.height_policy.fixed(3);
        remove_btn.brush.set_background(cppurses::Color::Violet);
        remove_btn.brush.set_foreground(cppurses::Color::Black);
    }

    /// update size_policy internals to selected widget's internals
    void new_selected(Meta_widget* w)
    {
        if (w == nullptr)
            return;
        width_policy_settings.reset(w->width_policy);
    }

    sig::Signal<void()> add_widget;
    sig::Signal<void()> remove_selected;
    sig::Signal<void(cppurses::Size_policy const&)> width_policy_updated;
};

struct Layout_demo : cppurses::layout::Horizontal<> {
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
            [this](cppurses::Size_policy const& p) {
                this->update_width_policy(p);
            });
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
        selected_->close();
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

    void update_width_policy(cppurses::Size_policy const& p)
    {
        if (selected_ == nullptr)
            return;
        selected_->width_policy = p;
        cppurses::System::post_event<cppurses::Child_polished_event>(
            workspace, *selected_);
    }

   private:
    Meta_widget* selected_{nullptr};
};

}  // namespace layout_demo
#endif  // CPPURSES_DEMOS_LAYOUT_LAYOUT_DEMO_HPP
