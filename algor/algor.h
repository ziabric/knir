#ifndef ALGOR_H
#define ALGOR_H

#include <memory>
#include <cmath>

/**
 * @brief Storing pixel values
 */
struct BGRValue 
{
    unsigned int b = 0;
    unsigned int g = 0;
    unsigned int r = 0;
};

struct BGRValued 
{
    double b = 0;
    double g = 0;
    double r = 0;
};

// 1 -- columns -> rows

/**
 * @brief Implementation of image noise reduction algorithms
 * 
 */
class algor
{
public:
    /// @brief Constructor
    algor() = default;
    /// @brief Destructor
    ~algor();
    /// @brief Creating an original image with default pixel values
    /// @param width Image width in pixels
    /// @param height Image height in pixels
    void setOrigImageSize(unsigned int width, unsigned int height);
    /// @brief Changing the pixel value in the original image
    /// @param column Pixel width address
    /// @param row Pixel height address
    /// @param pixel Pixel color value in RGB format
    /// @return Error value ( true - there is no error, false - errors when recording a pixel )
    bool setOrigImagePixel(int column, int row, BGRValue pixel);
    /// @brief Getting the width value of the original image
    /// @return The width of the original image
    int getImageWidth() const;
    /// @brief Getting the height value of the original image
    /// @return The height of the original image
    int getImageHeight() const;
    /// @brief Getting the pixel value
    /// @param row Pixel width address
    /// @param column Pixel height address
    /// @return Pixel value in RGB format
    BGRValue getOrigImagePixel(int row, int column) const;
    /// @brief Getting the pixel value in the modified image
    /// @param row ixel width address
    /// @param column Pixel height address
    /// @return Pixel value in RGB format
    BGRValue getModImagePixel(int row, int column) const;
    /// @brief The ability to link to the original image
    /// @return Error flag ( true -- the image exists, false -- the image does not exist)
    bool origImageAvailable() const;
    /// @brief The ability to link to a modified image
    /// @return Error flag ( true -- the image exists, false -- the image does not exist)
    bool modImageAvailable() const;
    /// @brief Cleaning images
    void clearData(); 
    /// @brief Implementation of the median filter
    /// @param radius The radius of the matrix for the median value
    void medianFilter(int radius);
    /// @brief Implementation of the median filter
    /// @param radius The radius of the matrix for the median value
    void newMedianFilter(int radius);
    /// @brief 
    void haar();
    /// @brief 
    /// @param kernelSize 
    /// @param spatialSigma 
    /// @param intensitySigma 
    void bilateralFilter(int kernelSize, double spatialSigma, double intensitySigma);
    /// @brief 
    /// @param kernelSize 
    /// @param spatialSigma 
    /// @param rangeSigma 
    void bilateralFilterRange(int kernelSize, double spatialSigma, double rangeSigma);

    BGRValued getMSE();

    BGRValued getPSNR();

    BGRValued getSSIM(int radius, int c1, int c2);
protected:
    /// @brief Search median value in array
    /// @param arr Array of median values
    /// @param size Array size
    /// @return Median value
    unsigned int calculateMedian(unsigned int* arr, int size);
    /// @brief 
    /// @param arr 
    /// @param size 
    /// @return 
    unsigned int calculateNewMedian(unsigned int* arr, int size);
    /// @brief Implementation of the change of two values
    /// @param a First value
    /// @param b Second value
    void swap(unsigned int &a, unsigned int &b);
    /// @brief Get minimum value
    /// @param a First value
    /// @param b Second value
    /// @return Minimum value between a and b
    int min(int a, int b);  
    /// @brief Get maximum value
    /// @param a First value
    /// @param b Second value
    /// @return Maximum value between a and b
    int max(int a, int b);
    /// @brief Taking the sqrt of a number based on the Newton-Raphson algorithm
    /// @param x input value
    /// @return sqrt value
    double sqrt(double x);
    /// @brief 
    /// @param x 
    /// @return 
    double abs(double x);
    /// @brief 
    /// @param list 
    /// @param listLength 
    void bubbleSort(unsigned int* list, int listLength);
    
    int partition(unsigned int* arr, int low, int high);
    
    void quickSort(unsigned int* arr, int low, int high);

    double gaussianBase(double x, double y, double sigma);

    double gaussianRange(double x, double sigma);

    double power(double base, int exponent);

    BGRValued average(BGRValue* arr, int size);

    BGRValued variance(BGRValue* arr, int size, BGRValued mean);

    BGRValued covariance(BGRValue* arr1,  BGRValue* arr2, int size, BGRValued mean1, BGRValued mean2);

private:
    /// @brief Image Height
    unsigned int height_;
    /// @brief Image Width
    unsigned int width_;
    /// @brief Pointer to the pixel values of the original image
    std::shared_ptr<BGRValue[]> origData_ = nullptr;
    /// @brief Pointer to the pixel values of the modified image
    std::shared_ptr<BGRValue[]> modData_ = nullptr;
    /// @brief exp
    float e = 2.718281828459045;
};

#endif