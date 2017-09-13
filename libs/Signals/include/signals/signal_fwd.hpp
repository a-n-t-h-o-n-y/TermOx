/// \file
/// \brief Contains the foward declaration of Signal. Use this header instead of
/// trying to forward declare the class yourself.
#ifndef SIGNAL_FWD_HPP
#define SIGNAL_FWD_HPP

#include <mutex>
#include <functional>

#include "optional_last_value.hpp"

#include "detail/function_type_splitter.hpp"

namespace sig {

template <typename Signature,
          typename Combiner = sig::Optional_last_value<
              typename sig::Function_type_splitter<Signature>::return_type>,
          typename Group = int,
          typename GroupCompare = std::less<Group>,
          typename SlotFunction = std::function<Signature>,
          typename Mutex = std::mutex>
class Signal;

}  // namespace sig

#endif  // SIGNAL_FWD_HPP
