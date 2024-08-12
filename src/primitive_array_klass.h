//
// Created by Cowpox on 8/11/24.
//

#ifndef PRIMITIVE_ARRAY_INSTANCE_H
#define PRIMITIVE_ARRAY_INSTANCE_H
#include "array_klass.h"
#include "classfile.h"

namespace bjvm {

class PrimitiveArrayKlass : public ArrayKlass {
  classfile::PrimitiveType m_type;
  int m_member_size_bytes;

public:
  PrimitiveArrayKlass(classfile::PrimitiveType type, PlainKlass* superclass);

  size_t GetMemberSizeBytes() override;

  bool Link(VM* vm) override;

  classfile::PrimitiveType GetPrimitiveType() const;
};

} // bjvm

#endif //PRIMITIVE_ARRAY_INSTANCE_H
