#include "optional/none.hpp"
#include <gtest/gtest.h>

TEST(NoneTest, BoolConversion) {
    EXPECT_FALSE(opt::none);
}
