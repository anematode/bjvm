//
// Created by Cowpox on 8/11/24.
//

#ifndef HEAP_OBJECT_H
#define HEAP_OBJECT_H

#include <cstdint>

#include "base_klass.h"

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
protected:
  BaseKlass* m_class;

public:
  explicit HeapObject(BaseKlass* klass);

  int IdentityHashCode() const;

  int IsPlainObject() const {
    return m_class->IsPlainObjectKlass();
  }

  BaseKlass* GetKlass() {
    return m_class;
  }

  jvalue LoadField(int m_static_or_instance_offset) {
    return *(reinterpret_cast<jvalue*>(this) + m_static_or_instance_offset + 2);  // grimace
  }

  void PutField(int m_static_or_instance_offset, jvalue value) {
    *(reinterpret_cast<jvalue*>(this) + m_static_or_instance_offset + 2) = value;  // grimace
  }
};

} // bjvm

#endif //HEAP_OBJECT_H
