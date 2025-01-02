#pragma once
#include "Compressed/Pixel.h"
#include <vector>

namespace ImageLibraryV2 {
  class Image {
  public:
    Image();
    Image(Image &&) = default;
    Image(const Image &) = default;
    Image &operator=(Image &&) = default;
    Image &operator=(const Image &) = default;
    ~Image();

    void SetX(unsigned long _x);
    [[nodiscard]]
    unsigned long GetX() const;

    void SetY(unsigned long _y);
    [[nodiscard]]
    unsigned long GetY() const;

    void SetChannels(unsigned long _channels);
    [[nodiscard]]
    unsigned long GetChannels() const;

    ImageLibraryV2::Pixel SetPixel(const ImageLibraryV2::Pixel &_p, unsigned long _x, unsigned long _y);
    ImageLibraryV2::Pixel GetPixel(unsigned long _x, unsigned long _y) const;

    void Populate(unsigned char *_buf);
    void Populate();

    void RotateClockwise();
    void RotateAntiClockwise();

    const unsigned char *ToBuffer(unsigned char *_buf);

  private:
    unsigned long m_x, m_y;
    unsigned long m_channelCount;
    std::vector<std::vector<ImageLibraryV2::Pixel>> m_pixels;
  };
}
