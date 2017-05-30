#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "gauss_pyr.h"

cv::Mat across_scale_addition(const std::vector<cv::Mat>& scale_images)
{
    cv::Size im_size = scale_images[0].size();
    cv::Mat result = scale_images[0];
    for (int i = 1; i < scale_images.size(); ++i)
    {
        cv::Mat resized;
        if ( scale_images[i].size() != im_size )
        {
            cv::resize(scale_images[i], resized, im_size, 0, 0, cv::INTER_CUBIC);
        }
        else
        {
            resized = scale_images[i]; // Fixed bug here: should us [i] not [0]
        }

        result += resized;
    }
    return result;
}

int main(int argc, char** argv )
{
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    cv::Mat lab;
    cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);
    lab.convertTo(lab, CV_32F);
    std::vector<cv::Mat> lab_channels;
    cv::split(lab, lab_channels);

    float center_sigma = 2.0;
    float surround_sigma = 6.0;

    int num_layers = 3;
    gauss_pyr c_pyr_l(lab_channels[0], num_layers, center_sigma);
    gauss_pyr c_pyr_a(lab_channels[1], num_layers, center_sigma);
    gauss_pyr c_pyr_b(lab_channels[2], num_layers, center_sigma);
    gauss_pyr s_pyr_l(lab_channels[0], num_layers, surround_sigma);
    gauss_pyr s_pyr_a(lab_channels[1], num_layers, surround_sigma);
    gauss_pyr s_pyr_b(lab_channels[2], num_layers, surround_sigma);

    // Example: CS and SC contrasts, a channel (red-green vs. green-red)
    cv::namedWindow("Center-surround contrast", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Surround-center contrast", CV_WINDOW_AUTOSIZE);
    for (int i = 0; i < num_layers; ++i)
    {
        cv::Mat center = c_pyr_a.get(i);
        cv::Mat surround = s_pyr_a.get(i);
        
        cv::Mat CS = center - surround;
        cv::threshold(CS, CS, 0, 1, cv::THRESH_TOZERO);

        cv::Mat SC = surround - center;
        cv::threshold(SC, SC, 0, 1, cv::THRESH_TOZERO);

        cv::imshow("Center-surround contrast", CS );
        cv::imshow("Surround-center contrast", SC );
        cv::waitKey(0);
    }

    // Across-scale addition example for CS
    std::vector<cv::Mat> CS_vec;
    for (int i = 0; i < num_layers; ++i)
    {
        cv::Mat center = c_pyr_a.get(i);
        cv::Mat surround = s_pyr_a.get(i);
        
        cv::Mat CS = center - surround;
        cv::threshold(CS, CS, 0, 1, cv::THRESH_TOZERO);
        CS_vec.push_back(CS);
    }
    cv::Mat F = across_scale_addition(CS_vec);

    cv::namedWindow("Feature map", CV_WINDOW_AUTOSIZE);
    cv::imshow("Feature map", F );
    cv::waitKey(0);
    return 0;
}