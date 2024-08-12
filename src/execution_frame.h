//
// Created by Cowpox on 8/11/24.
//

#ifndef EXECUTION_FRAME_H
#define EXECUTION_FRAME_H

#include <cstdint>
#include <vector>

#include "utilities.h"

namespace bjvm {

/**
 * A single execution frame on the call stack. No type checking is performed during bytecode interpretation as this is
 * the point of classfile verification.
 *
 * In the future this might be a variable length struct e.g. for better cache locality, since we know max_locals
 * and max_stack.
 */
struct ExecutionFrame {
 std::vector<jvalue> m_locals;
 std::vector<jvalue> m_stack;

 int m_stack_index = 0;
 int m_instruction_index = 0;

public:
 ExecutionFrame(int max_locals, int max_stack) : m_locals(max_locals), m_stack(max_stack) {}
};

} // bjvm

#endif //EXECUTION_FRAME_H
