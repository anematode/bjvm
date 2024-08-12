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

void VM::AddClassFromClasspath(const std::string &class_name, std::vector<uint8_t> &&class_bytes) {
  if (m_classpath_classes.count(class_name) == 0) {  // only first definition of a class is used
    BJVM_DEBUG("Adding class to classpath: " + class_name);

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

ClassInstance * VM::LoadClass(const std::string &klass) {
  using namespace classfile;
  try {
    if (!klass.empty() && klass[0] == '[') {
      return LoadArrayClass(klass);
    }

    if (m_loaded_classes.count(klass) == 0) {
      if (m_classpath_classes.count(klass) == 0) {
        throw std::runtime_error("Class not found: " + klass);
      }

      BJVM_DEBUG("Loading class " + klass);

      auto* cf = m_classpath_classes.at(klass);

      assert(cf->GetName() == klass);

      std::optional<std::string> superclass_name = cf->GetSuperclassName();

      ClassInstance* superclass = nullptr;

      if (superclass_name.has_value()) {
        superclass = LoadClass(superclass_name.value());
        if (!superclass) {
          throw std::runtime_error("Superclass not found: " + superclass_name.value());
        }

        if (superclass->IsInterface())
          throw std::runtime_error("IncompatibleClassChangeError Superclass is an interface: " + superclass_name.value());
      } else if (klass != PRIMORDIAL_OBJECT) {
        throw std::runtime_error("Class has no superclass: " + klass);
      }

      std::vector<ClassInstance*> superinterfaces;

      for (const auto& interface_name : cf->GetInterfaceNames()) {
        auto* interface = LoadClass(interface_name);
        if (!interface)
          throw std::runtime_error("Interface not found: " + interface_name);

        if (!interface->IsInterface())
          throw std::runtime_error("IncompatibleClassChangeError Interface is not an interface: " + interface_name);

        superinterfaces.push_back(interface);
      }

      auto* instance = new ClassInstance(cf);
      m_loaded_classes[klass] = instance;
      return instance;
    }

    return m_loaded_classes.at(klass);
  } catch (VerifyError& e) {
    // TODO
    throw;
  }
}

VM::VM(VMOptions&& vm_options) {
  this->m_options = vm_options;
  const auto& cp = vm_options.m_classpath;

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