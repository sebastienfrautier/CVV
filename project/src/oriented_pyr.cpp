#include "oriented_pyr.h"
#include <opencv2/opencv.hpp>

oriented_pyr::oriented_pyr(const laplacian_pyr& p, int num_orientations)
	: orientation_maps_(num_orientations, std::vector<cv::Mat>(p.number_of_layers()) )
	// Initialize as vector of num_orientations elements, 
	// each element is a vector of cv::Mat with element number equal 
	// to number of layers in p
{
	// these could also be made input arguments to allow user to tune the Gabor filters
	int filter_size = 25;
    double sigma = 10.0;
    double lambd = 10.0;
    double gamma = 1.0;
    double psi = 0.0;
	for (int o = 0; o < num_orientations; ++o)
	{
		double theta = (o * CV_PI) / num_orientations;
		cv::Mat gabor_kernel = cv::getGaborKernel( cv::Size(filter_size,filter_size), sigma, theta, lambd, gamma, psi, CV_32F );
		// Filter every layer of p with the kernel
		for ( int i = 0; i < p.number_of_layers(); ++i)
		{
			cv::filter2D(p.get(i), orientation_maps_[o][i], -1, gabor_kernel);
		}
	}
}

cv::Mat oriented_pyr::get(int orientation, int layer) const
{
	// returns the requested orientation in requested layer
	return orientation_maps_[orientation][layer];

}

std::vector<cv::Mat> oriented_pyr::get_orientation(int orientation) const
{
	// return the vector containing every layer of requested orientation
	return orientation_maps_[orientation];
}