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
     std::cout <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
     return -1;
    }

    cv::Mat rgbImage, image;
    image = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

    if(!image.data)                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    cv::blur(image, image, cv::Size(3,3));
    cv::Canny(image, image, 50, 350, 3, true);

    cv::cvtColor(image, rgbImage, cv::COLOR_GRAY2RGB);

    // image.

    run(rgbImage);

    cv::imwrite( "out.png", rgbImage);


    return 0;
}
