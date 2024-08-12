//
// Created by Cowpox on 8/9/24.
//

#include "constant_pool.h"
#include <iostream>

namespace bjvm {

std::string EntryClass::ToString(const ConstantPool *cp) const {
  return "Class " + (!cp ? std::to_string(m_name_index) : cp->Get<EntryUtf8>(m_name_index)->ToString(cp));
}

std::string EntryString::ToString(const ConstantPool *cp) const {
  return "String " + (!cp ? std::to_string(string_index) : '"' + cp->Get<EntryUtf8>(string_index)->ToString(cp)) + '"';
}

std::string EntryFieldRef::ToString(const ConstantPool *cp) const {
  return "FieldRef " + (!cp ? std::to_string(struct_index) : cp->Get<EntryClass>(struct_index)->ToString(cp)) + " "
         + (!cp ? std::to_string(name_and_type_index) : cp->Get<EntryNameAndType>(name_and_type_index)->ToString(cp));
}

std::string EntryMethodRef::ToString(const ConstantPool *cp) const {
  return "MethodRef " + (!cp ? std::to_string(struct_index) : cp->Get<EntryClass>(struct_index)->ToString(cp)) + " "
         + (!cp ? std::to_string(name_and_type_index) : cp->Get<EntryNameAndType>(name_and_type_index)->ToString(cp));
}

std::string EntryInterfaceMethodRef::ToString(const ConstantPool *cp) const {
  return "InterfaceMethodRef " + (!cp ? std::to_string(struct_index) : cp->Get<EntryClass>(struct_index)->ToString(cp))
         + " " + (!cp ? std::to_string(name_and_type_index) : cp->Get<EntryNameAndType>(name_and_type_index)->ToString(cp));
}

std::string EntryNameAndType::ToString(const ConstantPool *cp) const {
  return "NameAndType " + (!cp ? std::to_string(name_index) : cp->Get<EntryUtf8>(name_index)->ToString(cp)) + " "
         + (!cp ? std::to_string(descriptor_index) : cp->Get<EntryUtf8>(descriptor_index)->ToString(cp));
}

std::string EntryMethodHandle::ToString(const ConstantPool *cp) const {
  return "MethodHandle " + std::to_string(reference_kind) + " " +
    (!cp ? std::to_string(reference_index) : cp->Get<EntryFieldRef>(reference_index)->ToString(cp));
}

std::string EntryMethodType::ToString(const ConstantPool *cp) const {
  return "MethodType " + (!cp ? std::to_string(descriptor_index) : cp->Get<EntryUtf8>(descriptor_index)->ToString(cp));
}

std::string EntryInvokeDynamic::ToString(const ConstantPool *cp) const {
  return "InvokeDynamic " + std::to_string(bootstrap_method_attr_index) + " " +
    (!cp ? std::to_string(name_and_type_index) : cp->Get<EntryNameAndType>(name_and_type_index)->ToString(cp));
}

std::string EntryInteger::ToString(const ConstantPool *_cp) const {
  return std::to_string(m_value);
}

std::string EntryFloat::ToString(const ConstantPool *_cp) const {
  return std::to_string(m_value) + "f";
}

std::string EntryLong::ToString(const ConstantPool *_cp) const {
  return std::to_string(m_value) + "l";
}

std::string EntryDouble::ToString(const ConstantPool *_cp) const {
  return std::to_string(value) + "d";
}

std::string EntryUtf8::ToString(const ConstantPool *_cp) const {
  return {m_value.begin(), m_value.end()}; // TODO handle weird Java UTF-8
}

enum RawTag {
  Utf8 = 1,
  Integer = 3,
  Float = 4,
  Long = 5,
  Double = 6,
  Class = 7,
  String = 8,
  FieldRef = 9,
  MethodRef = 10,
  InterfaceMethodRef = 11,
  NameAndType = 12,
  MethodHandle = 15,
  MethodType = 16,
  InvokeDynamic = 18
};

ConstantPool ConstantPool::parse(ByteReader *reader) {
  auto size = reader->NextU16("constant pool size");
  ConstantPool cp { size };  // 4.1: 1 through size - 1 are considered valid

  int index = 1;
  while (index < size) {
    auto tag = reader->NextU8("constant pool tag");

    cp.m_entries.at(index) = ([&] () -> ConstantPoolEntry {
      switch (tag) {
        case Utf8: {
          auto bytes = reader->NextNBytes(reader->NextU16("utf8 length"), "utf8 value");
          std::string value{bytes.begin(), bytes.end()};
          return EntryUtf8{value};
        }
        case Integer:
          return EntryInteger{reader->NextI32("integer value")};
        case Float:
          return EntryFloat{reader->NextF32("float value")};
        case Long:
          index++;
          return EntryLong{reader->NextI64("long value")};
        case Double:
          index++;
          return EntryDouble{reader->NextF64("double value")};
        case Class:
          return EntryClass{reader->NextU16("class name index")};
        case String:
          return EntryString{reader->NextU16("string index")};
        case FieldRef:
          return EntryFieldRef{reader->NextU16("fieldref struct index"),
                               reader->NextU16("fieldref name and type index")};
        case MethodRef:
          return EntryMethodRef{reader->NextU16("methodref struct index"),
                                reader->NextU16("methodref name and type index")};
        case InterfaceMethodRef:
          return EntryInterfaceMethodRef{reader->NextU16("interface methodref struct index"),
                                         reader->NextU16("interface methodref name and type index")};
        case NameAndType:
          return EntryNameAndType{reader->NextU16("name and type name index"),
                                  reader->NextU16("name and type descriptor index")};
        case MethodHandle:
          return EntryMethodHandle{reader->NextU8("method handle reference kind"),
                                   reader->NextU16("method handle reference index")};
        case MethodType:
          return EntryMethodType{reader->NextU16("method type descriptor index")};
        case InvokeDynamic:
          return EntryInvokeDynamic{reader->NextU16("invoke dynamic bootstrap method attr index"),
                                    reader->NextU16("invoke dynamic name and type index")};
        default:
          throw std::runtime_error("Unknown constant pool tag " + std::to_string(tag));
      }
    })();

    index++;
  }

  if (index > size) {
    throw std::runtime_error("Invalid constant pool size");
  }

  return cp;
}

ConstantPoolEntry *ConstantPool::GetAny(int index) {
  if (index == 0 || index >= m_entries.size())
    throw std::runtime_error("Invalid constant pool index");
  return &m_entries[index];
}

const ConstantPoolEntry *ConstantPool::GetAny(int index) const {
  if (index == 0 || index >= m_entries.size())
    throw std::runtime_error("Invalid constant pool index");
  return &m_entries[index];
}

std::string ConstantPool::ToString() const {
  std::string result;
  for (int i = 1; i < m_entries.size(); i++) {
    result += std::to_string(i) + ": ";
    std::visit([&](const auto& entry) {
      result += entry.ToString(this);
    }, m_entries[i]);
    result += '\n';
  }
  return result;
}

std::string EntryInvalid::ToString(const ConstantPool *_cp) const {
  return "<invalid slot>";
}
} // bjvm