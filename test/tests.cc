//
// Created by Cowpox on 8/10/24.
//
#include <iostream>
#include <emscripten.h>
#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include "../src/byte_reader.h"
#include "../src/classfile.h"

std::vector<std::string> ListDirectory(const std::string& path) {
  void* length_and_data = EM_ASM_PTR({
     // Credit: https://stackoverflow.com/a/5827895
     const fs = require('fs');
     const path = require('path');
     function *walkSync(dir) {
       const files = fs.readdirSync(dir, { withFileTypes: true });
       for (const file of files) {
         if (file.isDirectory()) {
           yield* walkSync(path.join(dir, file.name));
         } else {
           yield path.join(dir, file.name);
         }
       }
     }

     var s = "";
     for (const filePath of walkSync(UTF8ToString($0))) s += filePath + "\n";

      const length = s.length;
      const result = _malloc(length + 4);
      Module.HEAPU32[result >> 2] = length;
      Module.HEAPU8.set(new TextEncoder().encode(s), result + 4);

      return result;
  }, path.c_str());


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
}

std::vector<uint8_t> ReadFile(const std::string& file) {
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

  std::vector<uint8_t> result(data, data + length);
  free(length_and_data);
  return result;
}

bool EndsWith(const std::string& s, const std::string& suffix) {
  if (s.size() < suffix.size()) {
    return false;
  }
  return s.substr(s.size() - suffix.size()) == suffix;
}

TEST_CASE("Test classfile parsing") {
  using namespace bjvm;

  // list all java files in the jre8 directory
  auto files = ListDirectory("jre8");

  long total_millis = 0;
  std::string longest;

  for (const auto& file : files) {
    if (!EndsWith(file, ".class")) {
      continue;
    }
    ByteReader reader { ReadFile(file) };

    long start = emscripten_get_now();

    std::cout << "Reading " << file << "\n";
    auto cf = classfile::Classfile::parse(&reader);
    std::string s = cf.ToString();
    if (s.size() > longest.size()) longest = std::move(s);

    total_millis += emscripten_get_now() - start;
  }

  std::cout << "Total time: " << total_millis << "ms\n";
  std::cout << "Longest: " << longest << "\n";
}