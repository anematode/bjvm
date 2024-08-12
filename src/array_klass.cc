//
// Created by Cowpox on 8/11/24.
//

#include "array_klass.h"

#include "class_instance.h"

namespace bjvm {
ArrayKlass::ArrayKlass(std::string &&name, PlainKlass* superclass): BaseKlass(std::move(name), superclass), m_length_field() {
  assert(superclass->GetName() == "java/lang/Object");
}

classfile::FieldInfo * ArrayKlass::GetField(VM *vm, const std::string &field_name) {
  return field_name == "length" ? &m_length_field : m_superclass->GetField(vm, field_name);
}
} // bjvm