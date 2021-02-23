#ifndef TERMOX_WIDGET_LAYOUT_FIXED_HPP
#define TERMOX_WIDGET_LAYOUT_FIXED_HPP
#include <termox/widget/widget.hpp>

namespace ox {

/// Fixed offset on left and right sides of Widget_t object.
template <template <typename> typename Layout_t, typename Widget_t>
class Fixed_horizontal : public Layout_t<Widget> {
   private:
    using Base_t = Layout_t<Widget>;

   public:
    ox::Widget& buffer_1;
    Widget_t& widget;
    ox::Widget& buffer_2;

   public:
    template <typename W = Widget_t, typename... Args>
    Fixed_horizontal(std::size_t left_offset,
                     std::size_t right_offset,
                     Args&&... args)
        : offset_l_{left_offset},
          offset_r_{right_offset},
          widget_{&(this->template make_child<W>(std::forward<Args>(args)...))}
    {}

    template <typename W = Widget_t, typename... Args>
    Fixed_horizontal(std::size_t offset, Args&&... args)
        : offset_l_{offset},
          offset_r_{offset},
          widget_{&(this->template make_child<W>(std::forward<Args>(args)...))}
    {}

   public:
    [[nodiscard]] auto widget() -> Widget_t& { return *widget_; }

    // TODO new name, or is there a simpler way to handle the internal widget?
    template <typename W = Widget_t, typename... Args>
    void replace_widget(Args&&... args)
    {
        this->remove_child(widget_);
        widget_ = &(this->template make_child<W>(std::forward<Args>(args)...));
    }

   private:
    std::size_t offset_l_;
    std::size_t offset_r_;
    Widget_t* widget_;  // class invariant: this must always point to a valid w.
};

}  // namespace ox
#endif  // TERMOX_WIDGET_LAYOUT_FIXED_HPP
