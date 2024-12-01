#include "GArgs/GArgs.hpp"
#include "GParsing/GParsing.hpp"
#include "Reload/ConfigFile.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  GParsing::HTTPRequest req;
  GParsing::HTTPResponse resp;
  std::vector<unsigned char> respVector;
  char *respContent;

  std::fstream f;
  unsigned long fSize;
  char *fContent;

  std::string command;
  const std::filesystem::path EXECUTABLE_PATH =
      std::filesystem::absolute(argv[0]);
  const std::filesystem::path EXECUTABLE_DIR = EXECUTABLE_PATH.parent_path();
  const std::filesystem::path WEBSITE_DIR =
      EXECUTABLE_PATH.parent_path() / "website";

  GArgs::Parser p("Image Library V2 - Reload", "V0.1");
  p.AddStructure("[flags:help=Program "
                 "Flags,argument_filter=-,value_amount=0;filepath:help=Path to "
                 "input/output "
                 "HTTP requests/responses]");

  p.AddKey(GArgs::Key("flags", "--help | -h", "Display this message"));
  p.AddKey(GArgs::Key("filepath", "* | any path",
                      "Set the path to input/output temp file"));

  p.ParseArgs(argc, argv);

  if (p.Contains("flags", "--help") || p.Contains("flags", "-h")) {
    p.DisplayHelp();
    return EXIT_SUCCESS;
  }

  if (p["filepath"].size() != 1) {
    std::cerr << "------------------------" << std::endl;
    std::cerr << "Please enter a filepath:" << std::endl;
    std::cerr << "------------------------" << std::endl;

    p.DisplayHelp();
    return EXIT_FAILURE;
  }

  if (!std::filesystem::exists(p["filepath"][0])) {
    return EXIT_FAILURE;
  }

  f.open(p["filepath"][0], std::ios::in | std::ios::binary | std::ios::ate);
  fSize = f.tellg();
  f.seekg(f.beg);

  fContent = new char[fSize]();
  f.read(fContent, fSize);
  req.ParseRequest(GParsing::ConvertToCharArray(fContent, fSize));
  delete[] fContent;
  f.close();

  if (req.uri.find("/reload") == req.uri.npos) {
    resp.version = "HTTP/1.1";
    resp.response_code = 500;
    resp.response_code_message = "Internal Server Error";
    resp.headers.push_back({"Connection", {"close"}});

    respVector = resp.CreateResponse();
    respContent = new char[respVector.size()]();
    GParsing::ConvertToCharPointer(respVector, respContent);

    f.open(p["filepath"][0], std::ios::out | std::ios::binary);
    f.write(respContent, respVector.size());
    f.close();

    delete[] respContent;

    return EXIT_FAILURE;
  }

  ImageLibraryV2::ConfigFile c(EXECUTABLE_DIR / "include-directories.txt");

  for(const auto &directory : c.ReadPaths()) {
    
  }

  // Response
  resp.version = "HTTP/1.1";
  resp.response_code = 301;
  resp.response_code_message = "Moved Permanently";
  resp.headers.push_back({"Location", {"/"}});

  respVector = resp.CreateResponse();
  respContent = new char[respVector.size()]();
  GParsing::ConvertToCharPointer(respVector, respContent);

  f.open(p["filepath"][0], std::ios::out | std::ios::binary);
  f.write(respContent, respVector.size());
  f.close();

  delete[] respContent;

  return EXIT_SUCCESS;
}
