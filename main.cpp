#include "contactangle.hpp"

#include <iostream>

int main(int argc, char ** argv)
{
    if (argc != 2) {
        std::cout << " Usage: display_image ImageToLoadAndDisplay" << std::endl;
        return -1;
    }

    cv::Mat rgbImage, image;
    image = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

    if (!image.data) { // Check for invalid input
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    cv::blur(image, image, cv::Size(3, 3));
    cv::Canny(image, image, 50, 350, 3, true);

    cv::cvtColor(image, rgbImage, cv::COLOR_GRAY2RGB);

    // image.

    getContactAngle(rgbImage);

    cv::imwrite("out.png", rgbImage);


    return 0;
}
