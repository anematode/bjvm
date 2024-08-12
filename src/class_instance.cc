//
// Created by Cowpox on 8/11/24.
//

#include "class_instance.h"

#include "utilities.h"
#include "vm.h"

namespace bjvm {
bool ClassInstance::Link(VM *vm) {
  // TODO verification -- probably propagate pending VerifyError

  if (m_status != Status::Loaded) {
    throw std::runtime_error("Class not loaded, or already linked: " + m_classfile->GetName());
  }

  BJVM_DEBUG("Linking class: " + m_classfile->GetName());

  /** Preparation: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-5.html#jvms-5.4.2 */
  auto& constant_pool = m_classfile->m_cp;

  m_static_fields.resize(m_classfile->m_fields.size());  // zero initialisation is fine for all

  std::string my_name = m_classfile->GetName();

  /** 5.4.3: Resolution */
  for (int i = 1; i < constant_pool.Size(); ++i) {
    auto entry = constant_pool.GetAny(i);

    std::visit(overloaded { [&] (EntryClass& klass) {
        std::string name = constant_pool.GetUtf8(klass.m_name_index);
        if (name != my_name) {
          klass.m_instance = vm->LoadClass(name);
        } else {
          klass.m_instance = this;
        }
      },
      [] (auto && arg) {}
      }, *entry);
  }

  for (int i = 1; i < constant_pool.Size(); ++i) {
    auto entry = constant_pool.GetAny(i);

    std::visit(overloaded {
      [&] (EntryFieldRef field_ref) {
        auto klass = constant_pool.Get<EntryClass>(field_ref.struct_index);
        auto name_and_type = constant_pool.Get<EntryNameAndType>(field_ref.name_and_type_index);

        auto* result = klass->m_instance->GetFieldInfo(constant_pool.GetUtf8(name_and_type->name_index));
        if (!result) {
          throw std::runtime_error("Field not found: " + constant_pool.GetUtf8(name_and_type->name_index));
        }
        field_ref.m_field_info = result;
      },
      [&] (EntryMethodRef method_ref) {
        auto klass = constant_pool.Get<EntryClass>(method_ref.struct_index);
        auto name_and_type = constant_pool.Get<EntryNameAndType>(method_ref.name_and_type_index);

        std::string name = constant_pool.GetUtf8(name_and_type->name_index);
        std::string descriptor = constant_pool.GetUtf8(name_and_type->descriptor_index);

        auto* result = klass->m_instance->GetMethodInfo(name, descriptor);
        if (!result) {
          throw std::runtime_error("Method not found: " + constant_pool.GetUtf8(name_and_type->name_index));
        }
        method_ref.m_method_info = result;
      },
      [&] (auto&& arg) { }
    }, *entry);
  }

  return true;
}
} // bjvm