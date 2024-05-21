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
double algor::gaussianBase(double x, double y, double sigma)
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
                        double weight_b = gaussianBase(i, j, spatialSigma) * gaussianBase(intensityDiff_b, 0, intensitySigma);
                        filteredValue_b += origData_[neighborX * width_ + neighborY].b * weight_b;
                        weightSum_b += weight_b;

                        double intensityDiff_g = origData_[neighborX * width_ + neighborY].g - origData_[x * width_ + y].g;
                        double weight_g = gaussianBase(i, j, spatialSigma) * gaussianBase(intensityDiff_g, 0, intensitySigma);
                        filteredValue_g += origData_[neighborX * width_ + neighborY].g * weight_g;
                        weightSum_g += weight_g;

                        double intensityDiff_r = origData_[neighborX * width_ + neighborY].r - origData_[x * width_ + y].r;
                        double weight_r = gaussianBase(i, j, spatialSigma) * gaussianBase(intensityDiff_r, 0, intensitySigma);
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

void algor::bilateralFilter_sigma(int kernelSize, double spatialSigma, double intensitySigmaAlpha, double intensitySigmaBetta) 
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
                        // double weight_b = koshi_kernal(i, j, spatialSigma) * gaussianBase(intensityDiff_b, 0, intensitySigmaAlpha);
                        double weight_b = koshi_kernal(i, j, spatialSigma) * koshi_kernal(intensityDiff_b, 0, intensitySigmaAlpha);
                        filteredValue_b += origData_[neighborX * width_ + neighborY].b * weight_b;
                        weightSum_b += weight_b;

                        double intensityDiff_g = origData_[neighborX * width_ + neighborY].g - origData_[x * width_ + y].g;
                        // double weight_g = koshi_kernal(i, j, spatialSigma) * gaussianBase(intensityDiff_g, 0, intensitySigmaAlpha);
                        double weight_g = koshi_kernal(i, j, spatialSigma) * koshi_kernal(intensityDiff_g, 0, intensitySigmaAlpha);
                        filteredValue_g += origData_[neighborX * width_ + neighborY].g * weight_g;
                        weightSum_g += weight_g;

                        double intensityDiff_r = origData_[neighborX * width_ + neighborY].r - origData_[x * width_ + y].r;
                        // double weight_r = koshi_kernal(i, j, spatialSigma) * gaussianBase(intensityDiff_r, 0, intensitySigmaAlpha);
                        double weight_r = koshi_kernal(i, j, spatialSigma) * koshi_kernal(intensityDiff_r, 0, intensitySigmaAlpha);
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

double algor::gaussianRange(double x, double sigma) 
{
    return power(e, (-(x * x) / (2 * sigma * sigma)));
}

double algor::koshi_kernal(int x, int y, double sigma)
{
    return ( 1 / ( 1 + (x*x + y*y) / (sigma * sigma) ) );
}

double algor::sigma_kernal(double x, double alpha, double beta)
{
    return 1.0 / (1.0 + exp(-beta * (x - alpha)));
}

void algor::bilateralFilterRange(int kernelSize, double spatialSigma, double rangeSigma) 
{
    modData_ = std::make_shared<BGRValue[]>(width_ * height_);

    int half_kernel = kernelSize / 2;
    for (int i = 0; i < height_; i+=1) {
        for (int j = 0; j < width_; j+=1) {
            double norm_factor_b = 0.0;
            double filtered_value_b = 0.0;
            
            double norm_factor_r = 0.0;
            double filtered_value_r = 0.0;

            double norm_factor_g = 0.0;
            double filtered_value_g = 0.0;

            for (int k = -half_kernel; k <= half_kernel; k+=1) {
                for (int l = -half_kernel; l <= half_kernel; l+=1) {
                    int ni = i + k;
                    int nj = j + l;

                    if (ni >= 0 && ni < height_ && nj >= 0 && nj < width_) {
                        // blue channal
                        double spatial_distance = sqrt(k * k + l * l);
                        double range_distance = origData_[nj*width_ + ni].b - origData_[j*width_ + i].b;
                        double spatial_weight = gaussianRange(spatial_distance, spatialSigma);
                        double range_weight = gaussianRange(range_distance, rangeSigma);
                        double weight = spatial_weight * range_weight;
                        filtered_value_b += origData_[nj*width_ + ni].b * weight;
                        norm_factor_b += weight;
                        // green channal
                        spatial_distance = sqrt(k * k + l * l);
                        range_distance = origData_[nj*width_ + ni].g - origData_[j*width_ + i].g;
                        spatial_weight = gaussianRange(spatial_distance, spatialSigma);
                        range_weight = gaussianRange(range_distance, rangeSigma);
                        weight = spatial_weight * range_weight;
                        filtered_value_g += origData_[nj*width_ + ni].g * weight;
                        norm_factor_g += weight;
                        // red channal
                        spatial_distance = sqrt(k * k + l * l);
                        range_distance = origData_[nj*width_ + ni].r - origData_[j*width_ + i].r;
                        spatial_weight = gaussianRange(spatial_distance, spatialSigma);
                        range_weight = gaussianRange(range_distance, rangeSigma);
                        weight = spatial_weight * range_weight;
                        filtered_value_r += origData_[nj*width_ + ni].r * weight;
                        norm_factor_r += weight;
                    }
                }
            }

            modData_[j*width_ + i].b = filtered_value_b / norm_factor_b;
            modData_[j*width_ + i].g = filtered_value_g / norm_factor_g;
            modData_[j*width_ + i].r = filtered_value_r / norm_factor_r;
        }
    }
}

void algor::newbilateralFilter(int kernelSize, double spatialSigma, double intensitySigma) 
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
                        double weight_b = gaussianBase(i, j, spatialSigma) * gaussianBase(intensityDiff_b, 0, intensitySigma);
                        filteredValue_b += origData_[neighborX * width_ + neighborY].b * weight_b;
                        weightSum_b += weight_b;

                        double intensityDiff_g = origData_[neighborX * width_ + neighborY].g - origData_[x * width_ + y].g;
                        double weight_g = gaussianBase(i, j, spatialSigma) * gaussianBase(intensityDiff_g, 0, intensitySigma);
                        filteredValue_g += origData_[neighborX * width_ + neighborY].g * weight_g;
                        weightSum_g += weight_g;

                        double intensityDiff_r = origData_[neighborX * width_ + neighborY].r - origData_[x * width_ + y].r;
                        double weight_r = gaussianBase(i, j, spatialSigma) * gaussianBase(intensityDiff_r, 0, intensitySigma);
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

BGRValued algor::getMSE()
{
    BGRValued output;
    if (!origImageAvailable() || !modImageAvailable() ) return output;

    for (int i = 0; i < width_ * height_; i += 1)
    {
        output.b += (origData_[i].b - modData_[i].b) * (origData_[i].b - modData_[i].b);
        output.g += (origData_[i].g - modData_[i].g) * (origData_[i].g - modData_[i].g);
        output.r += (origData_[i].r - modData_[i].r) * (origData_[i].r - modData_[i].r);
    }
    output.b /= (width_*height_);
    output.g /= (width_*height_);
    output.r /= (width_*height_);
    return output;
}

BGRValued algor::getPSNR()
{
    BGRValued output;
    if (!origImageAvailable() || !modImageAvailable() ) return output;

    for (int i = 0; i < width_ * height_; i += 1)
    {
        output.b += (origData_[i].b - modData_[i].b) * (origData_[i].b - modData_[i].b);
        output.g += (origData_[i].g - modData_[i].g) * (origData_[i].g - modData_[i].g);
        output.r += (origData_[i].r - modData_[i].r) * (origData_[i].r - modData_[i].r);
    }
    output.b /= (width_*height_);
    output.g /= (width_*height_);
    output.r /= (width_*height_);

    output.b = (20 * std::log10(255/sqrt(output.b)));
    output.g = (20 * std::log10(255/sqrt(output.g)));
    output.r = (20 * std::log10(255/sqrt(output.r)));
    return output;
}

BGRValued algor::getSSIM(int radius, int c1, int c2)
{
    BGRValued mu1 = average(origData_.get(), width_*height_);
    BGRValued mu2 = average(modData_.get(), width_*height_);

    BGRValued sigma1 = variance(origData_.get(), width_*height_, mu1);
    BGRValued sigma2 = variance(modData_.get(), width_*height_, mu2);

    BGRValued sigma12 = covariance(origData_.get(), modData_.get(), width_*height_, mu1, mu2);

    BGRValued output;

    output.b = ((2 * mu1.b * mu2.b + c1) * (2 * sigma12.b + c2)) / ((mu1.b * mu1.b + mu2.b * mu2.b + c1) * (sigma1.b + sigma2.b + c2));
    output.g = ((2 * mu1.g * mu2.g + c1) * (2 * sigma12.g + c2)) / ((mu1.g * mu1.g + mu2.g * mu2.g + c1) * (sigma1.g + sigma2.g + c2));
    output.r = ((2 * mu1.r * mu2.r + c1) * (2 * sigma12.r + c2)) / ((mu1.r * mu1.r + mu2.r * mu2.r + c1) * (sigma1.r + sigma2.r + c2));

    return output;
}

BGRValued algor::average(BGRValue* arr, int size) 
{
    BGRValued sum;
    for (int i = 0; i < size; i++) {
        sum.b += arr[i].b;
        sum.g += arr[i].g;
        sum.r += arr[i].r;
    }
    sum.b /= size;
    sum.g /= size;
    sum.r /= size;
    return sum;
}

BGRValued algor::variance(BGRValue* arr, int size, BGRValued mean) 
{
    BGRValued sum;
    for (int i = 0; i < size; i++) {
        sum.b += (arr[i].b - mean.b) * (arr[i].b - mean.b);
        sum.g += (arr[i].g - mean.g) * (arr[i].g - mean.g);
        sum.r += (arr[i].r - mean.r) * (arr[i].r - mean.r);
    }
    sum.b /= size;
    sum.g /= size;
    sum.r /= size;
    return sum;
}

BGRValued algor::covariance(BGRValue* arr1,  BGRValue* arr2, int size, BGRValued mean1, BGRValued mean2) 
{
    BGRValued sum;
    for (int i = 0; i < size; i++) {
        sum.b += (arr1[i].b - mean1.b) * (arr2[i].b - mean2.b);
        sum.g += (arr1[i].g - mean1.g) * (arr2[i].g - mean2.g);
        sum.r += (arr1[i].r - mean1.r) * (arr2[i].r - mean2.r);
    }
    sum.b/=size;
    sum.g/=size;
    sum.r/=size;
    return sum;
}

void algor::swapOrigMod()
{
    modData_.swap(origData_);
}