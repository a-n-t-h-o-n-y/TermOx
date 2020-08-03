# Creating New Widgets

A new Widget can be defined by extending the `Widget` class. The main tool for
defining a Widget's behavior is overriding the event handler virtual functions
of `Widget`.  The constructor is commonly used to set [`Color`](colors.md)s and
[`Trait`](traits.md)s, the width and height [`Size_policy`](size-policy.md)
objects, and the [`Focus_policy`](focus-policy.md).

## Example

```cpp
class Foo : public cppurses::Widget {
   public:
    Foo();
};
```

# Extending Existing Widgets

The same tools can be used to extend existing Widgets by inheriting from the
Widget and overriding event handlers, or changing traits via the constructor.

## Example

```cpp
class Bar : public cppurses::... {
   public:
    
};
```

## See Also

- [Events](events.md)
- [Colors](colors.md)
- [Traits](traits.md)
- [Size Policy](size-policy.md)
- [Focus Policy](focus-policy.md)
