#pragma once
#include <vector>

namespace ImageLibraryV2 {
  class Pixel {
  public:
    Pixel();
    Pixel(Pixel &&) = default;
    Pixel(const Pixel &) = default;
    Pixel &operator=(Pixel &&) = default;
    Pixel &operator=(const Pixel &) = default;
    ~Pixel();

    /// Sets the amount of bytes per pixel
    /// Usually 3 for RGB
    void SetChannels(unsigned long _channels);

    /// Get the amount of bytes per pixel
    [[nodiscard]]
    unsigned long GetChannels() const;

    /// Set the values for individual channels
    /// Contains the same amount of values as set by SetChannels
    void SetPixel(const std::vector<unsigned char> &_pixelChannels);

    // Get the values for individual channels
    [[nodiscard]]
    std::vector<unsigned char> GetPixel() const;

  private:
    unsigned long m_channelsCount;
    std::vector<unsigned char> m_channels;
  };
}
