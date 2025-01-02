#include "Compressed/Image.h"
#include <vector>

namespace ImageLibraryV2 {
Image::Image() : m_x(0), m_y(0), m_channelCount(0), m_pixels() {}
Image::~Image() {}

void Image::SetX(unsigned long _x) { m_x = _x; }
unsigned long Image::GetX() const { return m_x; }

void Image::SetY(unsigned long _y) { m_y = _y; }
unsigned long Image::GetY() const { return m_y; }

void Image::SetChannels(unsigned long _channels) { m_channelCount = _channels; }
unsigned long Image::GetChannels() const { return m_channelCount; }

ImageLibraryV2::Pixel Image::SetPixel(const ImageLibraryV2::Pixel &_p,
                                      unsigned long _x, unsigned long _y) {
  ImageLibraryV2::Pixel old = m_pixels[_y][_x];
  m_pixels[_y][_x] = _p;
  return old;
}

ImageLibraryV2::Pixel Image::GetPixel(unsigned long _x,
                                      unsigned long _y) const {
  return m_pixels[_y][_x];
}

void Image::Populate() {
  m_pixels.clear();

  for (unsigned long i = 0; i < m_y; i++) {
    m_pixels.push_back(std::vector<ImageLibraryV2::Pixel>());

    for (unsigned long j = 0; j < m_x; j++) {
      m_pixels[i].emplace_back();
      m_pixels[i][j].SetChannels(m_channelCount);
    }
  }
}

void Image::Populate(unsigned char *_buf) {
  m_pixels.clear();

  unsigned long bufferIndex = 0;

  for (unsigned long i = 0; i < m_y; i++) {
    m_pixels.push_back(std::vector<ImageLibraryV2::Pixel>());

    for (unsigned long j = 0; j < m_x; j++) {
      std::vector<unsigned char> p;

      m_pixels[i].emplace_back();
      m_pixels[i][j].SetChannels(m_channelCount);

      for (unsigned long k = 0; k < m_channelCount; k++) {
        p.push_back(_buf[bufferIndex++]);
      }

      m_pixels[i][j].SetPixel(p);
    }
  }
}

const unsigned char *Image::ToBuffer(unsigned char *_buf) {
  unsigned long bufferIndex = 0;

  for (unsigned long i = 0; i < m_y; i++) {
    for (unsigned long j = 0; j < m_x; j++) {
      ImageLibraryV2::Pixel p = m_pixels[i][j];

      for (const auto &byte : p.GetPixel()) {
        _buf[bufferIndex++] = byte;
      }
    }
  }

  return _buf;
}

void Image::RotateClockwise() {
  unsigned char *buf;
  ImageLibraryV2::Image img;
  img.SetChannels(m_channelCount);
  img.SetX(m_y);
  img.SetY(m_x);
  img.Populate();

  for (unsigned long i = 0; i < m_y; i++) {
    for (unsigned long j = 0; j < m_x; j++) {
      img.SetPixel(m_pixels[i][j], img.GetX() - 1 - i, j);
    }
  }
  *this = img;
}


void Image::RotateAntiClockwise() {
  unsigned char *buf;
  ImageLibraryV2::Image img;
  img.SetChannels(m_channelCount);
  img.SetX(m_y);
  img.SetY(m_x);
  img.Populate();

  for (unsigned long i = 0; i < m_y; i++) {
    for (unsigned long j = 0; j < m_x; j++) {
      img.SetPixel(m_pixels[i][j], i, img.GetY() - 1 - j);
    }
  }

  *this = img;
}

} // namespace ImageLibraryV2
