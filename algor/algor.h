#ifndef ALGOR_H
#define ALGOR_H

struct BGRValue 
{
    unsigned int b = 0, g = 0, r = 0;
};

// 1 -- columns -> rows

class algor
{
public:
    algor() = default;
    ~algor();
    void setOrigImageSize(unsigned int width, unsigned int height);
    bool setOrigImagePixel(int column, int row, const BGRValue &pixel);
    int getImageWidth() const;
    int getImageHeight() const;
    BGRValue getOrigImagePixel(int row, int column) const;
    BGRValue getModImagePixel(int row, int column) const;
    bool origImageAvailable() const;
    bool modImageAvailable() const;
    void clearData(); 
private:
    unsigned int height_;
    unsigned int width_;
    BGRValue * origData_ = nullptr;
    BGRValue * modData_ = nullptr;
};

#endif