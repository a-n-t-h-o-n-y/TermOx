#ifndef TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
#define TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
namespace ox::layout {

// TODO

/// Make any Linear_layout type passive.
/** A Passive Layout will always have the size policy that is the sum of its
 * children's size hints.*/
template <typename Layout_t>
class Passive : public Layout_t {
   public:
};

}  // namespace ox::layout
#endif  // TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
