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
    if (std::fabs(cross) < std::numeric_limits<double>::epsilon()) {
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

    // for simplicity right now assume line is parallel to OX
    // which is the case for our surface
    if (l[0].y != l[1].y) {
        throw std::runtime_error("(OX axes || line) is assumed in calculations");
    }

    if (l[0].y < (center.y - radius) || l[0].y > (center.y + radius)) {
        return res;
    }

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

double getInnerAngleBetweenLines(Line line1, Line line2)
{
    auto slope1 = getSlope(line1);
    auto slope2 = getSlope(line2);

    auto diff = slope2 - slope1;

    return std::atan(diff) / M_PI * 180;
}
