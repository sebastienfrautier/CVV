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


        std::vector<cv::Mat> on_off_a(5);
        std::vector<cv::Mat> off_on_a(5);
        std::vector<cv::Mat> on_off_l(5);
        std::vector<cv::Mat> off_on_l(5);
        std::vector<cv::Mat> on_off_b(5);
        std::vector<cv::Mat> off_on_b(5);

        cv::cvtColor(input[i], lab[i], cv::COLOR_BGR2Lab);
        lab[i].convertTo(lab[i], CV_32F);
        std::vector<cv::Mat> lab_channels;
        cv::split(lab[i], lab_channels);

        float center_sigma = 2.0;
        float surround_sigma = 5.0;

        int num_layers = 5;
        gauss_pyr c_pyr_l(lab_channels[0], num_layers, center_sigma);
        gauss_pyr c_pyr_a(lab_channels[1], num_layers, center_sigma);
        gauss_pyr c_pyr_b(lab_channels[2], num_layers, center_sigma);
        gauss_pyr s_pyr_l(lab_channels[0], num_layers, surround_sigma);
        gauss_pyr s_pyr_a(lab_channels[1], num_layers, surround_sigma);
        gauss_pyr s_pyr_b(lab_channels[2], num_layers, surround_sigma);

        // Example: CS and SC contrasts, a channel (red-green vs. green-red)
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
        cv::Mat F_CS = across_scale_addition(CS_vec);


        std::vector<cv::Mat> SC_vec;
        for (int i = 0; i < num_layers; ++i)
        {
            cv::Mat center = c_pyr_a.get(i);
            cv::Mat surround = s_pyr_a.get(i);

            cv::Mat SC = surround - center;
            cv::threshold(SC, SC, 0, 1, cv::THRESH_TOZERO);
            SC_vec.push_back(SC);
        }
        cv::Mat F_SC = across_scale_addition(SC_vec);


    // V1
    //cv::Mat F_RESULT = F_CS - F_SC;

    // V2
    cv::Mat F_RESULT_a = (F_CS + F_SC)/2.0f;

    cv::normalize( F_RESULT_a,  F_RESULT_a, 0, 1, cv::NORM_MINMAX);

    //cv::imshow("Feature map", F_RESULT);
    //cv::waitKey(0);


    // Example: CS and SC contrasts, a channel (red-green vs. green-red)
        // Across-scale addition example for CS
        std::vector<cv::Mat> CS_vec_l;
        for (int i = 0; i < num_layers; ++i)
        {
            cv::Mat center = c_pyr_l.get(i);
            cv::Mat surround = s_pyr_l.get(i);

            cv::Mat CS_l = center - surround;
            cv::threshold(CS_l, CS_l, 0, 1, cv::THRESH_TOZERO);
            CS_vec_l.push_back(CS_l);
        }
        cv::Mat F_CS_l = across_scale_addition(CS_vec_l);




        std::vector<cv::Mat> SC_vec_l;
        for (int i = 0; i < num_layers; ++i)
        {

            cv::Mat center = c_pyr_l.get(i);
            cv::Mat surround = s_pyr_l.get(i);


            cv::Mat SC_l = surround - center;

            cv::threshold(SC_l, SC_l, 0, 1, cv::THRESH_TOZERO);

            SC_vec_l.push_back(SC_l);

        }


        cv::Mat F_SC_l = across_scale_addition(SC_vec_l);



    // V1
    //cv::Mat F_RESULT = F_CS - F_SC;

    // V2
    cv::Mat F_RESULT_l = (F_CS_l + F_SC_l)/2.0f;

    cv::normalize( F_RESULT_l,  F_RESULT_l, 0, 1, cv::NORM_MINMAX);



    // Example: CS and SC contrasts, a channel (red-green vs. green-red)
        // Across-scale addition example for CS
        std::vector<cv::Mat> CS_vec_b;
        for (int i = 0; i < num_layers; ++i)
        {
            cv::Mat center = c_pyr_b.get(i);
            cv::Mat surround = s_pyr_b.get(i);

            cv::Mat CS_b = center - surround;
            cv::threshold(CS_b, CS_b, 0, 1, cv::THRESH_TOZERO);
            CS_vec_b.push_back(CS_b);
        }
        cv::Mat F_CS_b = across_scale_addition(CS_vec_b);




        std::vector<cv::Mat> SC_vec_b;
        for (int i = 0; i < num_layers; ++i)
        {

            cv::Mat center = c_pyr_b.get(i);
            cv::Mat surround = s_pyr_b.get(i);


            cv::Mat SC_b = surround - center;

            cv::threshold(SC_b, SC_b, 0, 1, cv::THRESH_TOZERO);

            SC_vec_b.push_back(SC_b);

        }



        cv::Mat F_SC_b = across_scale_addition(SC_vec_b);
        //cv::imshow("Feature map", F_CS_l);
        //cv::waitKey(0);

    // V1
    //cv::Mat F_RESULT = F_CS - F_SC;

    // V2
    cv::Mat F_RESULT_b = (F_CS_b + F_SC_b)/2.0f;

    cv::normalize( F_RESULT_b,  F_RESULT_b, 0, 1, cv::NORM_MINMAX);



/*
    cv::imshow("L-channel", F_RESULT_l);
    cv::waitKey(0);
    cv::imshow("A-channel", F_RESULT_a);
    cv::waitKey(0);
    cv::imshow("B-channel", F_RESULT_b);
    cv::waitKey(0);

*/
    cv::Mat full = (F_RESULT_b + F_RESULT_l + F_RESULT_a)/3.0f;
    cv::imshow("FULL", full);
    cv::waitKey(10);



        cv::String file_name = fn[i];

        size_t lastindex = file_name.find_last_of(".");
        size_t lastindexslash = file_name.find_last_of("/");

        cv::String rawname = file_name.substr(lastindexslash+1, (lastindex-lastindexslash)-1);
        //cv::String b = "/Users/kbrusch/CVV/project/output/"+rawname+"_saliency.png";

        cv::String b = "/Users/kbrusch/Desktop/CVV/e7/method1/"+rawname+"_saliency.png";
        cv::imwrite( b, 255 *full);

        if (input[i].empty()) continue; //only proceed if sucsessful
        // you probably want to do some preprocessing
        data.push_back(input[i]);


    }
    return 0;
}
