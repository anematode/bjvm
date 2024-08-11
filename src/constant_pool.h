//
// Created by Cowpox on 8/9/24.
//

#ifndef BROWSER_JVM_CONSTANT_POOL_H
#define BROWSER_JVM_CONSTANT_POOL_H

#include <vector>
#include "byte_reader.h"

namespace bjvm {

struct ConstantPool;

struct EntryUtf8 {
  std::string m_value;

  std::string ToString(const ConstantPool* _cp) const;
};

struct EntryInteger {
  int32_t m_value;

  std::string ToString(const ConstantPool* _cp) const;
};

struct EntryFloat {
  float m_value;

  std::string ToString(const ConstantPool* _cp) const;
};

struct EntryLong {
  int64_t m_value;

  std::string ToString(const ConstantPool* _cp) const;
};

struct EntryDouble {
  double value;

  std::string ToString(const ConstantPool* _cp) const;
};

struct EntryClass {
  uint16_t name_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryString {
  uint16_t string_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryFieldRef {
  uint16_t struct_index;
  uint16_t name_and_type_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryMethodRef {
  uint16_t struct_index;
  uint16_t name_and_type_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryInterfaceMethodRef {
  uint16_t struct_index;
  uint16_t name_and_type_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryNameAndType {
  uint16_t name_index;
  uint16_t descriptor_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryMethodHandle {
  uint8_t reference_kind;
  uint16_t reference_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryMethodType {
  uint16_t descriptor_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryInvokeDynamic {
  uint16_t bootstrap_method_attr_index;
  uint16_t name_and_type_index;

  std::string ToString(const ConstantPool* cp) const;
};

struct EntryInvalid {
  std::string ToString(const ConstantPool* _cp) const;
};

/** Single entry in the constant pool. */
using ConstantPoolEntry = std::variant<EntryInvalid, EntryUtf8, EntryInteger, EntryFloat, EntryLong, EntryDouble, EntryClass,
  EntryString, EntryFieldRef, EntryMethodRef, EntryInterfaceMethodRef, EntryNameAndType, EntryMethodHandle,
  EntryMethodType, EntryInvokeDynamic>;

class ConstantPool {
  // defaults to EntryInvalid, should not have its contents move
  std::vector<ConstantPoolEntry> m_entries;

  template <typename T>
  void Put(int index, T entry) {
    m_entries.at(index) = entry;
  }

public:
  ConstantPool(int size) : m_entries(size) {}

  /** Get the entry of type TEntry at the given index. */
  template <typename TEntry>
  TEntry* GetUnchecked(int index) {
#ifdef DEBUG
    return Get(index);
#else
    return std::get_if<TEntry>(&m_entries[index]);
#endif
  }

  template <typename TEntry>
  const TEntry* GetUnchecked(int index) const {
#ifdef DEBUG
    return Get(index);
#else
    return std::get_if<TEntry>(&m_entries[index]);
#endif
  }

  template <typename TEntry>
  bool Has(int index) const {
    return index < m_entries.size() && index >= 0 && GetUnchecked<TEntry>(index);
  }

  /** Get a constant pool entry by index. */
  ConstantPoolEntry* GetAny(int index);
  const ConstantPoolEntry* GetAny(int index) const;

  /** Get a constant pool entry of a specific type by index. */
  template <typename TEntry>
  TEntry* Get(int index) {
    if (index == 0 || index >= m_entries.size())
      throw std::runtime_error("Invalid constant pool index");
    auto p = GetUnchecked<TEntry>(index);
    if (!p)
      throw std::runtime_error("Constant pool entry type mismatch");
    return p;
  }

  template <typename TEntry>
  const TEntry* Get(int index) const {
    if (index == 0 || index >= m_entries.size())
      throw std::runtime_error("Invalid constant pool index");
    auto p = GetUnchecked<TEntry>(index);
    if (!p)
      throw std::runtime_error("Constant pool entry type mismatch");
    return p;
  }

  /** Get the UTF-8 entry at the given index. */
  const std::string& GetUtf8(int index) const {
    return Get<EntryUtf8>(index)->m_value;
  }

  std::string ToString() const;

  /** Parse a constant pool from the given reader. */
  static ConstantPool parse(ByteReader *reader);
};

} // bjvm

#endif //BROWSER_JVM_CONSTANT_POOL_H
