#include <algor.h>

algor::~algor()
{
    delete[] origData_;
    delete[] modData_;
}
void algor::setOrigImageSize(unsigned int width, unsigned int height)
{
    width_ = width;
    height_ = height;
    origData_ = new BGRValue[width_ * height_];
}
bool algor::setOrigImagePixel(int column, int row, const BGRValue &pixel)
{
    if ( column > width_ || row > height_ ) return false;
    else 
    {
        origData_[column * width_ + row] = pixel;
        return true;
    }
}
int algor::getImageWidth() const
{
    return width_;
}
int algor::getImageHeight() const
{
    return height_;
}
BGRValue algor::getOrigImagePixel(int column, int row) const
{
    return {0,0,0}; 
}
BGRValue algor::getModImagePixel(int column, int row) const
{
    return {0,0,0};
}
bool algor::origImageAvailable() const
{
    return (origData_ != nullptr) ? true : false;
}
bool algor::modImageAvailable() const
{
    return (modData_ != nullptr) ? true : false;
}
void algor::clearData()
{
    delete[] origData_;
    delete[] modData_;
    origData_ = nullptr;
    modData_ = nullptr;
}