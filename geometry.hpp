#pragma once

#include <opencv2/core/core.hpp>

#include <array>

using Line = std::array<cv::Point, 2>;

Line getNormal(cv::Point, cv::Point);
int EuqlidianDistance(Line);
int getSlope(Line);