#include "Core/Core.h"
#include "GArgs/GArgs.hpp"
#include "GParsing/GParsing.hpp"
#include "Reload/ConfigFile.h"
#include "TinyEXIF.h"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <vector>

constexpr char WEBSITE_DIR_RELATIVE_FROM_DATA_DIR[] = "..";
constexpr char LIBRARY_DIR_RELATIVE_FROM_DATA_DIR[] = "library";
constexpr char TEMPLATES_DIR_RELATIVE_FROM_WEBSITE_DIR[] = "html-templates";
constexpr char IMAGE_URI_PREFIX[] = "/library/";
const std::string IMAGE_FOLDER_TILE_IDENTIFIER = "<!-- $image-folder-tile -->";
const std::string IMAGE_FOLDER_TILE_NAME_IDENTIFIER = "<!-- $image-folder-name -->";
const std::string IMAGE_FOLDER_PAGE_NAME_IDENTIFIER = "<!-- $image-folder-name -->";
const std::string IMAGE_TILE_IDENTIFIER = "<!-- $image-tile -->";
const std::string IMAGE_TILE_NAME_IDENTIFIER = "<!-- $image-name -->";

void AddImageToDateFolderFile(const std::filesystem::path &_websiteDir,
                              const std::filesystem::path &_dataDir,
                              const std::string &_date,
                              const std::filesystem::path &_imageURI) {
  std::fstream f;
  std::string fContent;
  std::string fTemplateContent;
  char fChar;

  unsigned long index;

  f.open(_websiteDir / TEMPLATES_DIR_RELATIVE_FROM_WEBSITE_DIR / "image-tile.html", std::ios::in);

  while (!f.eof()) {
    f.get(fChar);
    fTemplateContent += fChar;
  }

  f.close();

  f.open(_dataDir / LIBRARY_DIR_RELATIVE_FROM_DATA_DIR / (_date + ".html"), std::ios::in);

  while (!f.eof()) {
    f.get(fChar);
    fContent += fChar;
  }

  f.close();

  index = fTemplateContent.find(IMAGE_FOLDER_PAGE_NAME_IDENTIFIER);
  while (index != fTemplateContent.npos) {
    fTemplateContent.insert(index + IMAGE_FOLDER_PAGE_NAME_IDENTIFIER.length(), _date);
    fTemplateContent.erase(index, IMAGE_FOLDER_PAGE_NAME_IDENTIFIER.length());
    index = fTemplateContent.find(IMAGE_FOLDER_PAGE_NAME_IDENTIFIER);
  }

  index = fTemplateContent.find(IMAGE_TILE_NAME_IDENTIFIER);
  while (index != fTemplateContent.npos) {
    fTemplateContent.insert(index + IMAGE_TILE_NAME_IDENTIFIER.length(), _imageURI);
    fTemplateContent.erase(index, IMAGE_TILE_NAME_IDENTIFIER.length());
    index = fTemplateContent.find(IMAGE_TILE_NAME_IDENTIFIER);
  }
  
  index = fContent.find(IMAGE_TILE_IDENTIFIER);
  fContent.insert(index + IMAGE_TILE_IDENTIFIER.length(), fTemplateContent);
  fContent.erase(index, IMAGE_TILE_IDENTIFIER.length());

  f.open(_dataDir / LIBRARY_DIR_RELATIVE_FROM_DATA_DIR / (_date + ".html"), std::ios::out);

  for (unsigned long i = 0; i < fContent.length(); i++) {
    f.put(fContent[i]);
  }

  f.close();
}

void ConfigureDateFolderURI(const std::filesystem::path &_dataDir,
                            const std::string &_date) {
  std::string command;
  command = "AdvancedWebserver-Configure-Tool --set-data-dir=";
  command += _dataDir;
  command += " ";
  command += IMAGE_URI_PREFIX;
  command += " ";
  command += "folder_io";
  command += " ";
  command += "\"";
  command += _dataDir / LIBRARY_DIR_RELATIVE_FROM_DATA_DIR;
  command += "\"";
  command += " ";
  command += "text/html";

  ImageLibraryV2::Execute(command);
}

void SetupDateFolderFile(const std::filesystem::path &_websiteDir,
                         const std::filesystem::path &_dataDir,
                         const std::string &_date) {
  const std::filesystem::path fPath =
      _dataDir / LIBRARY_DIR_RELATIVE_FROM_DATA_DIR / (_date + ".html");
  std::fstream f;
  std::string fTemplateContent;
  std::string fContent;
  char fChar;

  unsigned long index;

  // Read template file into memory buffer
  f.open(_websiteDir / TEMPLATES_DIR_RELATIVE_FROM_WEBSITE_DIR /
             "image-folder-page.html",
         std::ios::in);
  while (!f.eof()) {
    f.get(fChar);
    fTemplateContent += fChar;
  }
  f.close();

  // Write template identifiers
  index = fTemplateContent.find(IMAGE_FOLDER_PAGE_NAME_IDENTIFIER);
  while (index != fTemplateContent.npos) {
    fTemplateContent.insert(index + IMAGE_FOLDER_PAGE_NAME_IDENTIFIER.length(),
                            _date);
    fTemplateContent.erase(index, IMAGE_FOLDER_PAGE_NAME_IDENTIFIER.length());
    index = fTemplateContent.find(IMAGE_FOLDER_PAGE_NAME_IDENTIFIER, index + 1);
  }

  f.open(fPath, std::ios::out);

  for (unsigned long i = 0; i < fTemplateContent.length(); i++) {
    f.put(fTemplateContent[i]);
  }

  f.close();
}

void ConfigureCoverImageURI(const std::filesystem::path &_dataDir,
                            const std::filesystem::path &_executableDir,
                            const std::string &_date,
                            const std::string &_extension) {
  std::string command;
  command = "AdvancedWebserver-Configure-Tool --set-data-dir=";
  command += _dataDir;
  command += " ";
  command += IMAGE_URI_PREFIX + _date + '/' + _date;
  command += " ";
  command += "file_io";
  command += " ";
  command += "\"";
  command += _dataDir / LIBRARY_DIR_RELATIVE_FROM_DATA_DIR /
             (_date + '.' + _extension);
  command += "\"";
  command += " ";
  command += "image/" + _extension;

  ImageLibraryV2::Execute(command);

  command = "AdvancedWebserver-Configure-Tool --set-data-dir=";
  command += _dataDir;
  command += " ";
  command += IMAGE_URI_PREFIX + (std::string)"compressed/" + _date + '/' + _date;
  command += " ";
  command += "executable";
  command += " ";
  command += "\"";
  command += _executableDir / "ImageLibraryV2-Compressed";
  command += "\"";
  command += " ";
  command += "image/" + _extension;

  ImageLibraryV2::Execute(command);
}

void AddDateCoverImage(const std::filesystem::path &_dataDir,
                       const std::string &_date,
                       const std::filesystem::path &_imagePath,
                       const std::string &_extension) {
  std::filesystem::create_directory(_dataDir /
                                    LIBRARY_DIR_RELATIVE_FROM_DATA_DIR);

  if (!std::filesystem::exists(_dataDir / LIBRARY_DIR_RELATIVE_FROM_DATA_DIR /
                               (_date + '.' + _extension))) {
    std::filesystem::copy(_imagePath, _dataDir /
                                          LIBRARY_DIR_RELATIVE_FROM_DATA_DIR /
                                          (_date + '.' + _extension));
  }
}

bool HasDate(const std::filesystem::path &_libraryFile,
             const std::string &_date) {
  std::string fContent;
  char fChar;
  std::fstream f;
  f.open(_libraryFile, std::ios::in);
  while (!f.eof()) {
    f.get(fChar);
    fContent += fChar;
  }

  f.close();

  if (fContent.find(_date) == fContent.npos) {
    return false;
  } else {
    return true;
  }
}

void RefreshConfigurations(const std::filesystem::path &_websiteDir,
                           const std::filesystem::path &_executableDir,
                           const std::filesystem::path &_dataDir,
                           const std::filesystem::path &_tmpFile) {
  std::string command;
  // Remove old configurations
  std::filesystem::directory_iterator di(_dataDir);
  for (const auto &path : di) {
    if (std::filesystem::is_regular_file(path)) {
      if (path.path() != _tmpFile) {
        std::filesystem::remove(path);
      }
    }
  }

  // Run Website Setup
  command = getenv("SHELL");
  command += " -c ";
  command += _websiteDir / "SetupAdvancedWebserver.sh";
  ImageLibraryV2::Execute(command);

  command = "AdvancedWebserver-Configure-Tool --set-data-dir=";
  command += _dataDir;
  command += " ";
  command += "/reload";
  command += " ";
  command += "executable";
  command += " ";
  command += "\"";
  command += _executableDir / "ImageLibraryV2-Reload";
  command += "\"";
  ImageLibraryV2::Execute(command);
}

void WriteResponse(const std::filesystem::path &_tmpFile) {
  GParsing::HTTPResponse resp;
  std::vector<unsigned char> respVector;
  char *respContent;
  std::fstream f;

  // Response
  resp.version = "HTTP/1.1";
  resp.response_code = 301;
  resp.response_code_message = "Moved Permanently";
  resp.headers.push_back({"Location", {"/"}});
  resp.headers.push_back({"Connection", {"close"}});

  respVector = resp.CreateResponse();
  respContent = new char[respVector.size()]();
  GParsing::ConvertToCharPointer(respVector, respContent);

  for (int i = 0; i < respVector.size(); i++) {
    std::cout << respContent[i];
  }

  f.open(_tmpFile, std::ios::out | std::ios::binary);
  f.write(respContent, respVector.size());
  f.close();

  delete[] respContent;
}

void WriteErrorResponse(const std::filesystem::path &_tmpFile) {
  GParsing::HTTPResponse resp;
  std::vector<unsigned char> respVector;
  char *respContent;
  std::fstream f;

  resp.version = "HTTP/1.1";
  resp.response_code = 500;
  resp.response_code_message = "Internal Server Error";
  resp.headers.push_back({"Connection", {"close"}});

  respVector = resp.CreateResponse();
  respContent = new char[respVector.size()]();
  GParsing::ConvertToCharPointer(respVector, respContent);

  f.open(_tmpFile, std::ios::out | std::ios::binary);
  f.write(respContent, respVector.size());
  f.close();

  delete[] respContent;
}

GParsing::HTTPRequest ReadRequest(const std::filesystem::path &_tmpFile) {
  GParsing::HTTPRequest req;
  std::fstream f;
  unsigned long fSize;
  char *fContent;

  // Read Input request
  f.open(_tmpFile, std::ios::in | std::ios::binary | std::ios::ate);
  fSize = f.tellg();
  f.seekg(f.beg);

  fContent = new char[fSize]();
  f.read(fContent, fSize);
  req.ParseRequest(GParsing::ConvertToCharArray(fContent, fSize));
  delete[] fContent;
  f.close();

  return req;
}

void SetupArgsParser(GArgs::Parser &_parser, int _argc, char *_argv[]) {
  _parser.AddStructure(
      "[flags:help=Program "
      "Flags,argument_filter=-,value_amount=0;filepath:help=Path to "
      "input/output "
      "HTTP requests/responses]");

  _parser.AddKey(GArgs::Key("flags", "--help | -h", "Display this message"));
  _parser.AddKey(GArgs::Key("filepath", "* | any path",
                            "Set the path to input/output temp file"));

  _parser.ParseArgs(_argc, _argv);
}

int Validation(const GArgs::Parser &_p) {
  std::vector<std::string> values;

  // Validation
  if (_p.Contains("flags", "--help") || _p.Contains("flags", "-h")) {
    _p.DisplayHelp();
    return EXIT_SUCCESS;
  }

  for (const auto &pair : _p) {
    if (pair.first == "filepath") {
      values = pair.second;
    }
  }

  if (values.size() != 1) {
    std::cerr << "------------------------" << std::endl;
    std::cerr << "Please enter a filepath:" << std::endl;
    std::cerr << "------------------------" << std::endl;

    _p.DisplayHelp();
    return EXIT_FAILURE;
  }

  if (!std::filesystem::exists(values[0])) {
    return EXIT_FAILURE;
  }

  return -1;
}

bool ReadExif(const std::filesystem::path _path, std::string &_date,
              std::string &_extension) {
  std::fstream f;
  f.open(_path, std::ios::in);
  TinyEXIF::EXIFInfo imageEXIF(f);
  if (!imageEXIF.Fields) {
    f.close();
    return false;
  }

  _date = imageEXIF.DateTime.substr(0, imageEXIF.DateTime.find(' '));
  _extension = _path.extension().string().erase(0, 1);
  for (auto &c : _extension) {
    c = std::tolower(c);
  }

  f.close();
  return true;
}

void SetupLibraryFile(const std::filesystem::path &_websiteDir) {
  std::fstream f;
  unsigned long fSize;
  std::string fContentTemplate;
  std::string fContent;

  std::filesystem::remove(_websiteDir / "library.html");
  if (!std::filesystem::exists(_websiteDir / "library.html")) {
    // Generate library.html file from template
    fContentTemplate = "";
    f.open(_websiteDir / TEMPLATES_DIR_RELATIVE_FROM_WEBSITE_DIR /
               "library.html",
           std::ios::in | std::ios::ate);
    if (f.is_open()) {
      fSize = f.tellg();
      f.seekg(f.beg);

      for (unsigned long i = 0; i < fSize; i++) {
        fContentTemplate += f.get();
      }
    }
    f.close();

    f.open(_websiteDir / "library.html", std::ios::out);

    if (f.is_open()) {
      for (const auto &c : fContentTemplate) {
        f.put(c);
      }
    }

    f.close();
  }
}

void AddDateToLibraryFile(const std::filesystem::path &_websiteDir,
                          const std::string &_date) {
  std::fstream f;
  unsigned long fSize;
  std::string fContent;
  std::string fContentTemplate;
  unsigned long identifierIndex;

  // Read library file
  f.open(_websiteDir / "library.html", std::ios::in | std::ios::ate);

  if (f.is_open()) {
    fSize = f.tellg();
    f.seekg(f.beg);

    for (unsigned long i = 0; i < fSize; i++) {
      fContent += f.get();
    }
  }
  f.close();

  // Read folder tile template
  f.open(_websiteDir / TEMPLATES_DIR_RELATIVE_FROM_WEBSITE_DIR /
             "image-folder-tile.html",
         std::ios::in | std::ios::ate);

  if (f.is_open()) {
    fSize = f.tellg();
    f.seekg(f.beg);

    for (unsigned long i = 0; i < fSize; i++) {
      fContentTemplate += f.get();
    }
  }
  f.close();

  // Edit tile template to content
  identifierIndex = fContentTemplate.find(IMAGE_FOLDER_TILE_NAME_IDENTIFIER);
  while (identifierIndex != fContentTemplate.npos) {
    fContentTemplate.replace(identifierIndex,
                             IMAGE_FOLDER_TILE_NAME_IDENTIFIER.length(), _date);
    identifierIndex = fContentTemplate.find(IMAGE_FOLDER_TILE_NAME_IDENTIFIER);
  }

  // Replace input identifier with content tile
  identifierIndex = fContent.find(IMAGE_FOLDER_TILE_IDENTIFIER);
  if (identifierIndex != fContent.npos) {
    fContent.replace(identifierIndex, IMAGE_FOLDER_TILE_IDENTIFIER.length(),
                     fContentTemplate);
  }

  // Write updated library file
  f.open(_websiteDir / "library.html", std::ios::out);

  if (f.is_open()) {
    for (const auto &c : fContent) {
      f.put(c);
    }
  }

  f.close();
}

void ConfigureImageURI(const std::filesystem::path &_executableDir,
                       const std::filesystem::path &_dataDir,
                       const std::string &_date,
                       const std::string &_pathWithoutSpaces,
                       const std::filesystem::path &_path,
                       const std::string &_extension) {
  std::string command;
  command = "AdvancedWebserver-Configure-Tool --set-data-dir=";
  command += _dataDir;
  command += " ";
  command += IMAGE_URI_PREFIX + _date + '/' + _pathWithoutSpaces;
  command += " ";
  command += "file_io";
  command += " ";
  command += "\"";
  command += _path;
  command += "\"";
  command += " ";
  command += "image/" + _extension;

  ImageLibraryV2::Execute(command);

  command = "AdvancedWebserver-Configure-Tool --set-data-dir=";
  command += _dataDir;
  command += " ";
  command += IMAGE_URI_PREFIX + (std::string)"compressed/" + _date + '/' + _pathWithoutSpaces;
  command += " ";
  command += "executable";
  command += " ";
  command += "'";
  command += _executableDir / "ImageLibraryV2-Compressed";
  command += "'";

  ImageLibraryV2::Execute(command);

}

void ConfigureImageDirectory(const std::filesystem::path &_directory,
                             const std::filesystem::path &_executableDir,
                             const std::filesystem::path &_dataDir,
                             const std::filesystem::path &_websiteDir) {
  std::string pathWithoutSpaces;
  std::string command;
  std::fstream f;
  unsigned long fSize;
  std::string fContent;
  std::string fContentTemplate;
  std::string date;
  std::string extension;

  // Update Website Configuration
  const std::filesystem::recursive_directory_iterator rdi(_directory);
  for (const auto &path : rdi) {
    if (!std::filesystem::is_regular_file(path)) {
      continue;
    } else {
      // Read Date and Extension
      if (!ReadExif(path.path(), date, extension)) {
        continue;
      }
    }

    // Replace spaces with %20
    pathWithoutSpaces = "";
    for (const char &c : path.path().filename().string()) {
      if (c == ' ') {
        pathWithoutSpaces += "%20";
      } else {
        pathWithoutSpaces += c;
      }
    }

    if (!HasDate(_websiteDir / "library.html", date)) {
      ConfigureDateFolderURI(_dataDir, date);
      AddDateToLibraryFile(_websiteDir, date);
      AddDateCoverImage(_dataDir, date, path, extension);
      ConfigureCoverImageURI(_dataDir, _executableDir, date, extension);
      SetupDateFolderFile(_websiteDir, _dataDir, date);
    }
    AddImageToDateFolderFile(_websiteDir, _dataDir, date, pathWithoutSpaces);

    ConfigureImageURI(_executableDir, _dataDir, date, pathWithoutSpaces, path, extension);
    ConfigureDateFolderURI(_dataDir, date);
  }
}

int main(int argc, char *argv[]) {
  GArgs::Parser p("Image Library V2 - Reload", "V0.1");
  SetupArgsParser(p, argc, argv);

  int validate = Validation(p);
  if (validate >= 0) {
    return validate;
  }

  const std::filesystem::path EXECUTABLE_PATH = std::filesystem::canonical(argv[0]);
  const std::filesystem::path EXECUTABLE_DIR = EXECUTABLE_PATH.parent_path();
  const std::filesystem::path DATA_DIR = ((std::filesystem::path)p["filepath"][0]).parent_path();
  const std::filesystem::path WEBSITE_DIR = std::filesystem::canonical(DATA_DIR / WEBSITE_DIR_RELATIVE_FROM_DATA_DIR);

  // Definitions
  GParsing::HTTPRequest req;

  // Input
  req = ReadRequest(p["filepath"][0]);
  if (req.uri.find("/reload") == req.uri.npos) {
    WriteErrorResponse(p["filepath"][0]);
    return EXIT_FAILURE;
  }

  RefreshConfigurations(WEBSITE_DIR, EXECUTABLE_DIR, DATA_DIR, p["filepath"][0]);

  // Processing
  SetupLibraryFile(WEBSITE_DIR);
  ImageLibraryV2::ConfigFile c(EXECUTABLE_DIR / "include-directories.txt");

  // For each directory in configuration file
  for (const auto &directory : c.ReadPaths()) {
    if (!std::filesystem::exists(directory) ||
        std::filesystem::is_regular_file(directory) ||
        !directory.is_absolute()) {
      std::cout << "Error Reading Directory! Skipping path: " << directory
                << std::endl;
      continue;
    }

    std::cout << "Configuring: " << directory << std::endl;
    ConfigureImageDirectory(directory, EXECUTABLE_DIR, DATA_DIR, WEBSITE_DIR);
  }

  WriteResponse(p["filepath"][0]);

  return EXIT_SUCCESS;
}
