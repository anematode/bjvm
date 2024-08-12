//
// Created by Cowpox on 8/11/24.
//

#include "array_object.h"

namespace bjvm {
int ArrayObject::GetLength() const {
  return m_length;
}
} // bjvm