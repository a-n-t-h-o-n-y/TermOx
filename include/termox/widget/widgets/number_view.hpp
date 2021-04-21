#ifndef TERMOX_WIDGET_WIDGETS_NUMBER_VIEW_HPP
#define TERMOX_WIDGET_WIDGETS_NUMBER_VIEW_HPP
#include <iomanip>
#include <ios>
#include <memory>
#include <sstream>
#include <type_traits>
#include <utility>

#include <termox/widget/align.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

/// Displays a number on a single horizontal line.
template <typename Number_t>
class Number_view : public HLabel {
    static_assert(std::is_integral_v<Number_t> ||
                  std::is_floating_point_v<Number_t>);

   public:
    struct Parameters {
        Number_t initial = 0;
        int precision    = 2;
        Align alignment  = Align::Left;
    };

   public:
    /// Precision is only used if Number_t is floating point.
    Number_view(Number_t initial = 0,
                int precision    = 2,
                Align alignment  = Align::Left)
        : HLabel{as_str(initial, precision), alignment},
          value_{initial},
          precision_{precision}
    {}

    Number_view(Parameters parameters)
        : Number_view{parameters.initial, parameters.precision,
                      parameters.alignment}
    {}

   public:
    /// Set a new value to display in the Number_view
    void set_value(Number_t x)
    {
        value_ = x;
        this->HLabel::set_text(as_str(value_, precision_));
    }

    /// Return the current value.
    [[nodiscard]] auto value() const noexcept -> Number_t { return value_; }

    /// Set a new value for floating point precision to display.
    void set_precision(int x)
    {
        precision_ = x;
        this->set_value(value_);
    }

    /// Return the current precision value.
    [[nodiscard]] auto precision() const noexcept -> int { return precision_; }

   public:
    using HLabel::alignment;
    using HLabel::set_alignment;

   private:
    Number_t value_;
    int precision_;

   private:
    [[nodiscard]] static auto as_str(Number_t value, int precision)
        -> std::string
    {
        if constexpr (std::is_floating_point_v<Number_t>) {
            auto ss = std::stringstream{};
            ss << std::fixed << std::setprecision(precision) << value;
            return ss.str();
        }
        else
            return std::to_string(value);
    }

   private:
    using HLabel::set_text;
};

/// Helper function to make a Number_view instance.
template <typename Number_t>
[[nodiscard]] auto number_view(Number_t initial = 0,
                               int precision    = 2,
                               Align alignment  = Align::Left)
    -> std::unique_ptr<Number_view<Number_t>>
{
    return std::make_unique<Number_view<Number_t>>(initial, precision,
                                                   alignment);
}

/// Helper function to make a Number_view instance.
template <typename Number_t>
[[nodiscard]] auto number_view(
    typename Number_view<Number_t>::Parameters parameters)
    -> std::unique_ptr<Number_view<Number_t>>
{
    return std::make_unique<Number_view<Number_t>>(std::move(parameters));
}

using Int_view = Number_view<int>;

/// Helper function to make an Int_view instance.
[[nodiscard]] inline auto int_view(int initial     = 0,
                                   int precision   = 2,
                                   Align alignment = Align::Left)
    -> std::unique_ptr<Int_view>
{
    return std::make_unique<Int_view>(initial, precision, alignment);
}

/// Helper function to make an Int_view instance.
[[nodiscard]] inline auto int_view(Int_view::Parameters parameters)
    -> std::unique_ptr<Int_view>
{
    return std::make_unique<Int_view>(std::move(parameters));
}

using Double_view = Number_view<double>;

/// Helper function to make a Double_view instance.
[[nodiscard]] inline auto double_view(double initial  = 0,
                                      int precision   = 2,
                                      Align alignment = Align::Left)
    -> std::unique_ptr<Double_view>
{
    return std::make_unique<Double_view>(initial, precision, alignment);
}

/// Helper function to make a Double_view instance.
[[nodiscard]] inline auto double_view(Double_view::Parameters parameters)
    -> std::unique_ptr<Double_view>
{
    return std::make_unique<Double_view>(std::move(parameters));
}

using Float_view = Number_view<float>;

/// Helper function to make a Float_view instance.
[[nodiscard]] inline auto float_view(float initial   = 0,
                                     int precision   = 2,
                                     Align alignment = Align::Left)
    -> std::unique_ptr<Float_view>
{
    return std::make_unique<Float_view>(initial, precision, alignment);
}

/// Helper function to make a Float_view instance.
[[nodiscard]] inline auto float_view(Float_view::Parameters parameters)
    -> std::unique_ptr<Float_view>
{
    return std::make_unique<Float_view>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_NUMBER_VIEW_HPP
