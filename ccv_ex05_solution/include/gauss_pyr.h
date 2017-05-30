#ifndef GAUSS_PYR_H
#define GAUSS_PYR_H
#include <vector>
#include <opencv2/opencv.hpp>

class gauss_pyr
{
public:
    gauss_pyr(cv::Mat& img, int number_of_layers, float sigma);
    cv::Mat get(int layer) const;
    int number_of_layers() const;

private:
    std::vector<cv::Mat> layers_;
};

#endif