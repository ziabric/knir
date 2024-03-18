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
void algor::medianFilter(int radius)
{
    if (modData_ != nullptr) delete[] modData_;
    modData_ = new BGRValue[width_ * height_];

    unsigned int* medianArrayB = new unsigned int[radius * radius];
    unsigned int* medianArrayG = new unsigned int[radius * radius];
    unsigned int* medianArrayR = new unsigned int[radius * radius];
    int medianArrayCount = 0;

    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;

    for (int x = 1; x < width_ - 1; x += 1)
    {
        for (int y = 1; y < height_ - 1; y += 1)
        {
            int minX = max(0, x - radius);
            int maxX = min(width_ - 1, x + radius);
            int minY = max(0, y - radius);
            int maxY = min(height_ - 1, y + radius);

            for (int i = minY; i <= maxX; i+=1) 
            {
                for (int j = minX; j <= maxY; j+=1) 
                {
                    medianArrayB[medianArrayCount] = origData_[i*width_ + j].b;
                    medianArrayG[medianArrayCount] = origData_[i*width_ + j].g;
                    medianArrayR[medianArrayCount] = origData_[i*width_ + j].r;
                }
            }
            quickSort(medianArrayB, 0, (radius * radius) - 1);
            quickSort(medianArrayG, 0, (radius * radius) - 1);
            quickSort(medianArrayR, 0, (radius * radius) - 1);

            modData_[x*width_ + y].b = medianArrayB[medianArrayCount/2];
            modData_[x*width_ + y].g = medianArrayG[medianArrayCount/2];
            modData_[x*width_ + y].r = medianArrayR[medianArrayCount/2];
        }
    }
    delete[] medianArrayB;
    delete[] medianArrayG;
    delete[] medianArrayR;
}
int algor::min(int a, int b)
{
    return (a < b) ? a : b;
}
int algor::max(int a, int b)
{
    return (a > b) ? a : b;
}
void algor::quickSort(unsigned int* array, unsigned int left, unsigned int right) 
{
    if (left >= right) 
    {
        return;
    }
    unsigned int pivot = array[left];
    unsigned int i = left + 1;
    unsigned int j = right;
    while (i <= j) 
    {
        while (array[i] < pivot) 
        {
            i+=1;
        }
        while (array[j] >= pivot) 
        {
            j-=1;
        }
        if (i <= j) 
        {
            swap(array[i], array[j]);
        }
    }
    quickSort(array, left, j - 1);
    quickSort(array, i, right);
}

void algor::swap(unsigned int &a, unsigned int &b)
{
    unsigned int tmp = a;
    a = b;
    b = tmp;
}