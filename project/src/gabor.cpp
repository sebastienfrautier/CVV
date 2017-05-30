#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

int main(int argc, char** argv )
{
    int num_filters = 8;
    int filter_size = 25;
    double sigma = 10.0;
    double lambd = 10.0;
    double gamma = 1.0;
    double psi = 0.0;
    std::vector<cv::Mat> filter_bank;
    for (int o = 0; o < num_filters; ++o)
    {
        double theta = (o * M_PI) / num_filters;
        cv::Mat gabor_kernel = cv::getGaborKernel( cv::Size(filter_size,filter_size), sigma, theta, lambd, gamma, psi, CV_32F );
        filter_bank.push_back(gabor_kernel); 
    }

    for ( int o = 0; o < num_filters; ++o)
    {
        cv::namedWindow("Gabor kernel", CV_WINDOW_NORMAL);
        cv::imshow("Gabor kernel", filter_bank[o]);
        cv::waitKey(0);
    }

    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cv::Mat image_grayscale;
    cv::cvtColor(image, image_grayscale, cv::COLOR_BGR2GRAY);
    cv::Mat dst;
    for ( int o = 0; o < num_filters; ++o)
    {
        // note that filter2D performs correlation, not convolution.
        // if we wanted convolution, we would have to flip the kernel.
        cv::filter2D(image_grayscale, dst, -1, filter_bank[o]);
        // Normalize to range [0, 255] and convert to CV_8UC1 for display
        cv::normalize(dst, dst, 0, 255, cv::NORM_MINMAX, CV_8UC1);
        cv::namedWindow("Gabor filtered", CV_WINDOW_NORMAL);
        cv::imshow("Gabor filtered", dst);
        cv::waitKey(0);
    }

    return 0;
}