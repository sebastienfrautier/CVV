#include <opencv2/opencv.hpp>
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
            resized = scale_images[i];
        }

        result += resized;
    }
    return result;
}

int main(int argc, char** argv )
{
    std::vector<cv::Mat> on_off(10);
    std::vector<cv::Mat> off_on(10);
    for ( int i = 0; i < 10; ++i)
    {
        std::stringstream ss;
        ss << i;
        std::string i_as_str = ss.str();

        std::string off_on_filename = "off_on_L_" + i_as_str + ".png";
        off_on[i] = cv::imread(off_on_filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

        std::string on_off_filename = "on_off_L_" + i_as_str + ".png";
        on_off[i] = cv::imread(on_off_filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);

        // Remove the block comment if you want to visualize the inputs
        /*        
        cv::namedWindow(off_on_filename.c_str(), CV_WINDOW_NORMAL);
        cv::imshow(off_on_filename.c_str(), off_on[i]);
        cv::namedWindow(on_off_filename.c_str(), CV_WINDOW_NORMAL);
        cv::imshow(on_off_filename.c_str(), on_off[i]);

        cv::waitKey(0);
        cv::destroyWindow(off_on_filename.c_str());
        cv::destroyWindow(on_off_filename.c_str());
        */
    }

    // Important: before addition, convert to floating point.
    // Otherwise, numerical overflow will occur and information will be lost.
    for ( int i = 0; i < 10; ++i)
    {
        on_off[i].convertTo(on_off[i], CV_32F);
        off_on[i].convertTo(off_on[i], CV_32F);
    }

    cv::Mat F_on_off = across_scale_addition(on_off);
    cv::Mat F_off_on = across_scale_addition(off_on);


    // For display, normalize to range (0,1) -- don't do this for calculations!
    cv::Mat display_only;
    cv::normalize(F_on_off, display_only, 0, 1, cv::NORM_MINMAX);
    cv::namedWindow("F_on_off", CV_WINDOW_NORMAL);
    cv::imshow("F_on_off", display_only);

    cv::normalize(F_off_on, display_only, 0, 1, cv::NORM_MINMAX);
    cv::namedWindow("F_off_on", CV_WINDOW_NORMAL);
    cv::imshow("F_off_on", display_only);
    cv::waitKey(0);

    // normalization factor to use
    double on_off_min, on_off_max, off_on_min, off_on_max;
    cv::minMaxLoc(F_on_off, &on_off_min, &on_off_max);
    cv::minMaxLoc(F_off_on, &off_on_min, &off_on_max);
    double desired_max = std::max( on_off_max, off_on_max);
    std::cout << "desired max " << desired_max << std::endl;

    // arithmetic mean fusion
    cv::Mat am = (F_on_off + F_off_on) / 2.0f;
    cv::normalize(am, am, 0.0f, (float)desired_max, cv::NORM_MINMAX);

    // For display, normalize to range (0,1) -- don't do this for calculations!
    cv::normalize(am, display_only, 0, 1, cv::NORM_MINMAX);
    cv::namedWindow("C_L arithmetic mean", CV_WINDOW_NORMAL);
    cv::imshow("C_L arithmetic mean", display_only);
    cv::waitKey(0);

    // max fusion
    cv::Mat fmax = cv::max( F_on_off, F_off_on);
    cv::normalize(fmax, fmax, 0.0f, (float)desired_max, cv::NORM_MINMAX);

    // For display, normalize to range (0,1) -- don't do this for calculations!
    cv::normalize(fmax, display_only, 0, 1, cv::NORM_MINMAX);
    cv::namedWindow("C_L max fusion", CV_WINDOW_NORMAL);
    cv::imshow("C_L max fusion", display_only);
    cv::waitKey(0);

    return 0;
}