#include "contactangle.hpp"

#include <iostream>

int main(int argc, char ** argv)
{
    if (argc != 2) {
        std::cout << " Usage: display_image ImageToLoadAndDisplay" << std::endl;
        return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    if (!image.data) { // Check for invalid input
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    getContactAngle(image);

    cv::imwrite("out.png", image);


    return 0;
}
