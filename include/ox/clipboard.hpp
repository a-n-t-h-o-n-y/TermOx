/** @file
 * Application clipboard callbacks for copying and pasting UTF-8 text.
 */
#pragma once
#include <functional>
#include <string>
#include <string_view>
namespace ox {
/** Replace either function to integrate a host clipboard. The application supplies
 * an in-memory clipboard by default; callbacks execute on the dispatch thread. */
struct Clipboard {
    /** Return clipboard text by value; host callbacks must run on the application thread. */
    std::function<std::string()> read;
    /** Consume borrowed text during this call; copy it if the host retains clipboard
     * storage.
     */
    std::function<void(std::string_view)> write;
};
}  // namespace ox
