#pragma once

#include <opencv2/core/core.hpp>

#include <vector>
#include <array>
#include <tuple>

using Line = std::array<cv::Point, 2>;

Line
getNormal(cv::Point, cv::Point);
int EuqlidianDistance(Line);
int getSlope(Line);

std::tuple<cv::Point, bool> findIntersection(Line, Line);
// TODO: return std::array<cv::Point, 2>
std::vector<cv::Point>
getCircleLineIntersection(Line l, cv::Point center, int radius);


Line
getTangentToCircle(cv::Point point, cv::Point center, int radius);
double
getInnerAngleBetweenLines(Line l1, Line l2);
