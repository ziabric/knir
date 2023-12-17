#pragma once

#include <opencv2/opencv.hpp>
#include <filesystem>

void applyMeanFilter(const cv::Mat &inputImage, cv::Mat &outputImage, int kernelSize) {
    cv::blur(inputImage, outputImage, cv::Size(kernelSize, kernelSize));
}

static bool arithmetic_mean_filter(std::filesystem::path path, int kernal_value = 5)
{
    cv::Mat inputImage = cv::imread(path.string());
    if(inputImage.empty())
    {
        std::cout << "Ошибка загрузки изображения" << std::endl;
        return -1;
    }

    cv::Mat outputImage;

    applyMeanFilter(inputImage, outputImage, kernal_value);

    cv::imwrite(std::string( "OUTPUT_" + path.filename().string()), outputImage);

    return 0;
}