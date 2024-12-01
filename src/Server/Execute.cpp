#include "Server/Execute.h"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

namespace ImageLibraryV2 {
std::string Execute(const std::string &_command,
                    const std::filesystem::path &_tempDir) {
  std::string tempDir = _tempDir;
  int replaceIndex = tempDir.find('~');
  if (replaceIndex != tempDir.npos) {
    tempDir.replace(replaceIndex, 1, getenv("HOME"));
  }
  std::filesystem::create_directory(tempDir);
  tempDir = (std::filesystem::path)tempDir / "temp";
  std::filesystem::create_directory(tempDir);
  tempDir = (std::filesystem::path)tempDir / "XXXXXX";

  char *tempFile = new char[tempDir.length() + 1]();
  std::string command;
  std::string commandOutput;
  char *fContent;
  unsigned long fSize;
  std::fstream f;

  for (int i = 0; i < tempDir.length(); i++) {
    tempFile[i] = tempDir[i];
  }

  tempFile[tempDir.length()] = '\0';

  close(mkstemp(tempFile));

  command = _command + " >> " + tempFile;
  if (std::system(command.c_str()) != EXIT_SUCCESS) {
    remove(tempFile);
    delete[] tempFile;
    return {};
  }

  f.open(tempFile, std::ios::in | std::ios::ate);
  fSize = f.tellg();
  f.seekg(f.beg);
  fContent = new char[fSize]();
  f.read(fContent, fSize);

  for (int i = 0; i < fSize; i++) {
    commandOutput += fContent[i];
  }
  delete[] fContent;

  f.close();

  remove(tempFile);
  delete[] tempFile;
  return commandOutput;
}

void Execute(const std::string &_command){
  std::system(_command.c_str());
}

} // namespace ImageLibraryV2
