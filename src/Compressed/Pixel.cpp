#include "Compressed/Pixel.h"

namespace ImageLibraryV2 {
Pixel::Pixel() : m_channelsCount(0), m_channels() {}
Pixel::~Pixel() {}

void Pixel::SetChannels(unsigned long _channels) {
  m_channelsCount = _channels;
}

unsigned long Pixel::GetChannels() const {
  return m_channelsCount;
}

void Pixel::SetPixel(const std::vector<unsigned char> &_pixelChannels) {
  if (_pixelChannels.size() == m_channelsCount) {
    m_channels = _pixelChannels;
  }
}

std::vector<unsigned char> Pixel::GetPixel() const {
  return m_channels;
}
} // namespace ImageLibraryV2
