//
// Created by Cowpox on 8/11/24.
//

#ifndef ARRAY_INSTANCE_H
#define ARRAY_INSTANCE_H

#include "base_klass.h"
#include "classfile.h"

namespace bjvm {

/**
 * Abstract base class for all array instances.
 */
class ArrayKlass : public BaseKlass {
 classfile::FieldInfo m_length_field;

public:
 ArrayKlass(std::string &&name, PlainKlass* superclass);

 classfile::FieldInfo *GetField(VM *vm, const std::string &field_name) override;

 virtual size_t GetMemberSizeBytes() = 0;
};

} // bjvm

#endif //ARRAY_INSTANCE_H
