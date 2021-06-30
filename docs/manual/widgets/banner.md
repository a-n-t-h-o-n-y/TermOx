# Banner Widget

[`<termox/widget/widgets/banner.hpp>`](../../../include/termox/widget/widgets/banner.hpp)

A Label with animated text display. An Animator class defines how the text
appears on the Label.

```cpp
// Animated Text Line/Label
template <typename Animator>
class Banner : public Widget {
   public:
    using Interval_t = std::chrono::milliseconds;

    struct Parameters {
        Glyph_string text   = U"";
        Interval_t interval = Interval_t{50};
        Animator animator   = Animator{};
    };

   public:
    explicit Banner(Glyph_string text   = U"",
                    Interval_t interval = std::chrono::milliseconds{50},
                    Animator animator   = Animator{});

    explicit Banner(Parameters);

   public:
    void set_text(Glyph_string text);

    auto text() const -> Glyph_string const&;

    void set_interval(Interval_t interval);

    auto interval() const -> Interval_t;

    auto animator() const -> Animator const&;

    auto animator() -> Animator&;
};
```

## Animators

```cpp
namespace ox::animator {

/// Left to right reveal of text, hold, left to right clearing of text.
class Scan;

/// Left to right reveal of text, then hold.
class Persistent_scan;

/// Holds, then scrolls from left to right, then restarts.
class Scroll;

/// Same as Scroll if text length is longer that display length, else static.
class Conditional_scroll;

/// Starts with scrambled text and sorts itself one Glyph at a time.
class Unscramble;

} // namespace ox::animator
```

## Type Aliases

```cpp
using Scan_banner               = Banner<animator::Scan>;
using Persistent_scan_banner    = Banner<animator::Persistent_scan>;
using Random_banner             = Banner<animator::Random>;
using Scroll_banner             = Banner<animator::Scroll>;
using Conditional_scroll_banner = Banner<animator::Conditional_scroll>;
using Unscramble_banner         = Banner<animator::Unscramble>;
```
