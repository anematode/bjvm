//
// Created by Cowpox on 8/11/24.
//

#ifndef ABSTRACT_CLASS_INSTANCE_H
#define ABSTRACT_CLASS_INSTANCE_H

#include <string>

#include "utilities.h"

namespace bjvm {

/** forward declarations */
class HeapObject;
class VM;
namespace classfile {
struct MethodInfo;
struct FieldInfo;
}
namespace native {
class Class;
}
class PlainKlass;

/**
 * Status of a reference type relative to the JVM.
 */
enum class Status {
 Error,
 Loaded,
 Linked,
 Initialising,
 Initialised,
};


/**
 * Abstract base class for all reference klass objects (plain reference types, reference array types, primitive array
 * types).
 *
 * TODO: Add class loader field
 */
class BaseKlass {
 std::string m_name;

protected:
 // Null iff this is java/lang/Object
 PlainKlass* m_superclass;
 std::vector<jvalue> m_static_fields;
 Status m_status = Status::Loaded;

 native::Class* m_class_instance = nullptr;

 /**
  * Pending linkage error if the instance failed to load for some reason (e.g. VerifyError, NoClassDefFoundError).
  * The JVM specifies that the linkage error object must be identical if it is retrieved multiple times.
  */
 HeapObject* m_linkage_error = nullptr;

public:
 explicit BaseKlass(std::string&& name, PlainKlass* superclass);

 // Make immovable
 BaseKlass(BaseKlass&&) = delete;
 BaseKlass(const BaseKlass&) = delete;

 virtual ~BaseKlass() = default;

 virtual bool IsPlainObjectKlass();

 /**
  * Link this class to the VM.
  * @param vm The virtual machine.
  * @return True if linking succeeded. If linking fails, GetLinkageError() can be used to get the error.
  */
 [[nodiscard]] virtual bool Link(VM* vm) = 0;

 /**
  * Get the method with the given name and descriptor. Returns null if the method is not found, and optionally sets
  * the VM's error to MethodNotFoundException.
  */
 virtual classfile::MethodInfo* GetMethod(
  VM* vm, const std::string& method_name, const std::string& descriptor, bool set_vm_error
 );

 /**
  * Get the field with the given name. Returns null if the field is not found.
  */
 virtual classfile::FieldInfo *GetField(VM* vm, const std::string& field_name) = 0;

 /**
  * The stage of the reference type (whether it's loaded, linked, initialized, or errored).
  */
 Status GetStatus() const;

 /**
  * Get the superclass of this class.
  */
 PlainKlass* GetSuperclass();

 /**
  * The linkage error associated with this reference type, or nullptr if no such error exists.
  */
 HeapObject* GetLinkageError();

 /**
  * Reference name, e.g. [B for byte[], java/lang/String for String, and [[Ljava/lang/String; for String[][].
  * TODO: Intern this for reflection purposes
  */
 const std::string& GetName() const;

 virtual bool IsInterface();

 jvalue GetStaticField(int offset) {
  return BJVM_CHECKED_VEC(m_static_fields, offset);
 }

 void SetStatus(Status status);

 void PutStaticField(int offset, jvalue val) {
  m_static_fields.at(offset) = val;
 }

 native::Class* GetClassInstance(VM* vm);
};

} // bjvm

#endif //ABSTRACT_CLASS_INSTANCE_H
