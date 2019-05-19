#include "geometry.hpp"

Line getNormal(cv::Point p1, cv::Point p2) {
    auto dx = p1.x - p2.x;
    auto dy = p1.y - p2.y;
    auto midX = (p1.x + p2.x) / 2;
    auto midY = (p1.y + p2.y) / 2;
    return {cv::Point(midX + dy, midY - dx), cv::Point(midX, midY)};
}


int EuqlidianDistance(Line l) {
    return cv::norm(l[1] - l[0]);
}

int getSlope(Line line) {
    int slope = (line[1].y - line[0].y) / (line[1].x - line[0].x);
    return slope;
}
