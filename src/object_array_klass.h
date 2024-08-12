//
// Created by Cowpox on 8/11/24.
//

#ifndef OBJECT_ARRAY_INSTANCE_H
#define OBJECT_ARRAY_INSTANCE_H
#include "array_klass.h"

namespace bjvm {

class ObjectArrayKlass : public ArrayKlass {
  BaseKlass* m_element;

public:
  ObjectArrayKlass(BaseKlass* element, PlainKlass* superclass);

  bool Link(VM *vm) override;

  size_t GetMemberSizeBytes() override;

  BaseKlass* GetElementType();
};

} // bjvm

#endif //OBJECT_ARRAY_INSTANCE_H
