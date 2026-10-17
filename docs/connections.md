# Connections and lifetime

Signals own their callbacks. A tracked widget argument borrows a widget identity
and supplies its current location when the signal is emitted.

```cpp
#include <ox/widget.hpp>

sl::Signal<void(int)> changed;
ox::Widget target;
auto id = ox::Connection{
    .signal = changed,
    .slot = [](int value, ox::Widget& widget) { widget.enabled = value != 0; },
}(target);
changed(1);
changed.disconnect(id);
```

## On this page

- [Connection ownership](#connection-ownership)
- [Tracked widgets](#tracked-widgets)
- [Signal emission](#signal-emission)

## Connection ownership

Constructing Connection only configures it; invoke it immediately to subscribe.
The signal owns the callback and its captures. Invoking Connection returns a
SignalsLight identifier for explicit disconnection. The identifier does not
disconnect the callback when it is destroyed.
Dropping the identifier keeps the slot connected. A disconnected slot returned by
signal.disconnect can still be called and keeps its captures alive while retained.
Callbacks must be copyable, as required by the signal's std::function storage.

## Tracked widgets

Tracked widgets are borrowed. Their references follow moves, and callbacks are skipped
if any target expires or loses its expected type through slicing. Const targets work.
Empty callbacks are rejected before wrapping/subscribing. Calling tracked directly
composes the same borrowing rules without connecting a signal. Signal invocation and
widget mutation belong to the application thread. Callback-owned captures survive
expiration of a tracked target until the slot is removed or its signal is destroyed.
Pass widgets as tracked arguments rather than capturing references to their locations.

## Signal emission

Signal emission snapshots entries: newly connected slots wait until the next emission,
disconnected pending slots are skipped, and destroying the signal safely cancels pending
callbacks. Mutable callbacks retain their state between emissions; copies own independent
callback state. TermOx fetches the fixed upstream SignalsLight revision through CMake.

See [Widget API](widget-api.md#timers-and-lifetime) for weak views and timers,
and the [Connection reference](reference/application.md#oxconnection) for signal
argument order and explicit disconnection.
