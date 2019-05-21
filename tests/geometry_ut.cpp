#include "gtest/gtest.h"
#include "geometry.hpp"

TEST(EuqlidianDistance, EuqlidianDistance)
{
    EXPECT_EQ(EuqlidianDistance({ cv::Point{5, 5}, cv::Point{10, 15} }), std::sqrt(125));
}

TEST(getNormal, getNormal)
{
    Line expected = { cv::Point{-3, 15}, cv::Point{7, 10}};
    EXPECT_EQ(getNormal(cv::Point{5, 5}, cv::Point{10, 15}), expected);
}

TEST(getSlope, getSlope)
{
    EXPECT_EQ(getSlope({ cv::Point{5, 5}, cv::Point{10, 15} }), 2.);
}

TEST(getSlope, getSlopeDivisionZero)
{
    // see comment in getSlope
    EXPECT_EQ(getSlope({ cv::Point{5, 5}, cv::Point{5, 15} }), std::numeric_limits<double>::infinity());
}


TEST(findIntersection, findIntersection)
{
    std::tuple<cv::Point, bool> expected{ cv::Point{5, 5}, true };
    EXPECT_EQ(findIntersection({{ cv::Point{0, 5}, cv::Point{10, 5} }},
    						   {{ cv::Point{5, 10}, cv::Point{5, 0} }}),
    						   expected);
}

TEST(findIntersection, findIntersectionParalell)
{
	auto result = findIntersection({{ cv::Point{0, 5}, cv::Point{10, 5} }},
								   {{ cv::Point{0, 0}, cv::Point{10, 0} }});
	EXPECT_EQ(std::get<1>(result), false);
}

TEST(findIntersection, findIntersectionCross)
{
    auto result = findIntersection({ { cv::Point{3, 5}, cv::Point{15, 15} } },
                                    { { cv::Point{8, 0}, cv::Point{10, 20} } });
    EXPECT_EQ(std::get<1>(result), true);
    EXPECT_EQ(std::get<0>(result), cv::Point(9, 10));
}

TEST(getCircleLineIntersection, getCircleLineIntersection)
{
	Line line = {{ cv::Point{0, 5}, cv::Point{10, 5} }};
	cv::Point center = {5, 5};
	int radius = 3;
	std::vector<cv::Point> expected = { cv::Point{8, 5}, cv::Point{2, 5} };
    EXPECT_EQ(getCircleLineIntersection(line, center, radius), expected);
}

/* TODO: figures could not intersect
TEST(getCircleLineIntersection, getCircleLineIntersectionNoIntersection)
{
    ASSERT_ANY_THROW(getCircleLineIntersection();
}
*/

TEST(getTangentToCircle, getTangentToCircle)
{
	Line expected = {{ cv::Point{400, 300}, cv::Point{400, std::numeric_limits<int>::min()} }};
	auto result = getTangentToCircle(cv::Point{400, 300}, cv::Point{300, 300}, 100);
    EXPECT_EQ(result, expected);
}

TEST(getInnerAngleBetweenLines, getInnerAngleBetweenLines)
{
	double expected = 90.0;
	auto result = getInnerAngleBetweenLines({{ cv::Point{0, 5}, cv::Point{0, 0} }},
    						   {{ cv::Point{0, 0}, cv::Point{3, 0} }});
    EXPECT_EQ(result, expected);
}