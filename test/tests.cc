//
// Created by Cowpox on 8/10/24.
//
#include <iostream>
#include <emscripten.h>
#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include "../src/byte_reader.h"
#include "../src/classfile.h"
#include "../src/utilities.h"

bool EndsWith(const std::string& s, const std::string& suffix) {
  if (s.size() < suffix.size()) {
    return false;
  }
  return s.substr(s.size() - suffix.size()) == suffix;
}

TEST_CASE("Test classfile parsing") {
  using namespace bjvm;

  // list all java files in the jre8 directory
  auto files = ListDirectory("jre8", true);

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