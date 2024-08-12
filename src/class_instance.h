//
// Created by Cowpox on 8/11/24.
//

#ifndef CLASS_INSTANCE_H
#define CLASS_INSTANCE_H
#include "classfile.h"
#include "constant_pool.h"

namespace bjvm {

struct VM;

enum class Status {
  Error,
  Loaded,
  Linked,
  Initialised,
};

class ClassInstance {
  classfile::Classfile* m_classfile = nullptr;

  Status m_status = Status::Loaded;

  std::unordered_map<std::string, classfile::MethodInfo*> m_static_methods;
  std::unordered_map<std::string, classfile::MethodInfo*> m_instance_methods;

  std::vector<uint64_t> m_static_fields;

  int m_instance_field_count = 0;

  // TODO add loaders

  [[nodiscard]] bool LinkSuperClass(VM* vm);

  [[nodiscard]] bool LinkInterfaces(VM* vm);

  [[nodiscard]] bool LinkFields(VM* vm);

  [[nodiscard]] bool LinkMethods(VM* vm);

  [[nodiscard]] bool LinkAttributes(VM* vm);

public:
  ClassInstance(classfile::Classfile* classfile) : m_classfile(classfile) {}

  ClassInstance(ClassInstance&&) = delete;
  ClassInstance(const ClassInstance&) = delete;

  Status GetStatus() const {
    return m_status;
  }

  [[nodiscard]] bool Link(VM* vm);

  [[nodiscard]] bool InitClass(VM* vm) {
    return true;
  }

  classfile::MethodInfo* FindStaticMethod(const char * str, const char * text) {
    return nullptr;
  }

  bool IsInterface() const {
    return (static_cast<int>(m_classfile->m_access_flags) & static_cast<int>(classfile::AccessFlags::ACC_INTERFACE)) != 0;
  }

  classfile::FieldInfo * GetFieldInfo(const std::string & string) {
    for (auto& field : m_classfile->m_fields) {
      if (m_classfile->m_cp.GetUtf8(field.m_name_index) == string) {
        return &field;
      }
    }

    return nullptr;
  }

  classfile::MethodInfo * GetMethodInfo(const std::string &method_name, const std::string &descriptor) {
    for (auto& method : m_classfile->m_methods) {
      if (m_classfile->m_cp.GetUtf8(method.m_name_index) == method_name &&
          m_classfile->m_cp.GetUtf8(method.m_descriptor_index) == descriptor) {
        return &method;
      }
    }
  }
};

} // bjvm

#endif //CLASS_INSTANCE_H
