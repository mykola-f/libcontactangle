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

std::tuple<cv::Point, bool> findIntersection(Line l1, Line l2) {
    auto x1 = l1[0].x;
    auto y1 = l1[0].y;
    auto x2 = l1[1].x;
    auto y2 = l1[1].y;
    auto x3 = l2[0].x;
    auto y3 = l2[0].y;
    auto x4 = l2[1].x;
    auto y4 = l2[1].y;

    if (((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4)) == 0) {
        return std::make_tuple(cv::Point(), false);
    }

    if ((y1-y2)*(x3-x4) < 1.1*(x1-x2)*(y3-y4) && (y1-y2)*(x3-x4) > 0.9*(x1-x2)*(y3-y4)) {
        return std::make_tuple(cv::Point(), false);
    }
    int x = ((x1*y2-x2*y1)*(x3-x4)-(x1-x2)*(x3*y4-x4*y3))
                / ((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
    int y = ((x1*y2-x2*y1)*(y3-y4)-(y1-y2)*(x3*y4-x4*y3))
                / ((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
    return std::make_tuple(cv::Point(x, y), true);
}

std::vector<cv::Point> getCircleLineIntersection(Line l, cv::Point center, int radius) {
    std::vector<cv::Point> res;
    auto y = l[0].y - center.y;

    auto x1 = std::sqrt(radius * radius - y * y) + center.x;
    auto x2 = - std::sqrt(radius * radius - y * y) + center.x;

    res.push_back(cv::Point(x1, l[0].y));
    res.push_back(cv::Point(x2, l[0].y));

    return res;
}


Line getTangentToCircle(cv::Point point, cv::Point center, int radius) {
    constexpr auto LineScale = 10000.0f;
    auto k = - float(point.x - center.x) / (point.y - center.y);
    auto x1 = int(std::sqrt(LineScale / std::pow(k, 2)) + point.x);
    auto y1 = int(k * (x1 - point.x) + point.y);
    cv::Point second {x1, y1};
    return {point, second};
}

double getInnerAngleBetweenLines(Line l1, Line l2) {
    auto p1 = l1[0];
    auto p2 = l1[1];
    auto p3 = l2[0];
    auto p4 = l2[1];

    double angle1 , angle2 , angle;

    double x1 = (float)p1.x - p2.x;
    double y1 = (float)p1.y - p2.y;
    double x2 = (float)p3.x - p4.x;
    double y2 = (float)p3.y - p4.y;

    if (x1 != 0.0f) {
       angle1 = std::atan(y1/x1);
    } else {
        angle1 = M_PI / 2.0;	// 90 degrees
    }

    if (x2 != 0.0f)
        angle2 = std::atan(y2/x2);
    else {
        angle2 = M_PI / 2.0;	// 90 degrees
    }

    angle = std::fabs(angle2 - angle1);
    angle = angle * 180.0 / M_PI;

    return angle;
}
