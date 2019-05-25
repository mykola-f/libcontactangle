#include "contactangle.hpp"

#include <algorithm>
#include <iostream>

int getBrightness(const cv::Vec3b channels) {
    auto red   = channels[0];
    auto green = channels[1];
    auto blue  = channels[2];
    return (red + green + blue) / 3;
}

std::vector<cv::Point> getAllWhitePixels(const cv::Mat& img, int brightnessThreshold = 100)
{
    std::vector<cv::Point> res;
    for (int i = 0; i < img.cols; i++) {
        for (int j = 0; j < img.rows; j++) {
            auto channels   = img.at<cv::Vec3b>(cv::Point(i, j));
            auto brightness = getBrightness(channels);
            if (brightness > brightnessThreshold) {
                res.push_back(cv::Point(i, j));
            }
        }
    }
    return res;
}

std::vector<cv::Point> getPointsOutsideCircle(const std::vector<cv::Point>& in, cv::Point center, int radius)
{
    std::vector<cv::Point> res;
    for (auto p: in) {
        if (p.x < center.x - radius || p.x > center.x + radius) {
            res.push_back(p);
        }
    }
    return res;
}

std::vector<Line> getSurface(const std::vector<cv::Point>& points)
{
    std::vector<Line> res;
    for (int i = 0; i < points.size(); i++) {
        cv::Point p1 = points[i];
        for (int k = i + 1; k < points.size(); k++) {
            cv::Point p2 = points[k];
            if (p1.x != p2.x) {
                res.push_back(Line({ p1, p2 }));
            }
        }
    }
    return res;
}

ContactAngleResult getContactAngle(cv::Mat& img)
{
    ContactAngleResult result;

    // convert to gray for blur and edge detection
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    // blur image to reduce noise
    cv::blur(img, img, cv::Size(3, 3));
    // apply Canny edge detection algorithm
    cv::Canny(img, img, 50, 350, 3, true);
    // convert back to rgb
    // (TODO: really needed this convert back and forth ???
    //        Uhh... at least it works as is, so I wont touch it)
    cv::cvtColor(img, img, cv::COLOR_GRAY2RGB);

    auto points = getAllWhitePixels(img);

    cv::Point candidateCenter;
    int candidateRadius;

    int maxBrightnes = 0;

    for (int i = 0; i < 100000; i++) {
        auto p1 = random_element(points);
        auto p2 = random_element(points);
        auto p3 = random_element(points);

        while (EuqlidianDistance({ p2, p1 }) < img.cols / 40.f) {
            p2 = random_element(points);
        }

        while (EuqlidianDistance({ p3, p1 }) < img.cols / 40.f) {
            p3 = random_element(points);
        }

        auto line1 = getNormal(p1, p2);
        auto line2 = getNormal(p1, p3);

        auto intersectRes   = findIntersection(line1, line2);
        auto foundIntersect = std::get<1>(intersectRes);
        auto intersectPoint = std::get<0>(intersectRes);

        if (!foundIntersect) {
            i--;
            continue;
        }

        // TODO: remove static_cast
        auto radius = static_cast<int>(EuqlidianDistance({ intersectPoint, p2 }));
        auto center = intersectPoint;

        auto checkMatch = [img](cv::Point center, int radius){
              int lightPoints = 0;

              for (double theta = 0; theta < 2 * M_PI; theta += M_PI / 200) {
                  auto onCircle = cv::Point(radius * cos(theta) + center.x,
                      radius * sin(theta) + center.y);
                  if (onCircle.x >= img.cols || onCircle.x <= 0 ||
                    onCircle.y >= img.rows || onCircle.y <= 0)
                  {
                      return 0;
                  }
                  auto channels   = img.at<cv::Vec3b>(onCircle);
                  auto brightness = getBrightness(channels);
                  if (brightness > 200) {
                      lightPoints++;
                  }
              }
              return lightPoints * radius;
          };

        auto brightness = checkMatch(center, radius);
        if (brightness > maxBrightnes) {
            candidateCenter = center;
            candidateRadius = radius;
            maxBrightnes    = brightness;
        }
    }
    std::cout << "Candidate circle {" << candidateCenter << ", " << candidateRadius << "}" << std::endl;
    cv::circle(img, candidateCenter, candidateRadius, cv::Scalar(127, 255, 2));

    auto outside = getPointsOutsideCircle(points, candidateCenter, candidateRadius);
    for (auto p: outside) {
        cv::circle(img, p, 3, cv::Scalar(255, 0, 0));
    }

    auto surfaces = getSurface(outside);

    if (surfaces.empty()) {
        throw std::runtime_error("Surface not found");
    }

    std::sort(std::begin(surfaces), std::end(surfaces),
      [](const Line& l1, const Line& l2){
        return getSlope(l1) < getSlope(l2);
    });

    auto slopeMedian = getSlope(surfaces[surfaces.size() / 2]);
    std::vector<int> intercepts;
    intercepts.resize(outside.size());
    for (int i = 0; i < intercepts.size(); i++) {
        intercepts[i] = outside[i].y - slopeMedian * outside[i].x;
    }
    auto interceptMedian = getMedian(intercepts);
    Line surface         = { cv::Point(0, interceptMedian),
                             cv::Point(img.cols,
                               img.cols * slopeMedian + interceptMedian) };
    cv::line(img, surface[0], surface[1], cv::Scalar(0, 0, 255));

    auto intersect = getCircleLineIntersection(surface, candidateCenter, candidateRadius);
    auto tang1     = getTangentToCircle(intersect[0], candidateCenter, candidateRadius);
    cv::line(img, tang1[0], tang1[1], cv::Scalar(128, 0, 255));

    auto tang2 = getTangentToCircle(intersect[1], candidateCenter, candidateRadius);
    cv::line(img, tang2[0], tang2[1], cv::Scalar(128, 0, 255));

    auto theta1 = getInnerAngleBetweenLines({ intersect[0], intersect[1] }, tang1);
    std::cout << "theta (1) = " << theta1 << std::endl;

    auto theta2 = getInnerAngleBetweenLines({ intersect[0], intersect[1] }, tang2);
    std::cout << "theta (2) = " << theta2 << std::endl;

    // fill in result
    result.center  = candidateCenter;
    result.radius  = candidateRadius;
    result.surface = surface;
    result.theta1  = theta1;
    result.theta2  = theta2;

    return result;
} // run
