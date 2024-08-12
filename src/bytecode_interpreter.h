//
// Created by Cowpox on 8/11/24.
//

#ifndef BYTECODE_INTERPRETER_H
#define BYTECODE_INTERPRETER_H

#include "classfile.h"
#include "execution_frame.h"

namespace bjvm {
class VM;
class ConstantPool;
namespace classfile {
struct MethodInfo;
}

class BytecodeInterpreter {
public:
  jvalue execute(VM* vm, classfile::MethodInfo* method_info);
};

} // bjvm

#endif //BYTECODE_INTERPRETER_H
