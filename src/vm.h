//
// Created by Cowpox on 8/11/24.
//

#ifndef VM_H
#define VM_H

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

#include "array_object.h"
#include "bytecode_interpreter.h"
#include "classfile.h"
#include "class_instance.h"
#include "execution_frame.h"
#include "object_array_klass.h"
#include "primitive_array_klass.h"
#include "utilities.h"

namespace bjvm {
class HeapObject;

struct VMOptions {
  /**
   * Virtual machine class path.
   *
   * The class path is formatted as a colon-delimited series of .class or .jar files, or paths ending in *, which
   * indicate that all .class and .jar files (recursively) present in that directory should be included.
   *
   * For now, the runtime classes (i.e., rt.jar) must be provided as part of the class path.
   */
  std::string m_classpath;

  /**
   * Main class to execute, e.g., "com.example.Main".
   */
  std::string m_main;
};

/**
 * Performance counters for the virtual machine.
 */
struct VMCounters {
  size_t m_class_bytes = 0;
};

using ArgumentType = std::variant<classfile::PrimitiveType, BaseKlass*>;
std::string ArgumentTypeToString(const ArgumentType& type);

struct ParsedMethodDescriptor {
  std::vector<ArgumentType> m_args;
  ArgumentType m_return;
};

bool ArgumentTypeIsFat(const ArgumentType& value);

class VM {
  /**
   * Classes found in the classpath -- eagerly loaded and parsed
   */
  std::unordered_map<std::string, classfile::Classfile*> m_classpath_classes;

  /**
   * List of all native methods, each of which takes an array of jvalue. Native methods are mangled by JNI rules.
   */
  std::unordered_map<std::string, std::function<jvalue(VM*, const std::vector<jvalue>&)>> m_native_methods;

  /**
   * Classes which have been successfully loaded by the bootstrap class loader
   */
  std::unordered_map<std::string, PlainKlass*> m_loaded_classes;

  /**
   * Currently propagating throwable (including if e.g. raised by a native method); null if no throwable is propagating.
   */
  HeapObject* m_current_throwable{};

  void AddClassFromClasspath(const std::string& class_name, std::vector<uint8_t>&& class_bytes);
  void LoadClasspathEntry(const std::string& entry);

public:
  VMCounters m_counters{};
  VMOptions m_options;

  std::vector<ExecutionFrame> m_frames{};

  HeapObject* GetCurrentThrowable() {
    return m_current_throwable;
  }

  bool ExceptionRaised() const {
    return m_current_throwable != nullptr;
  }

  void RegisterNativeMethod(const std::string& name, std::function<jvalue(VM*, const std::vector<jvalue>&)> method) {
    m_native_methods[name] = std::move(method);
  }

  PlainKlass* PrimordialKlass() {
    return static_cast<PlainKlass*>(LoadClass("java/lang/Object"));
  }

  ArrayKlass* LoadArrayClass(const std::string& klass) {
    std::cout << "Loading array class" << klass << std::endl;
    assert(klass.at(0) == '[');

    if (klass.size() == 1) throw std::runtime_error("Invalid array class");
    switch (klass.at(1)) {
      case 'B': case 'C': case 'D': case 'F': case 'I': case 'J': case 'S': case 'Z':
        return new PrimitiveArrayKlass(classfile::ParsePrimitiveType(klass.at(1)).value(), PrimordialKlass());
      case '[': {
        auto* nested = LoadArrayClass(klass.substr(1));
        if (!nested) return nullptr;
        return new ObjectArrayKlass(nested, PrimordialKlass());
      }
      case 'L': {
        // Check last character is ;
        if (klass.back() != ';') return nullptr;

        auto* nested = LoadClass(klass.substr(2, klass.size() - 3));
        return new ObjectArrayKlass(nested, PrimordialKlass());
      }
      default:
        break;
    }

    return nullptr;
  }

  /**
   * Allocate a plain object with all fields initially zero.
   *
   * TODO: GC, obviously, and sooo much more :(
   */
  HeapObject* AllocatePlainObject(PlainKlass* klass, int extra_bytes = 0) {
    char* memory = new char[klass->GetSizeBytes() + extra_bytes];
    return new (memory) HeapObject(klass);
  }

  /**
   * Allocate an array object with all elements initially zero.
   */
  ArrayObject* AllocateArrayObject(ArrayKlass* klass, int length) {
    if (length < 0)
      throw std::runtime_error("Negative array length");

    std::cout << "Allocating array of length " << length << '\n';
    size_t bytes = klass->GetMemberSizeBytes() * length + sizeof(ArrayObject);
    std::cout << "Real bytes: " << bytes <<'\n';

    char* memory = new char[bytes];
    return new (memory) ArrayObject(klass, length);
  }

  /**
   * Create an instance of the non-array class klass, using its constructor ctor, and the given arguments, which should
   * be an array with length equal to the number of arguments of the constructor.
   * @param klass The class to initialize.
   * @param ctor The constructor to invoke.
   * @param arguments Pointer to arguments list.
   * @return The initialized class, or null if an error occurred; current_throwable is set in the latter case.
   */
  HeapObject* CreateObject(PlainKlass* klass, classfile::MethodInfo* ctor, const jvalue* arguments) {
    HeapObject* alloc = AllocatePlainObject(klass);

    CallSpecialMethod(alloc, ctor, arguments);

    return alloc;
  }

  jvalue CallSpecialMethod(HeapObject* object, classfile::MethodInfo* method, const jvalue* arguments) {
    auto* descriptor = ResolveMethodDescriptor(method);
    auto& frame = m_frames.emplace_back(method->m_code->m_max_locals, method->m_code->m_max_stack);

    int j = 0;
    frame.m_locals[j++] = ToJValue(object);
    for (int i = 0; i < descriptor->m_args.size(); ++i) {
      frame.m_locals[j] = arguments[i];
      if (ArgumentTypeIsFat(descriptor->m_args[i])) ++j;
      ++j;
    }

    BytecodeInterpreter interpreter;
    return interpreter.execute(this, method);
  }

  ArrayObject* CreateArrayObject(ArrayKlass* klass, size_t length) {
    return nullptr;
  }

  ParsedMethodDescriptor* ResolveMethodDescriptor(classfile::MethodInfo* method);

  jvalue CallStaticMethod(classfile::MethodInfo* method, const jvalue* arguments) {
    auto* descriptor = ResolveMethodDescriptor(method);
    auto& frame = m_frames.emplace_back(method->m_code->m_max_locals, method->m_code->m_max_stack);

    int j = 0;
    for (int i = 0; i < descriptor->m_args.size(); ++i) {
      frame.m_locals[j] = arguments[i];
      if (ArgumentTypeIsFat(descriptor->m_args[i])) ++j;
      ++j;
    }

    BytecodeInterpreter interpreter;
    return interpreter.execute(this, method);
  }

  BaseKlass *LoadClass(const std::string &klass);

  void Start() {
    BaseKlass* main_class = LoadClass(m_options.m_main);

    if (!main_class) {
      throw std::runtime_error("Main class not found: " + m_options.m_main);
    }

    PlainKlass* system_class = static_cast<PlainKlass*>(InitKlass(LoadClass("java/lang/System")).first);
    assert(system_class != nullptr);

    auto* method = system_class->GetMethod(nullptr, "initializeSystemClass", "()V", false);
    if (!method) throw std::runtime_error("No system class method found");

    CallStaticMethod(method, nullptr);

    bool ok = main_class->Link(this);
    if (!ok) {
      throw std::runtime_error("Main class failed to link: " + m_options.m_main);
    }

    auto* main_method = main_class->GetMethod(nullptr, "main", "([Ljava/lang/String;)V", false);
    if (!main_method) throw std::runtime_error("No main method found");

    ArrayKlass* string_class = static_cast<ArrayKlass*>(LoadClass("[Ljava/lang/String;"));
    assert(string_class != nullptr);

    ArrayObject* args = AllocateArrayObject(string_class, 0);
    CallStaticMethod(main_method, &*std::vector { reinterpret_cast<jvalue>(args) }.begin());
  }

  std::pair<BaseKlass*, bool> InitKlass(BaseKlass* klass) {
    if (klass == nullptr) return { nullptr, false };

    if (klass->GetStatus() != Status::Initialised) {
      if (klass->GetStatus() == Status::Initialising) { // prevent recursive issues
        return { klass, false };
      }

      bool ok = klass->Link(this);
      if (!ok) throw std::runtime_error("Failed to link class " + klass->GetName());

      klass->SetStatus(Status::Initialising);

      InitKlass(klass->GetSuperclass());
      std::cout << "Initialising class " << klass->GetName() << std::endl;
      if (auto* method = klass->GetMethod(this, "<clinit>", "()V", false)) {
        CallStaticMethod(method, nullptr);
      }

      klass->SetStatus(Status::Initialised);

      return { klass, true };
    }

    return { klass, false };
  }

  std::pair<BaseKlass*, bool> GetOrInitKlass(EntryClass* entry) {
    if (entry->m_instance) {
      return InitKlass(entry->m_instance);
    }

    return InitKlass(entry->m_instance = LoadClass(entry->m_name->m_value));
  }

  jvalue GetStaticField(EntryFieldRef * entry) {
    auto [ klass, initialised_now ] = GetOrInitKlass(entry->m_struct);
    if (!entry->m_field_info)
      entry->m_field_info = klass->GetField(this, entry->m_name_and_type->m_name->m_value);
    assert(entry->m_field_info->IsStatic());
    return klass->GetStaticField(entry->m_field_info->m_static_or_instance_offset);
  }

  classfile::MethodInfo* GetStaticMethod(EntryMethodRef* entry) {
    auto [ klass, initialised_now ] = GetOrInitKlass(entry->m_struct);

    return klass->GetMethod(this, entry->m_name_and_type->m_name->m_value, entry->m_name_and_type->m_descriptor->m_value, false);
  }

  void PutStaticField(EntryFieldRef * entry, jvalue val) {
    auto [ klass, initialised_now ] = GetOrInitKlass(entry->m_struct);
    if (!entry->m_field_info)
      entry->m_field_info = klass->GetField(this, entry->m_name_and_type->m_name->m_value);
    assert(entry->m_field_info->IsStatic());
    klass->PutStaticField(entry->m_field_info->m_static_or_instance_offset, val);
  }

  HeapObject* InternString(const std::string & string);

  ArrayKlass* LoadArrayOfClass(EntryClass * entry) {
    const auto& name = entry->m_name->m_value;
    if (name.at(0) == '[')
      return LoadArrayClass("[" + name);
    return LoadArrayClass("[L" + name + ";");
  }

  classfile::MethodInfo* GetMethod(EntryMethodRef * entry) {
    auto [ klass, initialised_now ] = GetOrInitKlass(entry->m_struct);

    auto* method = klass->GetMethod(this, entry->m_name_and_type->m_name->m_value, entry->m_name_and_type->m_descriptor->m_value, false);
    return entry->m_method_info = method;
  }

  std::pair<bool, jvalue> CallNativeMethod(classfile::MethodInfo * method_info, const std::vector<jvalue>& args) {
    std::string as_jni = method_info->ToJNIString();

    std::cout << "JNI string: " << as_jni << '\n';

    const auto& iterator = m_native_methods.find(as_jni);
    if (iterator == m_native_methods.end()) {
      return { false, 0 };
    }

    return { true, iterator->second(this, args) };
  }

  PrimitiveArrayKlass * LoadPrimitiveArrayClass(classfile::PrimitiveType atype);

  explicit VM(VMOptions&& vm_options);
};

} // bjvm

#endif //VM_H
