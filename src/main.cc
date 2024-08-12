#include <iostream>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <cstdlib>

#include "classfile.h"
#include "utilities.h"
#include "vm.h"

int main() {
  using namespace bjvm;

  std::vector<uint8_t> bytes;
  std::string file = "test/jre8/com/oracle/net/Sdp.class";

  ByteReader reader { ReadFile(file) };
  try {
    auto cf = classfile::Classfile::parse(&reader);
    std::cout << cf.ToString() << '\n';

    VM vm {
      VMOptions {
        .m_classpath = "test/jre8/*:test",
        .m_main = "Main"
      }
    };

    vm.Start();
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
  }


  return 0;
}
