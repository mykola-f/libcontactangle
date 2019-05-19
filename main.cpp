#include "geometry.hpp"
#include "utils.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <random>

using namespace cv;
using namespace std;

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
    // (x - cx)^2 + (y - cy)^2 = R^2
    // x(x2 - x1) + y(y2 - y1) = 0
    // k = (x1 - x2) / (y2 - y1)
    //
    // x^2 + k^2 * x^2 = R^2
    // x^2 = R^2 / (1 + k^2)

    std::vector<cv::Point> res;

    auto y = l[0].y - center.y;

    auto x1 = std::sqrt(radius * radius - y * y) + center.x;
    auto x2 = - std::sqrt(radius * radius - y * y) + center.x;


    // auto k = (l[0].x - l[1].x) / (l[0].y - l[1].y);
    // auto a = 1 + k * k;
    // auto b = 2*(k * center.y - center.x);
    // auto c = center.x * center.x + center.y * center.y - radius * radius;
    // auto D = b * b - 4 * a * c;

    // auto x1 = (-b - std::sqrt(D)) / (2*a);
    // auto x2 = (-b + std::sqrt(D)) / (2*a);

    res.push_back(cv::Point(x1, l[0].y));
    res.push_back(cv::Point(x2, l[0].y));
    return res;
}

//http://floating-point-gui.de/errors/comparison/
bool nearlyEqual(Line l, int a, int b, int epsilon) {
    int absA = abs(a);
    int absB = abs(b);
    int diff = abs(a - b);

    if (a == b) { // shortcut, handles infinities
        return true;
    } else if (a == 0 || b == 0 || diff == 0) {
        // a or b is zero or both are extremely close to it
        // relative error is less meaningful here
        return diff < 0;
    } else { // use relative error
        return diff / (absA + absB) < epsilon;
    }
}

std::vector<cv::Point> getAllWhitePixels(const cv::Mat& img, int brightnessThreshold=100) {
    std::vector<cv::Point> res;
    for (int i = 0; i < img.cols; i++) {
        for (int j = 0; j < img.rows; j++) {
            auto channels = img.at<cv::Vec3b>(cv::Point(i, j));
            auto red = channels[0];
            auto green = channels[1];
            auto blue = channels[2];
            auto brightness = (red + green + blue) / 3;
            if (brightness > brightnessThreshold) {
                res.push_back(cv::Point(i, j));
            }
        }
    }
    return res;
}


std::vector<cv::Point> getPointsOutsideCircle(std::vector<cv::Point> in, cv::Point center, int radius) {
    std::vector<cv::Point> res;
    for (auto p: in) {
        if (p.x < center.x - 1.1 * radius || p.x > center.x + 1.1 * radius) {
            res.push_back(p);
        }
    }
    return res;
}

std::vector<Line> getSurface(std::vector<cv::Point> points) {
    std::vector<Line> res;
    for (int i = 0; i < points.size(); i++) {
        cv::Point p1 = points[i];
        for (int k = i + 1; k < points.size(); k++) {
            cv::Point p2 = points[k];
            if (p1.x != p2.x) {
                res.push_back(Line({p1, p2}));
            }
        }
    }
    return res;
}

Line getTangentToCircle(cv::Point point, cv::Point center, int radius) {
    constexpr auto LineScale = 10000.0f;
    // y - y0 = k(x - x0)
    // (y1 - y0)^2 + (x1 - x0)^2 = 1
    // k^2(1 + (x1 - x0)^2) = 1
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

void run(cv::Mat& img) {
    auto points = getAllWhitePixels(img);
    std::vector<int> manywhites;

    cv::Point candidateCenter;
    int candidateRadius;

    int maxBrightnes = 0;

    for (int i = 0 ; i < 100000 ; i++) {
        auto p1 = random_element(points);
        auto p2 = random_element(points);
        auto p3 = random_element(points);

        while(EuqlidianDistance({p2, p1}) < img.cols/40) {
            p2 = random_element(points);
        }

        while(EuqlidianDistance({p3, p1}) < img.cols/40) {
            p3 = random_element(points);
        }

        auto line1 = getNormal(p1, p2);
        auto line2 = getNormal(p1, p3);

        auto intersectRes = findIntersection(line1, line2);
        auto foundIntersect = std::get<1>(intersectRes);
        auto intersectPoint = std::get<0>(intersectRes);

        if (!foundIntersect) {
            i--;
            continue;
        }

        auto radius = EuqlidianDistance({intersectPoint, p2});
        auto center = intersectPoint;

        auto checkMatch = [img](cv::Point center, int radius) {
            int lightPoints = 0;
            for (double theta = 0; theta < 2*M_PI; theta += M_PI/200) {
			    auto onCircle = cv::Point(radius*cos(theta) + center.x,
                                          radius*sin(theta) + center.y);
                if (onCircle.x >= img.cols || onCircle.x <= 0 ||
                    onCircle.y >= img.rows || onCircle.y <= 0) {
                    return 0;
                }
                auto channels = img.at<cv::Vec3b>(onCircle);
                auto red = channels[0];
                auto green = channels[1];
                auto blue = channels[2];
                auto brightness = (red + green + blue) / 3;
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
            maxBrightnes = brightness;
        }
    }
    std::cout << "Candidate circle {" << candidateCenter << ", " << candidateRadius << "}" << std::endl; 
    cv::circle(img, candidateCenter, candidateRadius, cv::Scalar(127, 255, 2));

    auto outside = getPointsOutsideCircle(points, candidateCenter, candidateRadius);
    for (auto p: outside) {
        cv::circle(img, p, 3, cv::Scalar(255, 0, 0));
    }

    auto surfaces = getSurface(outside);

    std::sort(std::begin(surfaces), std::end(surfaces),
        [](const Line& l1, const Line& l2){
            return getSlope(l1) < getSlope(l2);
        });
    
    auto slopeMedian = getSlope(surfaces[surfaces.size() / 2]);
    std::vector<int> intercepts;
    intercepts.resize(outside.size());
    for(int i = 0; i < intercepts.size(); i++) {
        intercepts[i] = outside[i].y - slopeMedian * outside[i].x;
    }
    auto interceptMedian = getMedian(intercepts);
    Line surface = {cv::Point(0, interceptMedian),
                    cv::Point(img.cols,
                              img.cols * slopeMedian + interceptMedian)
                    };
    cv::line(img, surface[0], surface[1], cv::Scalar(0, 0, 255));

    auto intersect = getCircleLineIntersection(surface, candidateCenter, candidateRadius);
    auto tang1 = getTangentToCircle(intersect[0], candidateCenter, candidateRadius);
    cv::line(img, tang1[0], tang1[1], cv::Scalar(128, 0, 255));

    auto tang2 = getTangentToCircle(intersect[1], candidateCenter, candidateRadius);
    cv::line(img, tang2[0], tang2[1], cv::Scalar(128, 0, 255));

    auto angle1 = getInnerAngleBetweenLines({intersect[0], intersect[1]}, tang1);
    std::cout << "theta (1) = " << angle1 << std::endl;

    auto angle2 = getInnerAngleBetweenLines({intersect[0], intersect[1]}, tang2);
    std::cout << "theta (2) = " << angle2 << std::endl;
}

int main( int argc, char** argv )
{

    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat rgbImage, image;
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

    if(!image.data)                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    cv::blur(image, image, cv::Size(3,3));
    cv::Canny(image, image, 50, 350, 3, true);

    cv::cvtColor(image, rgbImage, cv::COLOR_GRAY2RGB);

    // image.

    run(rgbImage);

    imwrite( "out.png", rgbImage);


    return 0;
}
