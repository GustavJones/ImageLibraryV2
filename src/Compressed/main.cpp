#include <streambuf>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "GParsing/GParsing.hpp"
#include "GArgs/GArgs.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>
#include "Configuration/Configuration.h"
#include "stb_image_resize2.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "Compressed/Image.h"
#include "TinyEXIF.h"

constexpr static unsigned short RESOLUTION_DIVISION = 4;
constexpr static unsigned short OUTPUT_QUILITY = 65;

GParsing::HTTPRequest ReadRequest(const std::filesystem::path &_file) {
  GParsing::HTTPRequest req;
  std::fstream f;
  std::string fContent;
  char fChar;

  f.open(_file, std::ios::in);

  while (!f.eof()) {
    f.get(fChar);
    fContent += fChar;
  }

  f.close();

  req.ParseRequest(GParsing::ConvertToCharArray(fContent.c_str(), fContent.length()));
  fContent = "";

  return req;
}

void WriteResponse(const std::filesystem::path &_file, const std::vector<unsigned char> &_buf) {
  GParsing::HTTPResponse resp;
  std::fstream f;
  std::string fContent;
  char fChar;

  f.open(_file, std::ios::out);

  for (unsigned long i = 0; i < _buf.size(); i++) {
    f.put(_buf[i]);
  } 

  f.close();
}


void WriteErrorResponse(const std::filesystem::path &_file) {
  GParsing::HTTPResponse resp;
  std::fstream f;
  std::string fContent;
  char fChar;

  std::vector<unsigned char> buf;

  resp.version = "HTTP/1.1";
  resp.response_code = 500;
  resp.response_code_message = "Internal Server Error";
  resp.headers.push_back({"Connection", {"close"}});

  f.open(_file, std::ios::out);

  for (unsigned long i = 0; i < buf.size(); i++) {
    f.put(buf[i]);
  } 

  f.close();
}

void SetupArgsParser(GArgs::Parser &_parser, int _argc, char *_argv[]) {
  _parser.AddStructure(
      "[flags:help=Program "
      "Flags,argument_filter=-,value_amount=0;filepath:help=Path to "
      "input/output "
      "HTTP requests/responses,value_amount=1]");

  _parser.AddKey(GArgs::Key("flags", "--help | -h", "Display this message"));
  _parser.AddKey(GArgs::Key("filepath", "* | any path",
                            "Set the path to input/output temp file"));

  _parser.ParseArgs(_argc, _argv);
}

int main(int argc, char *argv[]) {
  std::fstream f;
  std::vector<unsigned char> fContent;
  char fChar;

  unsigned char *originalImageBuffer;
  unsigned char *newImageBuffer;
  int originalImageBufferX, originalImageBufferY, originalImageBufferChannels;
  int newImageBufferX, newImageBufferY, newImageBufferChannels;
  std::vector<unsigned char> buf;

  GParsing::HTTPRequest req;
  GParsing::HTTPResponse resp;
  GArgs::Parser p("Image Library V2 - Reload", "V0.1");
  SetupArgsParser(p, argc, argv);

  const std::filesystem::path EXECUTABLE_PATH = std::filesystem::canonical(argv[0]);
  const std::filesystem::path EXECUTABLE_DIR = EXECUTABLE_PATH.parent_path();
  const std::filesystem::path DATA_DIR = ((std::filesystem::path)p["filepath"][0]).parent_path();

  if (p.Contains("flags", "-h")) {
    p.DisplayHelp();
  }

  if (p["filepath"].size() != 1) {
    std::cerr << "Not the right amount of arguments, only 1 needed." << std::endl;
    return EXIT_FAILURE;
  }

  req = ReadRequest(p["filepath"][0]);

  unsigned long index = req.uri.find("/compressed");
  req.uri.erase(index, 11);

  std::cout << "Getting Image Path" << std::endl;
  AdvancedWebserver::Configuration c;
  c.ReadFile(req.uri, DATA_DIR);

  std::cout << "Loading Image from Path" << std::endl;
  originalImageBuffer = stbi_load(c.GetPath().c_str(), &originalImageBufferX, &originalImageBufferY, &originalImageBufferChannels, 0);
  newImageBufferX = originalImageBufferX / RESOLUTION_DIVISION;
  newImageBufferY = originalImageBufferY / RESOLUTION_DIVISION;
  newImageBufferChannels = originalImageBufferChannels;

 
  f.open(c.GetPath(), std::ios::in | std::ios::binary);
  TinyEXIF::EXIFInfo imageEXIF;
  imageEXIF.parseFrom(f);
  f.close();

  newImageBuffer = new unsigned char[newImageBufferX * newImageBufferY * newImageBufferChannels]();

  std::cout << "Resize Image from Buffer" << std::endl;
  stbir_resize_uint8_linear(originalImageBuffer, originalImageBufferX, originalImageBufferY, 0, newImageBuffer, newImageBufferX, newImageBufferY, 0, (stbir_pixel_layout)newImageBufferChannels);

  ImageLibraryV2::Image img;
  img.SetChannels(newImageBufferChannels);
  img.SetX(newImageBufferX);
  img.SetY(newImageBufferY);
  img.Populate(newImageBuffer);

  switch (imageEXIF.Orientation) {
    case 8:
      img.RotateAntiClockwise();
      break;
    default:
      break;
  }

  img.ToBuffer(newImageBuffer);
  stbi_write_jpg((p["filepath"][0] + ".jpg").c_str(), img.GetX(), img.GetY(), newImageBufferChannels, newImageBuffer, OUTPUT_QUILITY);
  delete[] newImageBuffer;
  stbi_image_free(originalImageBuffer);

  std::cout << "Compressed: " << req.uri << std::endl;

  std::cout << "Reading output file from disk" << std::endl;

  
  f.open((p["filepath"][0] + ".jpg"), std::ios::in | std::ios::binary);

  while (!f.eof()) {
    f.get(fChar);
    fContent.push_back(fChar);
  }

  f.close();
  std::filesystem::remove((p["filepath"][0] + ".jpg"));

  resp.version = "HTTP/1.1";
  resp.response_code = 200;
  resp.response_code_message = "OK";
  resp.headers.push_back({"Connection", {"close"}});
  resp.headers.push_back({"Content-Type", {"image/jpg"}});
  resp.message = fContent;

  buf = resp.CreateResponse();
  WriteResponse(p["filepath"][0], buf);

  return EXIT_SUCCESS;
}
