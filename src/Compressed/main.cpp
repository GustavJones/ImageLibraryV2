#include "GParsing/GParsing.hpp"
#include "GArgs/GArgs.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

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
  std::vector<unsigned char> buf;

  GParsing::HTTPRequest req;
  GParsing::HTTPResponse resp;
  GArgs::Parser p("Image Library V2 - Reload", "V0.1");
  SetupArgsParser(p, argc, argv);

  if (p.Contains("flags", "-h")) {
    p.DisplayHelp();
  }

  if (p["filepath"].size() != 1) {
    std::cerr << "Not the right amount of arguments, only 1 needed." << std::endl;
    return EXIT_FAILURE;
  }

  req = ReadRequest(p["filepath"][0]);

  resp.version = "HTTP/1.1";
  resp.response_code = 301;
  resp.response_code_message = "Moved Permanently";
  resp.headers.push_back({"Location", {"/"}});
  resp.headers.push_back({"Connection", {"close"}});

  buf = resp.CreateResponse();
  WriteResponse(p["filepath"][0], buf);

  return EXIT_SUCCESS;
}
