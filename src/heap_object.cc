//
// Created by Cowpox on 8/11/24.
//

#include "heap_object.h"

namespace bjvm {
HeapObject::HeapObject(BaseKlass *klass): m_class(klass) {}

int HeapObject::IdentityHashCode() const {
  int result;
  const HeapObject* s = this;
  memcpy(&result, &s, sizeof(int));
  return result;
}
} // bjvm