#ifndef TERMOX_WIDGET_GROWTH_HPP
#define TERMOX_WIDGET_GROWTH_HPP
namespace ox {

/// Classifies ways a Widget might change size based on its contents.
enum class Growth : bool {
    Static,  // Fixed size, not dependent on contents
    Dynamic  // Grows in some proportion to the contents
};

}  // namespace ox
#endif  // TERMOX_WIDGET_GROWTH_HPP
