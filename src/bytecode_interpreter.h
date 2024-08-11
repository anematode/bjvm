//
// Created by Cowpox on 8/11/24.
//

#ifndef BYTECODE_INTERPRETER_H
#define BYTECODE_INTERPRETER_H

#include "classfile.h"
#include "execution_frame.h"

namespace bjvm {
class VM;

class BytecodeInterpreter {
  VM* m_vm;

  std::vector<ExecutionFrame> m_frames;

public:
  bool step();
};

} // bjvm

#endif //BYTECODE_INTERPRETER_H
