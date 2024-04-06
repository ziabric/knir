#include <algor.h>

algor::~algor()
{
    origData_.reset();
    modData_.reset();
}
void algor::setOrigImageSize(unsigned int width, unsigned int height)
{
    width_ = width;
    height_ = height;
    origData_ = std::make_shared<BGRValue[]>(width_ * height_);
}
bool algor::setOrigImagePixel(int column, int row, BGRValue pixel)
{
    if ( column >= width_ || row >= height_ ) return false;
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
    return origData_[column * width_ + row];
}
BGRValue algor::getModImagePixel(int column, int row) const
{
    return modData_[column * width_ + row];
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
    origData_.reset();
    modData_.reset();
    width_ = 0;
    height_ = 0;
}
void algor::medianFilter(int radius)
{
    modData_ = std::make_shared<BGRValue[]>(width_ * height_);

    int windowSize = (2 * radius + 1) * (2 * radius + 1);

    unsigned int redValues[windowSize];
    unsigned int greenValues[windowSize];
    unsigned int blueValues[windowSize];

    for (int i = radius; i < width_ - radius; i+=1) 
    {
        for (int j = radius; j < height_ - radius; j+=1) 
        {
            int index = 0;

            for (int x = -radius; x <= radius; x+=1) 
            {
                for (int y = -radius; y <= radius; y+=1) 
                {
                    redValues[index] = origData_[(i + x) * width_ + (j + y)].r;
                    greenValues[index] = origData_[(i + x) * width_ + (j + y)].g;
                    blueValues[index] = origData_[(i + x) * width_ + (j + y)].b;
                    index += 1;
                }
            }

            modData_[i * width_ + j].r = calculateMedian(redValues, windowSize);
            modData_[i * width_ + j].g = calculateMedian(greenValues, windowSize);
            modData_[i * width_ + j].b = calculateMedian(blueValues, windowSize);
        }
    }
}
void algor::newMedianFilter(int radius)
{
    modData_ = std::make_shared<BGRValue[]>(width_ * height_);

    int windowSize = (2 * radius + 1) * (2 * radius + 1);

    unsigned int redValues[windowSize];
    unsigned int greenValues[windowSize];
    unsigned int blueValues[windowSize];

    for (int i = radius; i < width_ - radius; i+=1) 
    {
        for (int j = radius; j < height_ - radius; j+=1) 
        {
            int index = 0;

            for (int x = -radius; x <= radius; x+=1) 
            {
                for (int y = -radius; y <= radius; y+=1) 
                {
                    redValues[index] = origData_[(i + x) * width_ + (j + y)].r;
                    greenValues[index] = origData_[(i + x) * width_ + (j + y)].g;
                    blueValues[index] = origData_[(i + x) * width_ + (j + y)].b;
                    index += 1;
                }
            }
            modData_[i * width_ + j].r = calculateNewMedian(redValues, windowSize);
            modData_[i * width_ + j].g = calculateNewMedian(greenValues, windowSize);
            modData_[i * width_ + j].b = calculateNewMedian(blueValues, windowSize);
        }
    }
}
int algor::min(int a, int b)
{
    return (a < b) ? a : b;
}
int algor::max(int a, int b)
{
    return (a > b) ? a : b;
}
void algor::bubbleSort(unsigned int* list, int listLength)
{
	while(listLength--)
	{
		bool swapped = false;
		
		for(int i = 0; i < listLength; i++)
		{
			if(list[i] > list[i + 1])
			{
				swap(list[i], list[i + 1]);
				swapped = true;
			}
		}
		
		if(swapped == false)
			break;
	}
}
int algor::partition(unsigned int* arr, int low, int high) 
{
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j+=1) 
    {
        if (arr[j] <= pivot) 
        {
            i+=1;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}
void algor::quickSort(unsigned int* arr, int low, int high) 
{
    if (low < high) 
    {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
unsigned int algor::calculateMedian(unsigned int* arr, int size)
{
    quickSort(arr, 0, size-1);
    return arr[size / 2];
}
unsigned int algor::calculateNewMedian(unsigned int* arr, int size)
{
    quickSort(arr, 0, size - 1);
    unsigned int value = arr[0], prevValue = arr[0];
    int valueCount = 1, prevValueCount = 1;
    for (int i = 1; i < size; i += 1)
    {
        if ( value != arr[i] )
        {
            if ( valueCount > prevValueCount )
            {
                prevValueCount = valueCount;
                prevValue = value;
            }
            value = arr[i];
            valueCount = 1;
        }
        else
        {
            valueCount += 1;
        }
    }

    return prevValue;
}
void algor::swap(unsigned int &a, unsigned int &b)
{
    unsigned int tmp = a;
    a = b;
    b = tmp;
}
double algor::sqrt(double x)
{
    if (x < 0) return -1;
    double guess = 1.0;
    while (abs(guess * guess - x) > 1e-10) guess = (guess + x / guess) / 2.0;
    return guess;
}
double algor::abs(double x)
{
    return (x > 0) ? x : -x;
}
void algor::haar()
{
    modData_ = std::make_shared<BGRValue[]>(width_ * height_);
    unsigned int c = 0;
    unsigned int d = 0;

    for ( int i = 0; i < width_-1; i += 2 )
    {
        for ( int j = 0; j < height_-1; j += 2 )
        {
            // For blue channel
            c = ( getOrigImagePixel( i , j ).b + getOrigImagePixel( i , (j)+1 ).b + getOrigImagePixel( (i)+1 , j ).b + getOrigImagePixel( (i)+1 , (j)+1 ).b ) / 2;
            d = ( getOrigImagePixel( i , j ).b + getOrigImagePixel( i , (j)+1 ).b - getOrigImagePixel( (i)+1 , j ).b + getOrigImagePixel( (i)+1 , (j)+1 ).b ) / 2;
            modData_[(i) * width_ + (j)].b = c + d;
            modData_[(i) * width_ + (j+1)].b = c - d;
            modData_[(i+1) * width_ + (j)].b = c - d;
            modData_[(i+1) * width_ + (j+1)].b = c + d;

            // For green channel
            c = ( getOrigImagePixel( i , j ).g + getOrigImagePixel( i , (j)+1 ).g + getOrigImagePixel( (i)+1 , j ).g + getOrigImagePixel( (i)+1 , (j)+1 ).g ) / 2;
            d = ( getOrigImagePixel( i , j ).g + getOrigImagePixel( i , (j)+1 ).g - getOrigImagePixel( (i)+1 , j ).g + getOrigImagePixel( (i)+1 , (j)+1 ).g ) / 2;
            modData_[(i) * width_ + (j)].g = c + d;
            modData_[(i) * width_ + (j+1)].g = c - d;
            modData_[(i+1) * width_ + (j)].g = c - d;
            modData_[(i+1) * width_ + (j+1)].g = c + d;

            // For red channel
            c = ( getOrigImagePixel( i , j ).r + getOrigImagePixel( i , (j)+1 ).r + getOrigImagePixel( (i)+1 , j ).r + getOrigImagePixel( (i)+1 , (j)+1 ).r ) / 2;
            d = ( getOrigImagePixel( i , j ).r + getOrigImagePixel( i , (j)+1 ).r - getOrigImagePixel( (i)+1 , j ).r + getOrigImagePixel( (i)+1 , (j)+1 ).r ) / 2;
            modData_[(i) * width_ + (j)].r = c + d;
            modData_[(i) * width_ + (j+1)].r = c - d;
            modData_[(i+1) * width_ + (j)].r = c - d;
            modData_[(i+1) * width_ + (j+1)].r = c + d;
        }
    }
}
double algor::gaussian(double x, double y, double sigma)
{
    return power(e, (-(x * x + y * y) / (2 * sigma * sigma)));
}
double algor::power(double base, int exponent) 
{
    if (exponent == 0)
        return 1;
    if (exponent == 1)
        return base;

    double result = 1;
    bool isNegativeExponent = exponent < 0;
    exponent = abs(exponent);
    
    for (int i = 0; i < exponent; i+=1) 
    {
        result *= base;
    }

    return isNegativeExponent ? 1 / result : result;
}
void algor::bilateralFilter(int kernelSize, double spatialSigma, double intensitySigma) 
{
    modData_ = std::make_shared<BGRValue[]>(width_ * height_);
    
    for (int y = 0; y < height_; y+=1) {
        for (int x = 0; x < width_; x+=1) {
            double weightSum_b = 0, weightSum_g = 0, weightSum_r = 0;
            double filteredValue_b = 0, filteredValue_g = 0, filteredValue_r = 0;

            for (int j = -kernelSize / 2; j <= kernelSize / 2; j+=1) 
            {
                for (int i = -kernelSize / 2; i <= kernelSize / 2; i+=1) 
                {
                    int neighborX = x + i;
                    int neighborY = y + j;

                    if (neighborX >= 0 && neighborX < width_ && neighborY >= 0 && neighborY < height_) 
                    {
                        double intensityDiff_b = origData_[neighborX * width_ + neighborY].b - origData_[x * width_ + y].b;
                        double weight_b = gaussian(i, j, spatialSigma) * gaussian(intensityDiff_b, 0, intensitySigma);
                        filteredValue_b += origData_[neighborX * width_ + neighborY].b * weight_b;
                        weightSum_b += weight_b;

                        double intensityDiff_g = origData_[neighborX * width_ + neighborY].g - origData_[x * width_ + y].g;
                        double weight_g = gaussian(i, j, spatialSigma) * gaussian(intensityDiff_g, 0, intensitySigma);
                        filteredValue_g += origData_[neighborX * width_ + neighborY].g * weight_g;
                        weightSum_g += weight_g;

                        double intensityDiff_r = origData_[neighborX * width_ + neighborY].r - origData_[x * width_ + y].r;
                        double weight_r = gaussian(i, j, spatialSigma) * gaussian(intensityDiff_r, 0, intensitySigma);
                        filteredValue_r += origData_[neighborX * width_ + neighborY].r * weight_r;
                        weightSum_r += weight_r;
                    }
                }
            }

            modData_[x*width_ + y].b = (int)(filteredValue_b / weightSum_b);
            modData_[x*width_ + y].g = (int)(filteredValue_g / weightSum_g);
            modData_[x*width_ + y].r = (int)(filteredValue_r / weightSum_r);
        }
    }
}