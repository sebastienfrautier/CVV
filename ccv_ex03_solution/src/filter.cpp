#include <opencv2/opencv.hpp>
#include <iostream>

// Important: you *must* call the executable with an image argument, e.g.
//  ./ccv_ex03_filter image.jpg
// otherwise the software will crash trying to apply filtering for task 2.

int main(int argc, char** argv )
{
    // 1
    float Mdata[3][3] = {{1.0, 1.0, 1.0},
                         {1.0, 2.0, 1.0},
                         {1.0, 1.0, 1.0}};
    cv::Mat M(3, 3, CV_32F, &Mdata);
    std::cout << "M = " << M << std::endl;

    float Gdata[3][3] = {{1, 2, 1},
                         {2, 4, 2},
                         {1, 2, 1}};
    cv::Mat G(3, 3, CV_32F, &Gdata);
    G /= 16.0f;
    std::cout << "G = " << G << std::endl;

    cv::Mat result1(3,3, CV_32F);
    cv::filter2D(M, result1, -1, G, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
    std::cout << "result1 = " << result1 << std::endl;

    cv::Mat g1(1, 3, CV_32F);
    g1.at<float>(0) = 1.0f/4.0f;
    g1.at<float>(1) = 2.0f/4.0f;
    g1.at<float>(2) = 1.0f/4.0f;
    cv::Mat g2(3, 1, CV_32F);
    g2.at<float>(0) = 1.0f/4.0f;
    g2.at<float>(1) = 2.0f/4.0f;
    g2.at<float>(2) = 1.0f/4.0f;
    std::cout << "g1 = " << g1 << "\ng2 = " << g2 << std::endl;

    cv::Mat result2(3,3, CV_32F);
    cv::filter2D(M, result2, -1, g1, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
    cv::filter2D(result2, result2, -1, g2, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
    std::cout << "result2 = " << result2 << std::endl;

    cv::Mat result3(3,3,CV_32F);
    cv::GaussianBlur(M, result3, cv::Size(3,3), 0, 0, cv::BORDER_CONSTANT);
    std::cout << "result3 = " << result3 << std::endl;


    // 2.
    cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cv::Mat blurred(image.rows, image.cols, image.depth());
    cv::namedWindow("Blurred image", CV_WINDOW_AUTOSIZE);
    for (int k = 3; k < 26; k+=2)
    {
        std::cout << "k = " << k << std::endl;
        cv::GaussianBlur(image, blurred, cv::Size(k,k), 0, 0, cv::BORDER_CONSTANT);
        cv::imshow("Blurred image", blurred);
        int c = cv::waitKey(500);
    }

    // DoG
    cv::Mat im_gray;
    cv::cvtColor(image, im_gray, cv::COLOR_BGR2GRAY);
    cv::Mat im_gray_float;
    im_gray.convertTo(im_gray_float, CV_32F);
    cv::Mat I1, I2;
    cv::GaussianBlur(im_gray_float, I1, cv::Size(5,5), 1.0, 0, cv::BORDER_CONSTANT);
    cv::GaussianBlur(im_gray_float, I2, cv::Size(5,5), 2.0, 0, cv::BORDER_CONSTANT);
    cv::Mat DoG = I1 - I2;
    cv::namedWindow("Difference of Gaussians", CV_WINDOW_AUTOSIZE);
    cv::imshow("Difference of Gaussians", DoG);
    cv::waitKey(0);
    return 0;
}