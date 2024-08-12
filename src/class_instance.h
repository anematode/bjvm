//
// Created by Cowpox on 8/11/24.
//

#ifndef CLASS_INSTANCE_H
#define CLASS_INSTANCE_H
#include "base_klass.h"
#include "classfile.h"
#include <unordered_map>
#include "constant_pool.h"

namespace bjvm {
/**
 * Class object for a non-array class.
 */
class PlainKlass : public BaseKlass {
  classfile::Classfile* m_classfile = nullptr;

  std::vector<PlainKlass*> m_superinterfaces;

  std::unordered_map<std::string, classfile::MethodInfo*> _methods;
  std::unordered_map<std::string, classfile::MethodInfo*> m_instance_methods;

protected:
  int m_size_bytes = 0;

public:
  explicit PlainKlass(std::string&& name, classfile::Classfile* classfile,
    PlainKlass* superclass, std::vector<PlainKlass*> superinterfaces);

  [[nodiscard]] bool Link(VM* vm) override;

  bool IsPlainObjectKlass() override;

  classfile::MethodInfo* FindStaticMethod(const char * str, const char * text) {
    return nullptr;
  }

  bool IsInterface() const {
    return (static_cast<int>(m_classfile->m_access_flags) & static_cast<int>(classfile::AccessFlags::ACC_INTERFACE)) != 0;
  }

  classfile::FieldInfo *GetField(VM* vm, const std::string & string) override {
    for (auto& field : m_classfile->m_fields) {
      if (field.m_name == string) {
        return &field;
      }
    }

    return nullptr;
  }

  classfile::MethodInfo * GetMethod(VM* vm, const std::string &method_name, const std::string &descriptor, bool set_vm_error) override {
    for (auto& method : m_classfile->m_methods) {
      if (method.m_name == method_name &&
          method.m_descriptor == descriptor) {
        return &method;
      }
    }

    // Search in superclass
    if (m_superclass) {
      auto* method = m_superclass->GetMethod(vm, method_name, descriptor, set_vm_error);
      if (method) return method;
    }

    // Search in interfaces
    for (auto& interface : m_superinterfaces) {
      auto* method = interface->GetMethod(vm, method_name, descriptor, set_vm_error);
      if (method) return method;
    }

    return nullptr;
  }

  bool IsInterface() override;

  int GetSizeBytes() const;
};

} // bjvm

#endif //CLASS_INSTANCE_H
