#include "Reload/ConfigFile.h"
#include <asm-generic/errno.h>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

namespace ImageLibraryV2 {
ConfigFile::ConfigFile(const std::filesystem::path &_path)
    : m_filePath(_path) {}

std::vector<std::filesystem::path> ConfigFile::ReadPaths() {
  std::ifstream f(m_filePath, std::ios::in);
  std::vector<std::filesystem::path> out;
  std::string line;
  char c;

  f.seekg(f.beg);

  while (!f.eof()) {
    f.get(c);
    if (c == '\n' || f.eof()) {
      if (line != "") {
        out.push_back(line);
      }
      line = "";
    } else {
      line += c;
    }
  }

  f.close();
  return out;
}

void ConfigFile::WritePaths(const std::vector<std::filesystem::path> &_paths) {
  std::ofstream f(m_filePath, std::ios::out);
  for (const std::filesystem::path &path : _paths) {
    for (const char &c : path.string()) {
      f.put(c);
    }
    f.put('\n');
  }

  f.flush();
  f.close();
}

void ConfigFile::AppendPath(const std::filesystem::path &_path) {
  std::vector<std::filesystem::path> read;
  read = ReadPaths();

  read.push_back(_path);
  WritePaths(read);
}

ConfigFile::~ConfigFile() {  }
} // namespace ImageLibraryV2
