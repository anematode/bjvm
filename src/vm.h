//
// Created by Cowpox on 8/11/24.
//

#ifndef VM_H
#define VM_H

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "classfile.h"
#include "class_instance.h"
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

class VM {
  /**
   * Classes found in the classpath -- eagerly loaded and parsed
   */
  std::unordered_map<std::string, classfile::Classfile*> m_classpath_classes;

  /**
   * Classes which have been successfully loaded by the bootstrap class loader
   */
  std::unordered_map<std::string, ClassInstance*> m_loaded_classes;

  /**
   * Currently propagating throwable (including if e.g. raised by a native method); null if no throwable is propagating.
   */
  HeapObject* m_current_throwable{};

  void AddClassFromClasspath(const std::string& class_name, std::vector<uint8_t>&& class_bytes);

  void LoadClasspathEntry(const std::string& entry);

public:
  VMCounters m_counters{};
  VMOptions m_options;

  HeapObject* GetCurrentThrowable() {
    return m_current_throwable;
  }

  bool ExceptionRaised() const {
    return m_current_throwable != nullptr;
  }

  ClassInstance* LoadArrayClass(const std::string& klass) {
    assert(klass.at(0) == '[');

    throw std::runtime_error("Unimplemented");

    return nullptr;
  }

  ClassInstance* LoadClass(const std::string& klass);

  void Start() {
    ClassInstance* main_class = LoadClass(m_options.m_main);

    if (!main_class) {
      throw std::runtime_error("Main class not found: " + m_options.m_main);
    }

    bool ok = main_class->Link(this);

    auto* main_method = main_class->FindStaticMethod("main", "([Ljava/lang/String;)V");
  }

  explicit VM(VMOptions&& vm_options);
};

} // bjvm

#endif //VM_H
