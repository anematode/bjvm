//
// Created by Cowpox on 8/12/24.
//

#ifndef CLASS_H
#define CLASS_H

#include "../heap_object.h"

namespace bjvm {
namespace native {

class Class : public HeapObject {
  volatile uint32_t m_unknown_fields[30];
  BaseKlass* m_klass;
public:
  explicit Class(BaseKlass* klass, BaseKlass* primordial_object) : HeapObject(primordial_object), m_klass(klass) {}
};

} // native
} // bjvm

#endif //CLASS_H
