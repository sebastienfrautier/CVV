#ifndef ORIENTED_PYR_H
#define ORIENTED_PYR_H
#include "laplacian_pyr.h"
#include <opencv2/opencv.hpp>

class oriented_pyr
{
public:
	oriented_pyr(const laplacian_pyr& p, int num_orientations);
	cv::Mat get(int orientation, int layer) const;
	std::vector<cv::Mat> get_orientation(int orientation) const;

private:
	std::vector<std::vector<cv::Mat> > orientation_maps_;
};


#endif