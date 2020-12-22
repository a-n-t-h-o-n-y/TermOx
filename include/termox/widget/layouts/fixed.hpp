#ifndef TERMOX_WIDGET_LAYOUT_FIXED_HPP
#define TERMOX_WIDGET_LAYOUT_FIXED_HPP
#include <cstddef>

#include <termox/system/system.hpp>
#include <termox/widget/layout/layout.hpp>

namespace cppurse::layout {

/// Fixed offset on left and right sides of Widget_t object.
template <typename Widget_t>
class Fixed_horizontal : public ox::Layout {
   public:
    template <typename W = Widget_t, typename... Args>
    Fixed_horizontal(std::size_t left_offset, std::size_t right_offset)
        : offset_l_{left_offset},
          offset_r_{right_offset},
          widget_{&(this->make_child<W>(std::forward<Args>(args)...))}
    {}

    template <typename W = Widget_t, typename... Args>
    Fixed_horizontal(std::size_t offset)
        : offset_l_{offset},
          offset_r_{offset},
          widget_{&(this->make_child<W>(std::forward<Args>(args)...))}
    {}

   public:
    auto widget() -> Widget_t& { return *widget_; }

    // TODO new name, or is there a simpler way to handle the internal widget?
    template <typename W = Widget_t, typename... Args>
    auto replace_widget(Args&&... args)
    {
        this->remove_child(widget_);
        widget_ = &(this->make_child<W>(std::forward<Args>(args)...));
    }

   protected:
    void update_geometry() override
    {
        // position
        // TODO make sure x position doesn't go off the end of widget, if it
        // does, then disable it? will that happen automatically? probably not.
        {
            auto const pos =
                Point{this->inner_x() + offset_l_, this->inner_y()};
            System::post_event(Move_event{*widget_}, pos;
        }
        // size
        {
            // TODO make sure the offset additions don't make width go below
            // zero.
            auto const area =
                Area{this->width() - (offset_l_ + offset_r_), this->height()};
            System::post_event(Resize_event{*widget_, area};
        }
    }

   private:
    std::size_t offset_l_;
    std::size_t offset_r_;
    Widget_t* widget_;  // class invariant: this must always point to a valid w.
};

}  // namespace cppurse::layout
#endif  // TERMOX_WIDGET_LAYOUT_FIXED_HPP
