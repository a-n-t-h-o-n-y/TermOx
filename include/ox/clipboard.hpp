#pragma once
#include <functional>
#include <string>
#include <string_view>
namespace ox {
/** Replace either function to integrate a host clipboard. The application supplies
 * an in-memory clipboard by default; callbacks execute on the dispatch thread. */
struct Clipboard {
    std::function<std::string()> read;
    std::function<void(std::string_view)> write;
};
}  // namespace ox
