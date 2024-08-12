//
// Created by Cowpox on 8/11/24.
//

#include "bytecode_interpreter.h"
#include "classfile.h"
#include "vm.h"

namespace bjvm {

int JavaDiv(int a, int b) {
  if (b == 0) {
    throw std::runtime_error("Division by zero");
  } else if (a == std::numeric_limits<int>::min() && b == -1) {
    return std::numeric_limits<int>::min();
  }

  return a / b;
}

int JavaRem(int a, int b) {
  if (b == 0) {
    throw std::runtime_error("Division by zero");
  } else if (a == std::numeric_limits<int>::min() && b == -1) {
    return 0;
  }

  return a % b;
}

jvalue BytecodeInterpreter::execute(VM *vm, classfile::MethodInfo* method_info) {
  using namespace classfile;
  using IC = InsnCode;

  ExecutionFrame frame = vm->m_frames.back();
  std::cout << "Executing method " + method_info->m_name << " with signature " + method_info->m_descriptor << std::endl;

  if (method_info->IsNative()) {
    std::vector<jvalue> args = frame.m_locals;
    auto [ found, result ] = vm->CallNativeMethod(method_info, frame.m_locals);
    if (!found)
      BJVM_DEBUG("Skipping native method " + method_info->m_name);
    return result;
  }

  // Gaming time
  int program_counter = 0;  // instruction index

  auto& code_opt = method_info->m_code;
  if (!code_opt.has_value()) {
    throw std::runtime_error("Method has no code");
  }

  auto& code = code_opt.value();

  std::cout << "Locals: ";
  for (auto k = frame.m_locals.begin(); k != frame.m_locals.end(); ++k) {
    std::cout << *k << ' ';
  }
  std::cout << '\n';

  while (true) {
    Insn instruction = code.m_code[program_counter];
    auto code = instruction.GetCode();

    std::cout << "Ex  " << CodeName(code) << std::endl;

    switch (code) {
      case InsnCode::nop: {
        program_counter++;
        continue;
      }
      case InsnCode::aaload:
        break;
      case InsnCode::aastore:
        break;
      case InsnCode::aconst_null: {
        frame.m_stack.push_back(0);
        program_counter++;
        continue;
      }
      case InsnCode::areturn: {
        return BJVM_CHECKED_POP(frame.m_stack);
      }
      case InsnCode::arraylength: {
        auto* obj = reinterpret_cast<HeapObject*>(BJVM_CHECKED_POP(frame.m_stack));
        std::cout << (uintptr_t)obj << '\n';
        assert(!obj->IsPlainObject());
        frame.m_stack.push_back(ToJValue(static_cast<ArrayObject*>(obj)->GetLength()));
        program_counter++;
        continue;
      }
      case InsnCode::athrow: {
        break;
      }
      case InsnCode::baload:
        break;
      case InsnCode::bastore:
        break;
      case InsnCode::caload:
        break;
      case InsnCode::castore: {
        int value = FromJValue<int>(BJVM_CHECKED_POP(frame.m_stack));
        int index = FromJValue<int>(BJVM_CHECKED_POP(frame.m_stack));
        auto* array = FromJValue<ArrayObject*>(BJVM_CHECKED_POP(frame.m_stack));

        array->GetData<uint16_t>()[index] = value;
        program_counter++;

        continue;
      }
      case InsnCode::d2f:
        break;
      case InsnCode::d2i:
        break;
      case InsnCode::d2l:
        break;
      case InsnCode::dadd:
        break;
      case InsnCode::daload:
        break;
      case InsnCode::dastore:
        break;
      case InsnCode::dcmpg:
        break;
      case InsnCode::dcmpl:
        break;
      case InsnCode::ddiv:
        break;
      case InsnCode::dmul:
        break;
      case InsnCode::dneg:
        break;
      case InsnCode::drem:
        break;
      case InsnCode::dreturn:
        break;
      case InsnCode::dsub:
        break;
      case InsnCode::dup: {
        jvalue top = BJVM_CHECKED_POP(frame.m_stack);
        frame.m_stack.push_back(top);
        frame.m_stack.push_back(top);
        program_counter++;
        continue;
      }
      case InsnCode::dup_x1:
        break;
      case InsnCode::dup_x2:
        break;
      case InsnCode::dup2:
        break;
      case InsnCode::dup2_x1:
        break;
      case InsnCode::dup2_x2:
        break;
      case InsnCode::f2d:
        break;
      case InsnCode::f2i:
        break;
      case InsnCode::f2l:
        break;
      case InsnCode::fadd:
        break;
      case InsnCode::faload:
        break;
      case InsnCode::fastore:
        break;
      case InsnCode::fcmpg:
        break;
      case InsnCode::fcmpl:
        break;
      case InsnCode::fdiv:
        break;
      case InsnCode::fmul:
        break;
      case InsnCode::fneg:
        break;
      case InsnCode::frem:
        break;
      case InsnCode::freturn:
        break;
      case InsnCode::fsub:
        break;
      case InsnCode::i2b:
        break;
      case InsnCode::i2c:
        break;
      case InsnCode::i2d:
        break;
      case InsnCode::i2f:
        break;
      case InsnCode::i2l:
        break;
      case InsnCode::i2s:
        break;
#define INT_OP(op) { \
  int a_ = FromJValue<int>(BJVM_CHECKED_POP(frame.m_stack)); \
  int b_ = FromJValue<int>(BJVM_CHECKED_POP(frame.m_stack)); \
  frame.m_stack.push_back(ToJValue(op(a_, b_))); \
  program_counter++; \
  continue; \
    }

      case InsnCode::iadd: INT_OP([&] (int a, int b) { return a + b; });
      case InsnCode::iaload:
        break;
      case InsnCode::iand: INT_OP([&] (int a, int b) { return a & b; });
      case InsnCode::iastore:
        break;
      case InsnCode::idiv: INT_OP(JavaDiv);
      case InsnCode::imul: INT_OP([&] (int a, int b) { int o; return __builtin_mul_overflow(a, b, &o); });
      case InsnCode::ineg:
        break;
      case InsnCode::ior: INT_OP([&] (int a, int b) { return a | b; });
      case InsnCode::irem: INT_OP(JavaRem);
      case InsnCode::ireturn: {
        return BJVM_CHECKED_POP(frame.m_stack);
      }
      case InsnCode::ishl: INT_OP([&] (int a, int b) { return static_cast<int>(static_cast<unsigned>(a) << (b & 0x1F)); });
      case InsnCode::ishr: INT_OP([&] (int a, int b) { return a >> (b & 0x1F); });
      case InsnCode::isub: INT_OP([&] (int a, int b) { return a - b; });
      case InsnCode::iushr: INT_OP([&] (int a, int b) { return static_cast<int>(static_cast<unsigned>(a) >> (b & 0x1F)); });
      case InsnCode::ixor: INT_OP([&] (int a, int b) { return a ^ b; });
      case InsnCode::l2d:
        break;
      case InsnCode::l2f:
        break;
      case InsnCode::l2i:
        break;
      case InsnCode::ladd:
        break;
      case InsnCode::laload:
        break;
      case InsnCode::land:
        break;
      case InsnCode::lastore:
        break;
      case InsnCode::lcmp:
        break;
      case InsnCode::ldc: {
        ConstantPoolEntry* ent = instruction.GetConstantPoolEntry();
        if (auto* str = std::get_if<EntryString>(ent)) {
          if (!str->m_thestring)
            str->m_thestring = vm->InternString(str->m_utf8->m_value);
          frame.m_stack.push_back(ToJValue(str->m_thestring));
        } else if (auto* integer = std::get_if<EntryInteger>(ent)) {
          frame.m_stack.push_back(ToJValue(integer->m_value));
        } else if (auto* float_ = std::get_if<EntryFloat>(ent)) {
          frame.m_stack.push_back(ToJValue(float_->m_value));
        } else if (auto* klass = std::get_if<EntryClass>(ent)) {
          auto* klass_obj = vm->LoadClass(klass->m_name->m_value);
          frame.m_stack.push_back(ToJValue(klass_obj->GetClassInstance(vm)));
        } else {
          std::cout << ent->index() << '\n';
          throw std::runtime_error("Invalid ldc entry");
        }

        program_counter++;
        continue;
      }
      case InsnCode::ldc2_w: {
        ConstantPoolEntry* ent = instruction.GetConstantPoolEntry();
        if (auto* long_ = std::get_if<EntryLong>(ent)) {
          frame.m_stack.push_back(ToJValue(long_->m_value));
          frame.m_stack.push_back(0);
        } else if (auto* double_ = std::get_if<EntryDouble>(ent)) {
          frame.m_stack.push_back(ToJValue(double_->value));
          frame.m_stack.push_back(0);
        } else {
          throw std::runtime_error("Invalid ldc2 entry");
        }
        break;
      }
      case InsnCode::ldiv:
        break;
      case InsnCode::lmul:
        break;
      case InsnCode::lneg:
        break;
      case InsnCode::lor:
        break;
      case InsnCode::lrem:
        break;
      case InsnCode::lreturn:
        break;
      case InsnCode::lshl:
        break;
      case InsnCode::lshr:
        break;
      case InsnCode::lsub:
        break;
      case InsnCode::lushr:
        break;
      case InsnCode::lxor:
        break;
      case InsnCode::monitorenter: {
        BJVM_DEBUG("Ignoring monitorenter");
        BJVM_CHECKED_POP(frame.m_stack);
        program_counter++;
        continue;
      }
      case InsnCode::monitorexit: {
        BJVM_DEBUG("Ignoring monitorexit");
        BJVM_CHECKED_POP(frame.m_stack);
        program_counter++;
        continue;
      }
      case InsnCode::pop:
        break;
      case InsnCode::pop2:
        break;
      case InsnCode::return_: {
        return 0;
      }
      case InsnCode::saload:
        break;
      case InsnCode::sastore:
        break;
      case InsnCode::swap:
        break;
      case InsnCode::dload:
        break;
      case InsnCode::fload:
        break;
      case InsnCode::lload:
        break;
      case InsnCode::dstore:
        break;
      case InsnCode::lstore:
        break;
      case InsnCode::iload:
      case InsnCode::aload: {
        auto index = instruction.Index();
        frame.m_stack.push_back(frame.m_locals.at(index));
        program_counter++;
        continue;
      }
      case InsnCode::fstore:
      case InsnCode::istore:
      case InsnCode::astore: {
        auto index = instruction.Index();
        frame.m_locals[index] = BJVM_CHECKED_POP(frame.m_stack);
        program_counter++;
        continue;
      }
      case InsnCode::anewarray: {
        auto* entry = std::get_if<EntryClass>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        auto* klass = vm->LoadArrayOfClass(entry);
        assert(klass != nullptr);

        auto* array = vm->CreateArrayObject(klass, FromJValue<int>(BJVM_CHECKED_POP(frame.m_stack)));
        frame.m_stack.push_back(reinterpret_cast<jvalue>(array));

        program_counter++;
        continue;
      }
      case InsnCode::checkcast:
        break;
      case InsnCode::getfield: {
        auto* entry = std::get_if<EntryFieldRef>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        HeapObject* obj = FromJValue<HeapObject*>(BJVM_CHECKED_POP(frame.m_stack));
        std::cout << (uintptr_t)obj << '\n';
        frame.m_stack.push_back(obj->LoadField(entry->m_field_info->m_static_or_instance_offset));

        program_counter++;
        continue;
      }
      case InsnCode::getstatic: {
        auto* entry = std::get_if<EntryFieldRef>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        frame.m_stack.push_back(vm->GetStaticField(entry));
        program_counter++;
        continue;
      }
      case InsnCode::instanceof:
        break;
      case InsnCode::invokedynamic:
        break;
      case InsnCode::new_: {
        EntryClass* entry = std::get_if<EntryClass>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        BaseKlass* klass = vm->LoadClass(entry->m_name->m_value);
        assert(klass != nullptr);

        if (!klass->IsPlainObjectKlass()) throw std::runtime_error("Cannot instantiate non-object class");

        HeapObject* obj = vm->AllocatePlainObject(static_cast<PlainKlass*>(klass));
        frame.m_stack.push_back(reinterpret_cast<jvalue>(obj));

        program_counter++;
        continue;
      }
      case InsnCode::putfield: {
        jvalue val = BJVM_CHECKED_POP(frame.m_stack);

        auto* entry = std::get_if<EntryFieldRef>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        BaseKlass* klass = entry->m_struct->m_instance;
        assert(klass != nullptr);

        auto* obj = FromJValue<HeapObject*>(BJVM_CHECKED_POP(frame.m_stack));
        std::cout << (uintptr_t)obj << '\n';
        std::cout << (uintptr_t)entry->m_field_info << '\n';
        std::cout << entry->m_field_info->m_static_or_instance_offset << '\n';
        obj->PutField(entry->m_field_info->m_static_or_instance_offset, val);

        program_counter++;
        continue;
      }
      case InsnCode::putstatic: {
        jvalue val = BJVM_CHECKED_POP(frame.m_stack);
        auto* entry = std::get_if<EntryFieldRef>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        vm->PutStaticField(entry, val);
        program_counter++;
        continue;
      }
      case InsnCode::invokevirtual: {
        auto* entry = std::get_if<EntryMethodRef>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        auto* interface_method = vm->GetMethod(entry);
        assert(interface_method != nullptr);
        assert(interface_method->GetArity() + 1 <= frame.m_stack.size());

        HeapObject* obj = FromJValue<HeapObject*>(*(&*frame.m_stack.end() - interface_method->GetArity() - 1));
        if (obj == nullptr) {
          std::cout << "Method: " << entry->m_name_and_type->m_name->m_value << '\n';
          throw std::runtime_error("Null pointer exception");
        }

        auto* klass = obj->GetKlass();
        auto* method = klass->GetMethod(vm, entry->m_name_and_type->m_name->m_value, entry->m_name_and_type->m_descriptor->m_value, false);

        assert(method != nullptr);

        auto* args_start = &*frame.m_stack.end() - method->GetArity() - 1;

        jvalue result = vm->CallSpecialMethod(FromJValue<HeapObject*>(*args_start), method, args_start + 1);
        for (int i = 0; i < method->GetArity() + 1; ++i) BJVM_CHECKED_POP(frame.m_stack);

        if (method->HasReturn())
          frame.m_stack.push_back(result);

        program_counter++;
        continue;
      }
      case InsnCode::invokespecial: {
        auto* entry = std::get_if<EntryMethodRef>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);

        auto* method = vm->GetMethod(entry);
        assert(method != nullptr);
        assert(method->GetArity() + 1 <= frame.m_stack.size());

        auto* args_start = &*frame.m_stack.end() - method->GetArity() - 1;

        std::cout << "invokespecial Gaming: ";
        for (auto* k = args_start; k != &*frame.m_stack.end(); ++k) {
          std::cout << *k << ' ';
        }
        std::cout << '\n';

        jvalue result = vm->CallSpecialMethod(FromJValue<HeapObject*>(*args_start), method, args_start + 1);
        for (int i = 0; i < method->GetArity() + 1; ++i) BJVM_CHECKED_POP(frame.m_stack);

        if (method->HasReturn())
          frame.m_stack.push_back(result);

        program_counter++;
        continue;
      }
      case InsnCode::invokestatic: {
        auto* entry = std::get_if<EntryMethodRef>(instruction.GetConstantPoolEntry());
        assert(entry != nullptr);
        std::cout << "Invoke static on " << entry->m_name_and_type->m_name->m_value << '\n';

        auto* method = vm->GetStaticMethod(entry);
        assert(method != nullptr);

        int arity = method->GetArity();

        assert(frame.m_stack.size() >= method->GetArity());

        auto* args_start = arity == 0 ? nullptr : &*frame.m_stack.end() - method->GetArity();

        std::cout << "arity: " << method->m_descriptor << '\n';

        std::cout << "Gaming: ";
        if (args_start)
        for (auto* k = args_start; k != &*frame.m_stack.end(); ++k) {
          std::cout << *k << ' ';
        }
        std::cout << '\n';


        std::cout << "Locals: ";
        for (auto k = frame.m_locals.begin(); k != frame.m_locals.end(); ++k) {
          std::cout << *k << ' ';
        }
        std::cout << '\n';

        jvalue result = vm->CallStaticMethod(method, args_start);
        if (method->HasReturn()) frame.m_stack.push_back(result);

        program_counter++;
        continue;
      }
      case InsnCode::goto_:
        break;
      case InsnCode::jsr:
        break;
#define INT_CMP(OP) { \
      int val = FromJValue<int>(BJVM_CHECKED_POP(frame.m_stack)); \
      if (val OP 0) program_counter = instruction.Index(); \
      else program_counter++; \
      continue; \
      }
#define BINARY_CMP(OP, type) { \
  int a = FromJValue<type>(BJVM_CHECKED_POP(frame.m_stack)); \
  int b = FromJValue<type>(BJVM_CHECKED_POP(frame.m_stack)); \
  if (a OP b) program_counter = instruction.Index(); \
  else program_counter++; \
  continue; \
  }

      case InsnCode::if_acmpeq: BINARY_CMP(==, jvalue);
      case InsnCode::if_acmpne: BINARY_CMP(!=, jvalue);
      case InsnCode::if_icmpeq: BINARY_CMP(==, int);
      case InsnCode::if_icmpne: BINARY_CMP(!=, int);
      case InsnCode::if_icmplt: BINARY_CMP(<, int);
      case InsnCode::if_icmpge: BINARY_CMP(>=, int);
      case InsnCode::if_icmpgt: BINARY_CMP(>, int);
      case InsnCode::if_icmple: BINARY_CMP(<=, int);
      case InsnCode::ifeq: INT_CMP(==);
      case InsnCode::ifne: INT_CMP(!=);
      case InsnCode::iflt: INT_CMP(<);
      case InsnCode::ifge: INT_CMP(>=);
      case InsnCode::ifgt: INT_CMP(>);
      case InsnCode::ifle: INT_CMP(<=);
      case InsnCode::ifnonnull: {
        int val = BJVM_CHECKED_POP(frame.m_stack);
        if (val == 0) program_counter++;
        else program_counter = instruction.Index();
        continue;
      }
      case InsnCode::ifnull: {
        int val = BJVM_CHECKED_POP(frame.m_stack);
        if (val != 0) program_counter++;
        else program_counter = instruction.Index();
        continue;
      }
      case InsnCode::iconst: {
        frame.m_stack.push_back(ToJValue(instruction.GetIntData()));
        program_counter++;
        continue;
      }
      case InsnCode::dconst:
        break;
      case InsnCode::fconst:
        break;
      case InsnCode::lconst:
        break;
      case InsnCode::iinc:
        break;
      case InsnCode::invokeinterface:
        break;
      case InsnCode::multianewarray:
        break;
      case InsnCode::newarray: {
        int count = FromJValue<int>(BJVM_CHECKED_POP(frame.m_stack));
        PrimitiveType atype = instruction.GetArrayType();

        PrimitiveArrayKlass* klass = vm->LoadPrimitiveArrayClass(atype);
        assert(klass != nullptr);
        assert(count >= 0);

        ArrayObject* obj = vm->AllocateArrayObject(klass, count);
        frame.m_stack.push_back(ToJValue(obj));

        program_counter++;
        continue;
      }
      case InsnCode::tableswitch:
        break;
      case InsnCode::lookupswitch:
        break;
      case InsnCode::ret:
        break;
    }

    throw std::runtime_error("Unimplemented opcode: " + std::string(CodeName(code)));
  }

  return 0ULL;
}
} // bjvm