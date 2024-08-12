//
// Created by Cowpox on 8/11/24.
//

#include "vm.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#else
#endif
#include <filesystem>
#include <fstream>

namespace bjvm {

const char* PRIMORDIAL_OBJECT = "java/lang/Object";

std::string ArgumentTypeToString(const ArgumentType &type) {
  return std::visit(overloaded {
    [] (classfile::PrimitiveType ty) -> std::string {
      switch (ty) {
        case classfile::PrimitiveType::boolean: return "boolean";
        case classfile::PrimitiveType::byte: return "byte";
        case classfile::PrimitiveType::char_: return "char";
        case classfile::PrimitiveType::short_: return "short";
        case classfile::PrimitiveType::int_: return "int";
        case classfile::PrimitiveType::long_: return "long";
        case classfile::PrimitiveType::float_: return "float";
        case classfile::PrimitiveType::double_: return "double";
        case classfile::PrimitiveType::void_: return "void";
        default:
          BJVM_UNREACHABLE();
      }
    },
    [] (const BaseKlass* klass) {
      return klass->GetName();
    }
  }, type);
}

bool ArgumentTypeIsFat(const ArgumentType &value) {
  using namespace classfile;
  if (!std::holds_alternative<PrimitiveType>(value)) return false;
  auto prim = std::get<PrimitiveType>(value);
  return prim == PrimitiveType::long_ || prim == PrimitiveType::double_;
}

void VM::AddClassFromClasspath(const std::string &class_name, std::vector<uint8_t> &&class_bytes) {
  if (m_classpath_classes.count(class_name) == 0) {  // only first definition of a class is used
    //BJVM_DEBUG("Adding class to classpath: " + class_name);

    ByteReader reader { class_bytes };

    auto* cf = new classfile::Classfile(classfile::Classfile::parse(&reader));
    m_classpath_classes[cf->GetName()] = cf;
    m_counters.m_class_bytes += class_bytes.size();
  }
}

void VM::LoadClasspathEntry(const std::string &entry) {
  if (HasSuffix(entry, ".class")) {
    AddClassFromClasspath(entry, ReadFile(entry));
  } else if (HasSuffix(entry, ".jar")) {
    std::vector<uint8_t> compressed_bytes = ReadFile(entry);

    BJVM_DEBUG("Skipping JAR file: " + entry);
  } else {
    bool recursive = HasSuffix(entry, "/*");
    std::string use_entry = entry;
    if (recursive)
      use_entry = entry.substr(0, entry.size() - 2);

    auto list = ListDirectory(use_entry, recursive);
    for (const auto& subentry : list) {
      if (HasSuffix(subentry, ".class") || HasSuffix(subentry, ".jar")) {
        LoadClasspathEntry(subentry);
      }
    }
  }
}

ParsedMethodDescriptor * VM::ResolveMethodDescriptor(classfile::MethodInfo *method) {
  auto* existing = method->m_parsed_descriptor;
  if (existing != nullptr) return existing;

  const std::string& descriptor = method->m_descriptor;
  std::vector<ArgumentType> args;

  // e.g. (Ljava/lang/String;I)V takes in a string and an int and returns void
  assert(descriptor[0] == '(');
  size_t i = 1;

  auto ParseEntry = [&] () -> ArgumentType {
    int j = i;
    while (descriptor.at(j) == '[') ++j;

    bool is_array = i != j;

    if (descriptor.at(j) == 'L') {
      size_t end = descriptor.find(';', i);
      if (end == std::string::npos) throw std::runtime_error("Invalid descriptor");

      std::string klass_name = descriptor.substr(i + !is_array, end - i - !is_array /* include or not include ; */);
      i = end + 1;
      return LoadClass(klass_name);
    } else if (is_array) {
      std::string array_klass_name = descriptor.substr(i, j - i + 1);
      i = j + 1;
      return LoadClass(array_klass_name);
    }

    return classfile::ParsePrimitiveType(descriptor[i++]).value();
  };

  while (descriptor.at(i) != ')') {
    args.push_back(ParseEntry());
  }

  ++i;

  auto return_type = ParseEntry();
  auto* parsed = new ParsedMethodDescriptor {
    .m_args = std::move(args),
    .m_return = return_type
  };
  return method->m_parsed_descriptor = parsed;
}

BaseKlass *VM::LoadClass(const std::string &klass) {
  std::cout << klass << '\n';
  using namespace classfile;
  try {
    if (m_loaded_classes.count(klass) == 0) {
  std::cout << "Hi4" << '\n';
      BJVM_DEBUG("Loading class " + klass);
  std::cout << "Hi2" << '\n';

      if (!klass.empty() && klass[0] == '[') {
        return LoadArrayClass(klass);
      }

      if (m_classpath_classes.count(klass) == 0) {
        throw std::runtime_error("Class not found: " + klass);
      }

      auto* cf = m_classpath_classes.at(klass);

      assert(cf->GetName() == klass);

      std::optional<std::string> superclass_name = cf->GetSuperclassName();
      PlainKlass* superclass = nullptr;

      if (superclass_name.has_value()) {
        superclass = static_cast<PlainKlass*>(LoadClass(superclass_name.value()));
        if (!superclass) {
          throw std::runtime_error("Superclass not found: " + superclass_name.value());
        }

        if (superclass->IsInterface())
          throw std::runtime_error("IncompatibleClassChangeError Superclass is an interface: " + superclass_name.value());
      } else if (klass != PRIMORDIAL_OBJECT) {
        throw std::runtime_error("Class has no superclass: " + klass);
      }

      std::vector<PlainKlass*> superinterfaces;

      for (const auto& interface_name : cf->GetInterfaceNames()) {
        auto* interface = LoadClass(interface_name);
        if (!interface)
          throw std::runtime_error("Interface not found: " + interface_name);

        if (!interface->IsInterface())
          throw std::runtime_error("IncompatibleClassChangeError Interface is not an interface: " + interface_name);

        superinterfaces.push_back(dynamic_cast<PlainKlass*>(interface));
      }

      auto* instance = new PlainKlass(std::string(cf->GetName()), cf, superclass, std::move(superinterfaces));
      m_loaded_classes[klass] = instance;
      cf->SetKlassPointer(instance);
      return instance;
    }

    std::cout << "Already loaded\n";

    return m_loaded_classes.at(klass);
  } catch (VerifyError& e) {
    // TODO
    throw;
  }
}

HeapObject* VM::InternString(const std::string &string) { // TODO: Actually intern them lmao
  PlainKlass* string_klass = static_cast<PlainKlass*>(InitKlass(LoadClass("java/lang/String")).first);
  assert(string_klass);

  PrimitiveArrayKlass* char_array = static_cast<PrimitiveArrayKlass*>(InitKlass(LoadClass("[Z")).first);
  assert(char_array);

  std::cout << "Here2\n";
  ArrayObject* obj = AllocateArrayObject(char_array, string.size());
  assert(obj);

  std::memcpy(obj->GetData<char>(), string.data(), string.size());

  HeapObject* string_obj = AllocatePlainObject(string_klass);
  string_obj->PutField(0, ToJValue(obj));

  return string_obj;
}

PrimitiveArrayKlass * VM::LoadPrimitiveArrayClass(classfile::PrimitiveType atype) {
  using P = classfile::PrimitiveType;
  const char* c;
  switch (atype) {
    case P::boolean: c = "[Z"; break;
    case P::byte: c = "[B"; break;
    case P::char_: c = "[C"; break;
    case P::short_: c = "[S"; break;
    case P::int_: c = "[I"; break;
    case P::long_: c = "[J"; break;
    case P::float_: c = "[F"; break;
    case P::double_: c = "[D"; break;
    default: BJVM_UNREACHABLE();
  }

  return static_cast<PrimitiveArrayKlass*>(LoadArrayClass(c));
}

VM::VM(VMOptions&& vm_options) {
  this->m_options = vm_options;
  const auto& cp = vm_options.m_classpath;

  RegisterNativeMethod("Java_java_lang_System_registerNatives", [](VM* vm, const std::vector<jvalue>&) {
    PlainKlass* system_klass = static_cast<PlainKlass*>(vm->LoadClass("java/lang/System"));

    //system_klass->PutStaticField(1, system_out);

    return 0;
  });

  // Split by :
  size_t start = 0;
  size_t end = cp.find(':');

  while (end != std::string::npos) {
    LoadClasspathEntry(cp.substr(start, end - start));
    start = end + 1;
    end = cp.find(':', start);
  }

  if (start < cp.size()) {
    LoadClasspathEntry(cp.substr(start));
  }
}
} // bjvm