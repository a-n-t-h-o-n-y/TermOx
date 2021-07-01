# Log Widget

[`<termox/widget/widgets/log.hpp>`](../../../include/termox/widget/widgets/log.hpp)

A scrollable list of logged messages. Received messages are posted at the bottom
of the Log.

```cpp
class Log : public Textbox {
   public:
    void post_message(Glyph_string message);
};
```
