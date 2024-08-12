//
// Created by Cowpox on 8/11/24.
//

#ifndef PRIMITIVE_ARRAY_OBJECT_H
#define PRIMITIVE_ARRAY_OBJECT_H
#include "array_klass.h"
#include "heap_object.h"

namespace bjvm {

class ArrayObject : public HeapObject {
  int m_length{};

  /**
   * Flexible array member containing the data. Note that this is 16 bytes into the struct, so alignment shouldn't be
   * an issue.
   */
  char m_array_data[];

public:
  ArrayObject(ArrayKlass* klass, int length) : HeapObject(klass), m_length(length) {
    std::memset(m_array_data, 0, length * klass->GetMemberSizeBytes());
  }

  ArrayKlass* GetArrayKlass() {
    return static_cast<ArrayKlass*>(m_class);
  }

  template <typename T>
  T* GetData() {
    std::cout << "sizeof(T): " << sizeof(T) << std::endl;
    std::cout << "GetArrayKlass()->GetMemberSizeBytes(): " << GetArrayKlass()->GetMemberSizeBytes() << std::endl;
    assert(sizeof(T) == GetArrayKlass()->GetMemberSizeBytes());
    return reinterpret_cast<T*>(m_array_data);
  }

  int GetLength() const;
};

} // bjvm

#endif //PRIMITIVE_ARRAY_OBJECT_H
