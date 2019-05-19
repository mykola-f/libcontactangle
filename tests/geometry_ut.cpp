#include "gtest/gtest.h"
#include "geometry.hpp"

TEST(EuqlidianDistance, EuqlidianDistance)
{
    EXPECT_EQ(EuqlidianDistance({ cv::Point{5, 5}, cv::Point{10, 15} }), 11);
}