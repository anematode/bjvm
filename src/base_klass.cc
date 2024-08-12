//
// Created by Cowpox on 8/11/24.
//

#include "base_klass.h"
#include <cassert>

#include "class_instance.h"
#include "native/class.h"
#include "vm.h"

namespace bjvm {
BaseKlass::BaseKlass(std::string &&name, PlainKlass *superclass): m_name(name), m_superclass(superclass) {
  assert(superclass != nullptr || m_name == "java/lang/Object");
}

bool BaseKlass::IsPlainObjectKlass() {
  return false;
}

classfile::MethodInfo * BaseKlass::GetMethod(VM *vm, const std::string &method_name, const std::string &descriptor,
  bool set_vm_error) {
  return m_superclass ? m_superclass->GetMethod(vm, method_name, descriptor, set_vm_error) : nullptr;
}

Status BaseKlass::GetStatus() const {
  return m_status;
}

PlainKlass * BaseKlass::GetSuperclass() {
  return m_superclass;
}

HeapObject * BaseKlass::GetLinkageError() {
  return m_linkage_error;
}

const std::string & BaseKlass::GetName() const {
  return m_name;
}

bool BaseKlass::IsInterface() {
  return false;
}

void BaseKlass::SetStatus(Status status) {
  m_status = status;
}

native::Class *BaseKlass::GetClassInstance(VM* vm) {
  if (m_class_instance) return m_class_instance;
  return m_class_instance = reinterpret_cast<native::Class*>(vm->AllocatePlainObject(
    static_cast<PlainKlass*>(vm->LoadClass("java/lang/Class")), sizeof(native::Class) - sizeof(HeapObject) + 8));
}
} // bjvm