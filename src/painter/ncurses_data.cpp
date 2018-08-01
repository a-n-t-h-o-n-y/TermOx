#include <cppurses/painter/detail/ncurses_data.hpp>
#include <mutex>
namespace cppurses {
namespace detail {

bool NCurses_data::resize_happened{false};
std::mutex NCurses_data::ncurses_mtx;

}  // namespace detail
}  // namespace cppurses
