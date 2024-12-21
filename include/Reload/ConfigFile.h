#pragma once
#include <filesystem>
#include <vector>

namespace ImageLibraryV2 {
class ConfigFile {
public:
  ConfigFile(const std::filesystem::path &_path);
  ConfigFile(ConfigFile &&) = default;
  ConfigFile(const ConfigFile &) = delete;
  ConfigFile &operator=(ConfigFile &&) = default;
  ConfigFile &operator=(const ConfigFile &) = delete;
  ~ConfigFile();

  std::vector<std::filesystem::path> ReadPaths();
  void WritePaths(const std::vector<std::filesystem::path> &_paths);
  void AppendPath(const std::filesystem::path &_path);

private:
  std::filesystem::path m_filePath;
};
} // namespace ImageLibraryV2
