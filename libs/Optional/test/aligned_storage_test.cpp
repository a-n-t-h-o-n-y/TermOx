#include "optional/detail/aligned_storage.hpp"
#include <gtest/gtest.h>

TEST(AlignedStorageTest, DefaultConstructor) {
    opt::detail::Aligned_storage<int> i;
    i.ref() = 5;
    EXPECT_EQ(5, i.ref());

    opt::detail::Aligned_storage<const int> ci;
}
