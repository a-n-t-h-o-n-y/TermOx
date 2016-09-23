#ifndef SLOT_FWD_HPP
#define SLOT_FWD_HPP

#include <functional>

namespace mcurses
{

template <typename Signature, typename FunctionType = std::function<Signature>>
class slot;

} // namespace mcurses

#endif // SLOT_FWD_HPP