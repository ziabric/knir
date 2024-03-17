#ifndef ALGOR_H
#define ALGOR_H

/**
 * @brief Storing pixel values
 */
struct BGRValue 
{
    unsigned int b = 0;
    unsigned int g = 0;
    unsigned int r = 0;
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
    bool setOrigImagePixel(int column, int row, const BGRValue &pixel);
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
private:
    /// @brief Image Height
    unsigned int height_;
    /// @brief Image Width
    unsigned int width_;
    /// @brief Pointer to the pixel values of the original image
    BGRValue * origData_ = nullptr;
    /// @brief Pointer to the pixel values of the modified image
    BGRValue * modData_ = nullptr;
};

#endif