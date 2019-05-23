#include "geometry.hpp"
#include "utils.hpp"

Line getNormal(cv::Point p1, cv::Point p2)
{
    auto d = p1 - p2;
    auto m = cv::Point {(p1.x + p2.x) / 2, (p1.y + p2.y) /2};
    return { cv::Point(m.x + d.y, m.y - d.x), m };
}

double EuqlidianDistance(Line line)
{
    return cv::norm(line[1] - line[0]);
}

double getSlope(Line line)
{
    auto diff = (line[1] - line[0]);
    if (diff.x == 0) {
        // this is what we want right now as getSlope is
        // used in std::sort for lines and we don't want
        // getSlope to throw in case the line is parallel
        // to OY axe.
        return signum(diff.y) * std::numeric_limits<double>::infinity();
    }

    return diff.y / diff.x;
}

// https://answers.opencv.org/question/9511/how-to-find-the-intersection-point-of-two-lines/
std::tuple<cv::Point, bool> findIntersection(Line line1, Line line2)
{
    auto x     = line2[0] - line1[0];
    auto dist1 = line1[1] - line1[0];
    auto dist2 = line2[1] - line2[0];

    cv::Matx<int, 2, 2> crossMatx = {
        dist1.x, dist1.y,
        dist2.x, dist2.y,
    };

    auto cross = cv::determinant(crossMatx.t());
    if (abs(cross) < 0) {
        return std::make_tuple(cv::Point(), false);
    }

    cv::Matx<int, 2, 2> t = {
        x.x,     x.y,
        dist2.x, dist2.y,
    };

    auto t1 = cv::determinant(t.t()) / cross;
    auto r = line1[0] + dist1 * t1;
    return std::make_tuple(r, true);
}

std::vector<cv::Point> getCircleLineIntersection(Line l, cv::Point center, int radius)
{
    std::vector<cv::Point> res;
    auto y = l[0].y - center.y;

    auto x1 = std::sqrt(radius * radius - y * y) + center.x;
    auto x2 = -std::sqrt(radius * radius - y * y) + center.x;

    res.push_back(cv::Point(x1, l[0].y));
    res.push_back(cv::Point(x2, l[0].y));

    return res;
}

Line getTangentToCircle(cv::Point point, cv::Point center, int radius)
{
    constexpr auto LineScale = 10000.0f;
    auto k  = -float(point.x - center.x) / (point.y - center.y);
    auto x1 = int(std::sqrt(LineScale / std::pow(k, 2)) + point.x);
    auto y1 = int(k * (x1 - point.x) + point.y);

    cv::Point second { x1, y1 };
    return { point, second };
}

double getInnerAngleBetweenLines(Line l1, Line l2)
{
    auto p1 = l1[0];
    auto p2 = l1[1];
    auto p3 = l2[0];
    auto p4 = l2[1];

    double angle1, angle2, angle;

    auto x1 = static_cast<double>(p1.x - p2.x);
    auto y1 = static_cast<double>(p1.y - p2.y);
    auto x2 = static_cast<double>(p3.x - p4.x);
    auto y2 = static_cast<double>(p3.y - p4.y);


    if (x1 != 0.0f) {
        angle1 = std::atan(y1 / x1);
    } else {
        angle1 = M_PI / 2.0; // 90 degrees
    }

    if (x2 != 0.0f) {
        angle2 = std::atan(y2 / x2);
    } else {
        angle2 = M_PI / 2.0; // 90 degrees
    }

    angle = std::fabs(angle2 - angle1);
    angle = angle * 180.0 / M_PI;

    return angle;
}
