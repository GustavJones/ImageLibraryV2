#pragma once
#include <filesystem>
#include <string>

namespace ImageLibraryV2 {
std::string Execute(const std::string &_command, const std::filesystem::path &_tempDir);
void Execute(const std::string &_command);
}
