#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

// Important: you *must* call the executable with an image argument, e.g.
//  ./ccv_ex03_filter image.jpg
// otherwise the software will crash trying to apply filtering for task 2.

class gauss_pyr
{
public:
    gauss_pyr(cv::Mat& img, int number_of_layers)
    {
        cv::Mat dst = img.clone(); //deep copy to not modify original image
        for (int i = 0; i < number_of_layers; ++i)
        {
            cv::GaussianBlur(dst, dst, cv::Size(5,5), 0, 0, cv::BORDER_CONSTANT);
            layers_.push_back( dst.clone() ); // remember to deep copy!
            std::cout << "layer " << i << " size " << layers_.back().size() << std::endl;
            cv::resize(dst, dst, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
        }
        std::cout << "Pyramid has " << layers_.size() << " layers." << std::endl;
    }

    cv::Mat get(int layer)
    {
        return layers_[layer];
    }

private:
    std::vector<cv::Mat> layers_;
};

int main(int argc, char** argv )
{
    // 1.
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

    // 2. 
    cv::Mat lab;
    cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);
    // Convert the image to float, and scale to range [0, 1]
    lab.convertTo(lab, CV_32FC3);
    lab /= 255.0f;

    // 3.
    std::vector<cv::Mat> lab_channels;
    cv::split(lab, lab_channels);

    // 4. - 6.
    int num_layers = 3;
    gauss_pyr pyr_l(lab_channels[0], num_layers);
    gauss_pyr pyr_a(lab_channels[1], num_layers);
    gauss_pyr pyr_b(lab_channels[2], num_layers);

    // 7.
    cv::namedWindow("Gaussian pyramid", CV_WINDOW_AUTOSIZE);

    // L pyramid
    for (int l = 0; l < num_layers; ++l)
    {
        cv::imshow("Gaussian pyramid", pyr_l.get(l) );
        std::cout << "Displaying layer " << l << " of L channel pyramid" << std::endl;
        int c = cv::waitKey(0);
    }

    // a pyramid
    for (int l = 0; l < num_layers; ++l)
    {
        cv::imshow("Gaussian pyramid", pyr_a.get(l) );
        std::cout << "Displaying layer " << l << " of a channel pyramid" << std::endl;
        int c = cv::waitKey(0);
    }

    // b pyramid
    for (int l = 0; l < num_layers; ++l)
    {
        cv::imshow("Gaussian pyramid", pyr_b.get(l) );
        std::cout << "Displaying layer " << l << " of b channel pyramid" << std::endl;
        int c = cv::waitKey(0);
    }

    return 0;
}