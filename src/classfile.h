//
// Created by Cowpox on 8/9/24.
//

#ifndef BROWSER_JVM_CLASSFILE_H
#define BROWSER_JVM_CLASSFILE_H

#include <cstdint>
#include <cassert>
#include <vector>
#include <algorithm>
#include <exception>
#include <iostream>
#include "byte_reader.h"
#include "constant_pool.h"

namespace bjvm::classfile {

/** Exception thrown when a classfile fails verification. For now we'll do very light verification. */
struct VerifyError : std::runtime_error {
  int m_offset;

  explicit VerifyError(const std::string& what, int offset = -1);
};

/**
 * Instruction code. Similar instructions like aload_0 are canonicalised to aload with an argument of 0.
 *
 * List of canonicalisations:
 *   aload_<n> -> aload, dload_<n> -> dload, fload_<n> -> fload, iload_<n> -> iload, lload_<n> -> lload,
 *   astore_<n> -> astore, dstore_<n> -> dstore, fstore_<n> -> fstore, istore_<n> -> istore, lstore_<n> -> lstore,
 *   bipush, sipush, iconst_<n>, iconst_<n> -> iconst, dconst_<d> -> dconst, fconst_<f> -> fconst
 */
enum class InsnCode : uint8_t {
  /** No operands */
  nop,

  aaload, aastore, aconst_null, areturn, arraylength, athrow, baload, bastore, caload, castore, d2f, d2i,
  d2l, dadd, daload, dastore, dcmpg, dcmpl, ddiv, dmul, dneg, drem, dreturn, dsub, dup, dup_x1, dup_x2, dup2, dup2_x1,
  dup2_x2, f2d, f2i, f2l, fadd, faload, fastore, fcmpg, fcmpl, fdiv, fmul, fneg, frem, freturn, fsub, i2b, i2c,
  i2d, i2f, i2l, i2s, iadd, iaload, iand, iastore, idiv, imul, ineg, ior, irem, ireturn, ishl, ishr, isub, iushr, ixor, l2d,
  l2f, l2i, ladd, laload, land, lastore, lcmp, ldc, ldc2_w, ldiv, lmul, lneg, lor, lrem, lreturn, lshl, lshr,
  lsub, lushr, lxor, monitorenter, monitorexit, pop, pop2, return_, saload, sastore, swap,

  /** Indexes into local variable table */
  dload, fload, iload, lload, dstore, fstore, istore, lstore, aload, astore,

  /** Indexes into constant pool */
  anewarray, checkcast, getfield, getstatic, instanceof, invokedynamic, new_, putfield, putstatic, invokevirtual,
  invokespecial, invokestatic,

  /** Indexes into instruction table */
  goto_, jsr, ret,

  if_acmpeq, if_acmpne, if_icmpeq, if_icmpne, if_icmplt, if_icmpge,
  if_icmpgt, if_icmple, ifeq, ifne, iflt, ifge, ifgt, ifle, ifnonnull, ifnull,

  /** Has some numerical immediate */
  iconst, dconst, fconst, lconst,

  /** Cursed */
  iinc, invokeinterface, multianewarray, newarray, tableswitch, lookupswitch
};

enum class PrimitiveType : uint8_t {
  boolean, byte, char_, short_, int_, long_, float_, double_
};

const char* CodeName(InsnCode code);

struct SwitchDataBase {
  int m_default_target;
  // Instruction index, not PC
  std::vector<int> m_targets;

  template <typename L>
  void TransformTargets(L lambda) {
    lambda(m_default_target);
    std::for_each(m_targets.begin(), m_targets.end(), lambda);
  }
};

struct TableswitchData : SwitchDataBase {
  int m_low;
  int m_high;
};

struct IIncData {
  uint16_t m_index;
  int16_t m_const;
};

struct InvokeInterfaceData {
  uint16_t m_index;
  uint8_t m_count;
};

struct MultianewarrayData {
  uint16_t m_index;
  uint8_t m_dims;
};

struct LookupswitchData : SwitchDataBase {
  std::vector<int> m_keys;
};

/** Passed down when parsing to allocate useful information. */
struct ParseContext {
  std::vector<TableswitchData> m_tableswitches;
  std::vector<LookupswitchData> m_lookupswitches;

  const ConstantPool* cp;

  long MakeTableswitch(TableswitchData&& data);

  long MakeLookupswitch(LookupswitchData&& data);
};

/**
 * Resolved bytecode instruction. The data field's interpretation depends on the instruction and may be a
 * pointer (e.g. for tableswitch and lookupswitch), index into the constant pool, or immediate value.
 */
class Insn {
  friend struct CodeAttribute;
  friend struct MethodInfo;

  union {
    // for newarray
    PrimitiveType atype;
    // Constant pool or local variable index or branch target (instruction index)
    uint16_t index;
    // Integer or long immediate
    long imm;
    // Float immediate
    float f_imm;
    // Double immediate
    double d_imm;
    // lookupswitch
    LookupswitchData* ls;
    // tableswitch
    TableswitchData* ts;
    // iinc
    IIncData iinc;
    // invokeinterface
    InvokeInterfaceData ii;
    // multianewarray
    MultianewarrayData mna;
  } m_data = { .imm = 0L };
  InsnCode m_code = InsnCode::nop;
  int m_pc = 0;

  explicit Insn(InsnCode code);
  Insn(InsnCode code, decltype(m_data) data);
public:
  Insn() = default;

  /** Get the instruction code. */
  InsnCode GetCode() const;

  /** Get the constant pool index, local variable index, or instruction index of this bytecode instruction. */
  uint16_t Index() const;

  /** Get the instruction's program counter. */
  uint16_t GetPC() const;

  /** Set the instruction's program counter. */
  void SetPC(uint16_t pc);

  /** Get the double immediate value of this dconst instruction. */
  double GetDoubleData() const;

  /** Get the integer immediate value of this iconst instruction. */
  int GetIntData() const;

  /** Get the long immediate value of this lconst instruction. */
  long GetLongData() const;

  /** Get the float immediate value of this fconst instruction. */
  float GetFloatData() const;

  /** Get the data for this tableswitch instruction. */
  const TableswitchData* GetTableswitchData() const;

  /** Get the data for this iinc instruction. */
  IIncData GetIIncData() const;

  /** Get the data for this multianewarray instruction. */
  MultianewarrayData GetMultianewarrayData() const;

  /** Get the data for this lookupswitch instruction. */
  const LookupswitchData* GetLookupswitchData() const;

  /** Get the data for this invokeinterface instruction. */
  const InvokeInterfaceData* GetInvokeInterfaceData() const;

  /** Get the primitive type of this newarray instruction. */
  PrimitiveType GetArrayType() const;

  /** Parse an instruction from a reader. */
  static Insn parse(ByteReader* reader, ParseContext* cp);

  /** Convert this instruction to a readable string. */
  std::string ToString(const ConstantPool* pool = nullptr) const;

  bool IsIf() const;

  bool ContainsBranch() const;
};

struct ClassfileVersion {
  uint16_t m_minor;
  uint16_t m_major;
};

enum class AccessFlags {
  ACC_PUBLIC = 0x0001,
  ACC_FINAL = 0x0010,
  ACC_SUPER = 0x0020,
  ACC_INTERFACE = 0x0200,
  ACC_ABSTRACT = 0x0400,
  ACC_SYNTHETIC = 0x1000,
  ACC_ANNOTATION = 0x2000,
  ACC_ENUM = 0x4000
};

/**
 * One entry in the ExceptionTable.
 *
 * If an exception is thrown in the range [start_pc, end_pc), the first handler matching m_catch_type is executed; its
 * location is at handler_pc. If no handlers match, execution continues to the current function's caller. (Note that
 * we're not implementing jsr yet)
 */
struct ExceptionTableEntry {
  uint16_t m_start;
  uint16_t m_end;
  uint16_t m_handler;
  uint16_t m_catch_type;
};

/**
 * Attribute containing a list of exceptions that a method may throw -- found as part of a CodeAttribute.
 */
struct ExceptionTableAttribute {
  std::vector<ExceptionTableEntry> m_exceptions;
};

struct BootstrapMethod {
  uint16_t m_method_ref;
  std::vector<uint16_t> m_arguments;
};

struct BootstrapMethodsAttribute {
  std::vector<BootstrapMethod> m_methods;

  static BootstrapMethodsAttribute parse(ByteReader* reader);
};

struct ConstantValueAttribute {
  uint16_t m_index;
};

/**
 * Enclosing method attribute, which specifies the method that encloses a local or anonymous class.
 */
struct EnclosingMethodAttribute {
  uint16_t m_class_index;
  uint16_t m_method_index;
};

/**
 * One member of an inner class attribute.
 */
struct InnerClass {
  uint16_t m_inner_class_info_index;
  uint16_t m_outer_class_info_index;
  uint16_t m_inner_name_index;
  uint16_t m_inner_class_access_flags;
};

/**
 * One entry in the LineNumberTable.
 */
struct LineNumberTableEntry {
  uint16_t m_start;
  uint16_t m_line_number;
};

/**
 * Line number table attribute, which maps bytecode offsets to line numbers in the source file.
 */
struct LineNumberTable {
  std::vector<LineNumberTableEntry> m_entries;
};

struct CodeAttribute {
  uint16_t m_max_stack;
  uint16_t m_max_locals;
  std::vector<Insn> m_code;
  std::vector<uint16_t> m_exceptions;

  // Mapping of program counter to instruction index
  std::vector<std::optional<uint16_t>> m_pc_to_index;

  ExceptionTableAttribute m_exception_table;
  std::optional<LineNumberTable> m_line_number_table;

  std::optional<int> ProgramCounterToInsnIndex(int pc) const;

  static CodeAttribute parse(ByteReader* reader, ParseContext* parse_context);
};

enum class FieldAccessFlags {
  PUBLIC = 0x0001,
  PRIVATE = 0x0002,
  PROTECTED = 0x0004,
  STATIC = 0x0008,
  FINAL = 0x0010,
  VOLATILE = 0x0040,
  TRANSIENT = 0x0080,
  SYNTHETIC = 0x1000,
  ENUM = 0x4000
};

/**
 * One field in a class.
 */
struct FieldInfo {
  FieldAccessFlags m_access_flags;
  uint16_t m_name_index;
  uint16_t m_descriptor_index;

  // If the field is static, this is the value it takes on
  std::optional<ConstantValueAttribute> m_constant_value;

  static FieldInfo parse(ByteReader* reader, ParseContext* ctx);
};

enum class MethodAccessFlags {
  PUBLIC = 0x0001,
  PRIVATE = 0x0002,
  PROTECTED = 0x0004,
  STATIC = 0x0008,
  FINAL = 0x0010,
  SYNCHRONIZED = 0x0020,
  BRIDGE = 0x0040,
  VARARGS = 0x0080,
  NATIVE = 0x0100,
  ABSTRACT = 0x0400,
  STRICT = 0x0800,
  SYNTHETIC = 0x1000
};

struct MethodInfo {
  MethodAccessFlags m_access_flags;
  uint16_t m_name_index;
  uint16_t m_descriptor_index;

  std::optional<CodeAttribute> m_code;

  static MethodInfo parse(ByteReader* reader, ParseContext* parse_context);

  std::string ToString(ConstantPool *p_pool) const {
    throw std::runtime_error("unimplemented");
  }

  /**
   * Replace indices into the lookupswitch/tableswitch data tables with pointers.
   * @param ctx The parse context.
   */
  void FixupSwitchInstructions(ParseContext * ctx);
};

/**
 * Parsed Java classfile.
 */
class Classfile {
  std::vector<TableswitchData> m_tableswitches;    // used to keep instructions compact and trivially copyable
  std::vector<LookupswitchData> m_lookupswitches;

  Classfile(ConstantPool&& cp) : m_cp(std::move(cp)) {}

public:
  ClassfileVersion m_version;
  ConstantPool m_cp;

  AccessFlags m_access_flags;
  uint16_t m_this_class;
  uint16_t m_super_class;
  std::vector<uint16_t> m_interfaces;
  std::vector<FieldInfo> m_fields;
  std::vector<MethodInfo> m_methods;

  std::optional<BootstrapMethodsAttribute> m_bootstrap_methods;

  /**
   * Parse a classfile from a reader.
   */
  static Classfile parse(ByteReader* reader);

  /**
   * Pretty print this classfile in the same format as javap.
   */
  std::string ToString() const;

  /**
   * Get the name of this class.
   */
  const std::string& GetName() const;
};

}

#endif //BROWSER_JVM_CLASSFILE_H
