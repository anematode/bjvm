#include <iostream>
#include <emscripten.h>

#include <cstdlib>

#include "classfile.h"

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

int main() {
  using namespace bjvm;

  std::vector<uint8_t> bytes;
  std::string file = "test/jre8/com/sun/beans/decoder/DocumentHandler.class";

  ByteReader reader { ReadFile(file) };
  auto cf = classfile::Classfile::parse(&reader);
  std::cout << cf.ToString() << '\n';

  return 0;
}
