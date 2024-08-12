//
// Created by Cowpox on 8/11/24.
//

#include "heap_object.h"

namespace bjvm {
int HeapObject::IdentityHashCode() const {
  return reinterpret_cast<int>(this);
}
} // bjvm