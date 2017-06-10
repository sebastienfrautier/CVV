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

    std::vector<cv::Mat> input(1001);
    std::vector<cv::Mat> lab(1001);


    cv::String path("/Users/kbrusch/CVV/input/*.jpg"); //select only jpg
    cv::vector<cv::String> fn;
    cv::vector<cv::Mat> data;
    cv::glob(path,fn,true); // recurse
    for (size_t i=0; i<fn.size(); ++i)
    {
        input[i] = cv::imread(fn[i]);
        std::stringstream ss;
        ss << i;
        std::string i_as_str = ss.str();


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

        cv::erode(F, E , element);

        cv::String file_name = fn[i];

        size_t lastindex = file_name.find_last_of(".");
        size_t lastindexslash = file_name.find_last_of("/");

        cv::String rawname = file_name.substr(lastindexslash+1, (lastindex-lastindexslash)-1);

        cv::namedWindow(rawname, CV_WINDOW_AUTOSIZE);


        //cv::COLOR_Lab2BGR
        //cv::Mat output;
        //cv::cvtColor(E, output, 56);

        //cv::cvtColor(E, output, cv::COLOR_Lab2RGB);
        //lab[i].convertTo(lab[i], CV_32F);



        cv::imwrite( "/Users/kbrusch/CVV/project/output/"+rawname+".png", E);

        cv::imshow("Feature map", E);
        cv::waitKey(10);

        if (input[i].empty()) continue; //only proceed if sucsessful
        // you probably want to do some preprocessing
        data.push_back(input[i]);


    }
    return 0;
}
