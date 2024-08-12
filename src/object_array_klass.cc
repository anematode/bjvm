//
// Created by Cowpox on 8/11/24.
//

#include "object_array_klass.h"

namespace bjvm {

// java/lang/Object -> [Ljava/lang/Object;,  [B -> [[B
std::string MakeArray(BaseKlass *element) {
  if (element->IsPlainObjectKlass()) {
    return "[L" + element->GetName() + ";";
  }
  return "[" + element->GetName();
}

ObjectArrayKlass::ObjectArrayKlass(BaseKlass *element, PlainKlass *superclass): ArrayKlass(MakeArray(element), superclass), m_element(element) {}

bool ObjectArrayKlass::Link(VM *vm) {
  return m_element->Link(vm);
}

size_t ObjectArrayKlass::GetMemberSizeBytes() {
  return sizeof(void*);
}

BaseKlass * ObjectArrayKlass::GetElementType() {
  return m_element;
}
} // bjvm