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

    ContactAngleResult res;

    try {
        res = getContactAngle(image);
    } catch(const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }

    std::cout << "Candidate circle {" << res.center << ", " << res.radius << "}" << std::endl;
    std::cout << "Surface {" << res.surface[0] << ", " << res.surface[1] << "}" << std::endl;
    std::cout << "theta (1) = " << res.theta1 << std::endl;
    std::cout << "theta (2) = " << res.theta2 << std::endl;

    cv::imwrite("out.png", image);


    return 0;
}
