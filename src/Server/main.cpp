#include "Server/Execute.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>

int main(int argc, char *argv[]) {
  std::string command;
  const std::filesystem::path EXECUTABLE_PATH = std::filesystem::absolute(argv[0]);
  const std::filesystem::path EXECUTABLE_DIR = EXECUTABLE_PATH.parent_path();
  const std::filesystem::path WEBSITE_DIR =
      EXECUTABLE_PATH.parent_path() / "website";

  std::filesystem::create_directory(EXECUTABLE_DIR);

  if (argc != 3) {
    std::cerr << "Not the right amount of arguments" << std::endl;
    return 1;
  }

  // Clone website files to local directory
  command = "git clone --recursive ";
  command += CLONE_URI;
  command += " ";
  command += WEBSITE_DIR;

  if (!std::filesystem::exists(WEBSITE_DIR)) {
    std::cout << ImageLibraryV2::Execute(command, EXECUTABLE_DIR);
  }

  // Error if git is not installed
  if (!std::filesystem::exists(WEBSITE_DIR)) {
    std::cerr << "Git clone error!" << std::endl;
    std::cerr << "Make sure git is installed." << std::endl;
    return 1;
  }

  // Setup AdvancedWebserver configuration files
  command = getenv("SHELL");
  command += " -c ";
  command += WEBSITE_DIR / "SetupAdvancedWebserver.sh";
  ImageLibraryV2::Execute(command);

  command = "AdvancedWebserver-Configure-Tool --set-data-dir=";
  command += WEBSITE_DIR / "server";
  command += " ";
  command += "/reload";
  command += " ";
  command += "executable";
  command += " ";
  command += EXECUTABLE_DIR / "ImageLibraryV2-Reload";
  ImageLibraryV2::Execute(command);

  // Start server
  command = "AdvancedWebserver --set-data-dir=";
  command += WEBSITE_DIR / "server";
  command += " ";
  command += argv[1];
  command += " ";
  command += argv[2];

  ImageLibraryV2::Execute(command);
  return EXIT_SUCCESS;
}
