//
// Created by Cowpox on 8/11/24.
//

#include "primitive_array_klass.h"

#include "utilities.h"

namespace bjvm {

using namespace classfile;

constexpr const char* GetArrayName(PrimitiveType type) {
  switch (type) {
    case PrimitiveType::boolean:
      return "[Z";
    case PrimitiveType::byte:
      return "[B";
    case PrimitiveType::char_:
      return "[C";
    case PrimitiveType::short_:
      return "[S";
    case PrimitiveType::int_:
      return "[I";
    case PrimitiveType::long_:
      return "[J";
    case PrimitiveType::float_:
      return "[F";
    case PrimitiveType::double_:
      return "[D";
    default:
      BJVM_UNREACHABLE();
  }
}

constexpr int GetPrimitiveSizeBytes(PrimitiveType type) {
  switch (type) {
    case PrimitiveType::boolean:
      return 1;
    case PrimitiveType::byte:
      return 1;
    case PrimitiveType::char_:
      return 2;
    case PrimitiveType::short_:
      return 2;
    case PrimitiveType::int_:
      return 4;
    case PrimitiveType::long_:
      return 8;
    case PrimitiveType::float_:
      return 4;
    case PrimitiveType::double_:
      return 8;
    default:
      BJVM_UNREACHABLE();
  }
}

PrimitiveArrayKlass::PrimitiveArrayKlass(PrimitiveType type, PlainKlass* superclass): ArrayKlass(GetArrayName(type), superclass), m_type(type) {
  m_member_size_bytes = GetPrimitiveSizeBytes(type);
  m_status = Status::Loaded;
}

size_t PrimitiveArrayKlass::GetMemberSizeBytes() {
  return m_member_size_bytes;
}

bool PrimitiveArrayKlass::Link(VM *vm) { return true; }

PrimitiveType PrimitiveArrayKlass::GetPrimitiveType() const {
  return m_type;
}
} // bjvm