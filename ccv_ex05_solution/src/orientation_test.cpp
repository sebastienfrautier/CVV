#include "gauss_pyr.h"
#include "laplacian_pyr.h"
#include "oriented_pyr.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

/* A note about function argument semantics in C++ 
 *
 * Consider four variants of a function that look similar: 
 * 1)   void f(MyClass m)
 * 2)   void f(const MyClass m)
 * 3)   void f(MyClass &m)
 * 4)   void f(const MyClass &m)
 *
 * The is a difference in how the parameter m is passed to the function.
 * In 1), the input m is copied using the copy constructor. Any changes
 * to m inside f are not visible at the call site.
 * In 2), the input m is copied using the copy constructor. Use of the 
 * keyword "const" forbids any changes to m inside f.
 * In 3), the input m is passed by reference - no copy is made, but a refe-
 * rence to the object m existing at the calls site is passed to f(). 
 * NB: Any changes to m will be visible at the call site! This can be useful
 * when passing heavy objects using a lot of memory to functions that modify
 * their inputs in some way.
 * In 4), the input m is also passed by reference. The keyword "const" forbids
 * any changes to m. This method is useful when passing read-only or input 
 * parameters to functions, especially if the object m is expensive to copy.
 * to change.
 *
 * Choosing the appropriate variant (1-4) has advantages: better performance if
 * copying is avoided, information to the user about the nature of the parameter
 * (input or output), guarantees about state of the object after the function call
 * (modified or not).
 *
 * Addendum: In class methods, you can also have the same 4 options, but possibly
 * with added const. If f were a method for some class, we have e.g. for (4):
 *   void f(const MyClass& m) const
 * The last keyword const here means that the method is not allowed to modify 
 * the state (member data) of the class.
 */

// Note we use (4) here: scale_images shall not be modified, 
// and we do not want to make a copy of it for use in the function.
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
            resized = scale_images[0];
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
    // Normalize to range [0, 1] and convert to CV_32F for calculations
    lab.convertTo(lab, CV_32F);
    lab /= 255.0f;

    // split the channels (L, a, b) to vector elements
    std::vector<cv::Mat> lab_channels;
    cv::split(lab, lab_channels);

    cv::namedWindow("Intensity", CV_WINDOW_NORMAL);
    cv::imshow("Intensity", lab_channels[0]);
    cv::waitKey(0);

    float gauss_pyr_sigma = 2.0;
    int num_layers = 3;
    gauss_pyr p(lab_channels[0], num_layers, gauss_pyr_sigma);

    for (int i = 0; i < p.number_of_layers(); ++i)
    {
        cv::Mat layer = p.get(i);
        // Normalize to range [0, 255] and convert to CV_8UC1 for display
        cv::normalize(layer, layer, 0, 255, cv::NORM_MINMAX, CV_8UC1);

        cv::namedWindow("Gaussian pyramid", CV_WINDOW_NORMAL);
        cv::imshow("Gaussian pyramid", layer);
        cv::waitKey(0);
    }

    float laplacian_pyr_sigma = 1.0;
    laplacian_pyr l(p, laplacian_pyr_sigma);

    for (int i = 0; i < l.number_of_layers(); ++i)
    {
        cv::Mat layer = l.get(i);
        // Normalize to range [0, 255] and convert to CV_8UC1 for display
        cv::normalize(layer, layer, 0, 255, cv::NORM_MINMAX, CV_8UC1);

        cv::namedWindow("Laplacian pyramid", CV_WINDOW_NORMAL);
        cv::imshow("Laplacian pyramid", layer);
        cv::waitKey(0);
    }

    int num_orientations = 4;
    oriented_pyr oriented(l, num_orientations);
    for (int o = 0; o < num_orientations; ++o)
    {
        for (int i = 0; i < num_layers; ++i)
        {
            cv::Mat layer = oriented.get(o, i);
            // Normalize to range [0, 255] and convert to CV_8UC1 for display
            cv::normalize(layer, layer, 0, 255, cv::NORM_MINMAX, CV_8UC1);

            cv::namedWindow("Oriented pyramid", CV_WINDOW_NORMAL);
            cv::imshow("Oriented pyramid", layer);
            cv::waitKey(0);
            
        }
    }


    // Finally, apply across-scale addition to all layers of each orientation
    for (int o = 0; o < num_orientations; ++o)
    {
        // get_orientation returns std::vector with cv::Mat's for all layers of orientation o
        cv::Mat result = across_scale_addition( oriented.get_orientation(o) );

        cv::namedWindow("Across-scale addition", CV_WINDOW_NORMAL);
        cv::imshow("Across-scale addition", result);
        cv::waitKey(0);
    }

    return 0;
}