#include <opencv2/opencv.hpp>
#include "gauss_pyr.h"
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>


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
    std::vector<cv::Mat> input(10);
    std::vector<cv::Mat> lab(10);

    for ( int i = 0; i < 1; ++i)
    {
        std::stringstream ss;
        ss << i;
        std::string i_as_str = ss.str();

        //std::string off_on_filename = "0_0_" + i_as_str + ".jpg";
        std::string filename = "0_0_272.jpg";
        input[i] = cv::imread(filename.c_str(), CV_LOAD_IMAGE_COLOR);



        cv::cvtColor(input[i], lab[i], cv::COLOR_BGR2Lab);
        lab[i].convertTo(lab[i], CV_32F);
        std::vector<cv::Mat> lab_channels;
        cv::split(lab[i], lab_channels);

        float center_sigma = 10.0;
        float surround_sigma = 15.0;

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

            //cv::imshow("Center-surround contrast", CS );
            //cv::imshow("Surround-center contrast", SC );
            //cv::waitKey(0);
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

        cv::Mat E;

        int erosion_size = 20;
        cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS,
                      cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                      cv::Point(erosion_size, erosion_size) );

        erode(F, E , element);

        cv::namedWindow("Feature map", CV_WINDOW_AUTOSIZE);
        cv::imshow("Feature map", E );
        cv::waitKey(0);


    }

    return 0;
}