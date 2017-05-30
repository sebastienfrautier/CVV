#include "laplacian_pyr.h"
#include <opencv2/opencv.hpp>

laplacian_pyr::laplacian_pyr(const gauss_pyr& p, float sigma)
	: layers_(p.number_of_layers())
	// above: initialize the vector to have same number of elements as layers in p.
{
	for (int i = 0; i < layers_.size(); ++i)
    {
    	// to create i'th layer: get i'th layer of p, and blur it; subtract.
    	cv::Mat blurred;
        cv::GaussianBlur(p.get(i), blurred, cv::Size(), sigma, sigma, cv::BORDER_REPLICATE);
        layers_[i] = p.get(i) - blurred;
    }
}

cv::Mat laplacian_pyr::get(int layer) const
{
	return layers_[layer];
}

int laplacian_pyr::number_of_layers() const
{
	return layers_.size();
}