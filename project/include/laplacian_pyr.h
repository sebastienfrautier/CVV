#ifndef LAPLACIAN_PYR_H
#define LAPLACIAN_PYR_H
#include "gauss_pyr.h"
#include <opencv2/opencv.hpp>

class laplacian_pyr
{
public:
	laplacian_pyr(const gauss_pyr& p, float sigma);
	cv::Mat get(int layer) const;
	int number_of_layers() const;
private:
	std::vector<cv::Mat> layers_;
};


#endif