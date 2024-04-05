#include <algor.h>

algor::~algor()
{
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
    origData_ = nullptr;
    modData_ = nullptr;
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
unsigned int algor::calculateMedian(unsigned int* arr, int size)
{
    bubbleSort(arr, size);
    return arr[size / 2];
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
void algor::haarForwardTransform(int levels) 
{
  for (int level = 1; level <= levels; level+=1) 
  {
    int step = 1 << (level - 1);
    for (int i = 0; i < height_; i += 2 * step) 
    {
      for (int j = 0; j < width_; j += 2 * step) 
      {
        // double ll = (origData_[i * width_ + j] + origData_[(i + step) * width_ + j] + origData_[i * width_ + j + step] + origData_[(i + step) * width_ + j + step]) * 0.5;
        // double lh = (origData_[i * width_ + j] - origData_[(i + step) * width_ + j]) * scaling_factor(level);
        // double hl = (origData_[i * width_ + j] - origData_[i * width_ + j + step]) * scaling_factor(level);
        // double hh = (origData_[(i + step) * width_ + j] - origData_[(i + step) * width_ + j + step]) * scaling_factor(level);

        // modData_[i * width_ + j] = ll;
        // modData_[(i + step) * width_ + j] = lh;
        // modData_[i * width_ + j + step] = hl;
        // modData_[(i + step) * width_ + j + step] = hh;
      }
    }
  }
}
void algor::haarInverseTransform(int levels) 
{
  for (int level = levels; level >= 1; --level) 
  {
    // int step = 1 << (level - 1);
    // for (int i = 0; i < height; i += 2 * step) 
    // {
    //   for (int j = 0; j < width_; j += 2 * step) 
    //   {
    //     double ll = modData_[i * width_ + j];
    //     double lh = modData_[(i + step) * width_ + j];
    //     double hl = modData_[i * width_ + j + step];
    //     double hh = modData_[(i + step) * width_ + j + step];

    //     modData_[i * width_ + j] = ll + scaling_factor(level) * (lh + hl + hh);
    //     modData_[(i + step) * width_ + j] = ll + scaling_factor(level) * (lh - hh);
    //     modData_[i * width_ + j + step] = ll + scaling_factor(level) * (-lh + hh);
    //     modData_[(i + step) * width_ + j + step] = ll - scaling_factor(level) * (lh + hl - hh);
    //   }
    // }
  }
}