//
// Created by Cowpox on 8/11/24.
//

#include "utilities.h"

#include <filesystem>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#include <fstream>
#endif

namespace bjvm {
bool HasSuffix(std::string_view str, std::string_view suffix) {
  // Credit: https://stackoverflow.com/a/20446239/13458117
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<uint8_t> ReadFile(const std::string& file) {
#ifdef EMSCRIPTEN
  void* length_and_data = EM_ASM_PTR({
    const fs = require('fs');
    const buffer = fs.readFileSync(UTF8ToString($0));
    const length = buffer.length;

    const result = _malloc(length + 4);
    Module.HEAPU32[result >> 2] = length;
    Module.HEAPU8.set(buffer, result + 4);

    return result;
  }, file.c_str());

  uint32_t length = *reinterpret_cast<uint32_t*>(length_and_data);
  uint8_t* data = reinterpret_cast<uint8_t*>(length_and_data) + 4;

  std::vector result(data, data + length);
  free(length_and_data);
  return result;
#else // !EMSCRIPTEN
  std::vector<uint8_t> result;
  if (std::filesystem::exists(file)) {
    std::ifstream ifs(file, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    result.resize(pos);
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(result.data()), pos);
  } else {
    throw std::runtime_error("Classpath file not found: " + file);
  }
  return result;
#endif
}

std::vector<std::string> ListDirectory(const std::string& path, bool recursive) {
#ifdef EMSCRIPTEN
  void* length_and_data = EM_ASM_PTR({
     // Credit: https://stackoverflow.com/a/5827895
     const fs = require('fs');
     const path = require('path');
     function *walkSync(dir, recursive) {
       const files = fs.readdirSync(dir, { withFileTypes: true });
       for (const file of files) {
         if (file.isDirectory() && recursive) {
           yield* walkSync(path.join(dir, file.name), recursive);
         } else {
           yield path.join(dir, file.name);
         }
       }
     }

     var s = "";
     for (const filePath of walkSync(UTF8ToString($0), $1)) s += filePath + "\n";

      const length = s.length;
      const result = _malloc(length + 4);
      Module.HEAPU32[result >> 2] = length;
      Module.HEAPU8.set(new TextEncoder().encode(s), result + 4);

      return result;
  }, path.c_str(), recursive);


  uint32_t length = *reinterpret_cast<uint32_t*>(length_and_data);
  uint8_t* data = reinterpret_cast<uint8_t*>(length_and_data) + 4;

  std::string s(data, data + length);
  free(length_and_data);

  std::vector<std::string> result;
  size_t start = 0;

  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == '\n') {
      result.push_back(s.substr(start, i - start));
      start = i + 1;
    }
  }

  return result;
#else // !EMSCRIPTEN
  using namespace std::filesystem;

  // Recursively list files (TODO: fix recursive = false)
  std::vector<std::string> result;

  for (const auto& entry : recursive_directory_iterator(path)) {
    if (entry.is_regular_file()) {
      result.push_back(entry.path().string());
    }
  }

  return result;
#endif
}
} //bjvm