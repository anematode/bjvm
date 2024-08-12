//
// Created by Cowpox on 8/11/24.
//

#ifndef HEAP_OBJECT_H
#define HEAP_OBJECT_H

#include <cstdint>

namespace bjvm {
/**
 * Base class for all heap objects.
 *
 * Following the (64-bit) HotSpot JVM, the first 64 bits of a heap object is the "mark word", which is used for garbage
 * collection and other important functionality. The next 32 bits (32 because WebAssembly is 32-bit) is a pointer to
 * a class object.
 *
 * In a WASM64 scenario this mill have to be changed (unless we do something akin to compressed oops...)
 */
class HeapObject {
  uint32_t m_mark_word[2] = { 0, 0 };
  void* m_class = nullptr;

  int IdentityHashCode() const;
};

} // bjvm

#endif //HEAP_OBJECT_H
