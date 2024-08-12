//
// Created by Cowpox on 8/9/24.
//

#include "classfile.h"

#include <sstream>
#include <iostream>

#include "utilities.h"

namespace bjvm::classfile {

/**
 * Converts the atype field of a newarray instruction to a primitive type.
 * @param byte The classfile byte.
 * @return The primitive type.
 */
PrimitiveType CheckAType(uint8_t byte) {
  switch (byte) {
    case 4: return PrimitiveType::boolean;
    case 5: return PrimitiveType::char_;
    case 6: return PrimitiveType::float_;
    case 7: return PrimitiveType::double_;
    case 8: return PrimitiveType::byte;
    case 9: return PrimitiveType::short_;
    case 10: return PrimitiveType::int_;
    case 11: return PrimitiveType::long_;

    default: throw VerifyError("Invalid array type " + std::to_string(byte));
  }
}

// Generated from https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-7.html
// copy([...document.getElementsByTagName('code')].map(o => o.nextSibling.data.trim() + " = " + o.innerText.match(/\(.*\)/g)[0].slice(1, 5)).join(',\n'));
enum RawOpcode {
  nop = 0x00,
  aconst_null = 0x01,
  iconst_m1 = 0x02,
  iconst_0 = 0x03,
  iconst_1 = 0x04,
  iconst_2 = 0x05,
  iconst_3 = 0x06,
  iconst_4 = 0x07,
  iconst_5 = 0x08,
  lconst_0 = 0x09,
  lconst_1 = 0x0a,
  fconst_0 = 0x0b,
  fconst_1 = 0x0c,
  fconst_2 = 0x0d,
  dconst_0 = 0x0e,
  dconst_1 = 0x0f,
  bipush = 0x10,
  sipush = 0x11,
  ldc = 0x12,
  ldc_w = 0x13,
  ldc2_w = 0x14,
  iload = 0x15,
  lload = 0x16,
  fload = 0x17,
  dload = 0x18,
  aload = 0x19,
  iload_0 = 0x1a,
  iload_1 = 0x1b,
  iload_2 = 0x1c,
  iload_3 = 0x1d,
  lload_0 = 0x1e,
  lload_1 = 0x1f,
  lload_2 = 0x20,
  lload_3 = 0x21,
  fload_0 = 0x22,
  fload_1 = 0x23,
  fload_2 = 0x24,
  fload_3 = 0x25,
  dload_0 = 0x26,
  dload_1 = 0x27,
  dload_2 = 0x28,
  dload_3 = 0x29,
  aload_0 = 0x2a,
  aload_1 = 0x2b,
  aload_2 = 0x2c,
  aload_3 = 0x2d,
  iaload = 0x2e,
  laload = 0x2f,
  faload = 0x30,
  daload = 0x31,
  aaload = 0x32,
  baload = 0x33,
  caload = 0x34,
  saload = 0x35,
  istore = 0x36,
  lstore = 0x37,
  fstore = 0x38,
  dstore = 0x39,
  astore = 0x3a,
  istore_0 = 0x3b,
  istore_1 = 0x3c,
  istore_2 = 0x3d,
  istore_3 = 0x3e,
  lstore_0 = 0x3f,
  lstore_1 = 0x40,
  lstore_2 = 0x41,
  lstore_3 = 0x42,
  fstore_0 = 0x43,
  fstore_1 = 0x44,
  fstore_2 = 0x45,
  fstore_3 = 0x46,
  dstore_0 = 0x47,
  dstore_1 = 0x48,
  dstore_2 = 0x49,
  dstore_3 = 0x4a,
  astore_0 = 0x4b,
  astore_1 = 0x4c,
  astore_2 = 0x4d,
  astore_3 = 0x4e,
  iastore = 0x4f,
  lastore = 0x50,
  fastore = 0x51,
  dastore = 0x52,
  aastore = 0x53,
  bastore = 0x54,
  castore = 0x55,
  sastore = 0x56,
  pop = 0x57,
  pop2 = 0x58,
  dup = 0x59,
  dup_x1 = 0x5a,
  dup_x2 = 0x5b,
  dup2 = 0x5c,
  dup2_x1 = 0x5d,
  dup2_x2 = 0x5e,
  swap = 0x5f,
  iadd = 0x60,
  ladd = 0x61,
  fadd = 0x62,
  dadd = 0x63,
  isub = 0x64,
  lsub = 0x65,
  fsub = 0x66,
  dsub = 0x67,
  imul = 0x68,
  lmul = 0x69,
  fmul = 0x6a,
  dmul = 0x6b,
  idiv = 0x6c,
  ldiv = 0x6d,
  fdiv = 0x6e,
  ddiv = 0x6f,
  irem = 0x70,
  lrem = 0x71,
  frem = 0x72,
  drem = 0x73,
  ineg = 0x74,
  lneg = 0x75,
  fneg = 0x76,
  dneg = 0x77,
  ishl = 0x78,
  lshl = 0x79,
  ishr = 0x7a,
  lshr = 0x7b,
  iushr = 0x7c,
  lushr = 0x7d,
  iand = 0x7e,
  land = 0x7f,
  ior = 0x80,
  lor = 0x81,
  ixor = 0x82,
  lxor = 0x83,
  iinc = 0x84,
  i2l = 0x85,
  i2f = 0x86,
  i2d = 0x87,
  l2i = 0x88,
  l2f = 0x89,
  l2d = 0x8a,
  f2i = 0x8b,
  f2l = 0x8c,
  f2d = 0x8d,
  d2i = 0x8e,
  d2l = 0x8f,
  d2f = 0x90,
  i2b = 0x91,
  i2c = 0x92,
  i2s = 0x93,
  lcmp = 0x94,
  fcmpl = 0x95,
  fcmpg = 0x96,
  dcmpl = 0x97,
  dcmpg = 0x98,
  ifeq = 0x99,
  ifne = 0x9a,
  iflt = 0x9b,
  ifge = 0x9c,
  ifgt = 0x9d,
  ifle = 0x9e,
  if_icmpeq = 0x9f,
  if_icmpne = 0xa0,
  if_icmplt = 0xa1,
  if_icmpge = 0xa2,
  if_icmpgt = 0xa3,
  if_icmple = 0xa4,
  if_acmpeq = 0xa5,
  if_acmpne = 0xa6,
  goto_ = 0xa7,
  jsr = 0xa8,
  ret = 0xa9,
  tableswitch = 0xaa,
  lookupswitch = 0xab,
  ireturn = 0xac,
  lreturn = 0xad,
  freturn = 0xae,
  dreturn = 0xaf,
  areturn = 0xb0,
  return_ = 0xb1,
  getstatic = 0xb2,
  putstatic = 0xb3,
  getfield = 0xb4,
  putfield = 0xb5,
  invokevirtual = 0xb6,
  invokespecial = 0xb7,
  invokestatic = 0xb8,
  invokeinterface = 0xb9,
  invokedynamic = 0xba,
  new_ = 0xbb,
  newarray = 0xbc,
  anewarray = 0xbd,
  arraylength = 0xbe,
  athrow = 0xbf,
  checkcast = 0xc0,
  instanceof = 0xc1,
  monitorenter = 0xc2,
  monitorexit = 0xc3,
  wide = 0xc4,
  multianewarray = 0xc5,
  ifnull = 0xc6,
  ifnonnull = 0xc7,
  goto_w = 0xc8,
  jsr_w = 0xc9
};

// The following fixups are required once all instructions are parsed:
//  1. lookupswitch/tableswitch need to have their data converted to pointers to the statically allocated data.
//  2. Branch offsets must be converted to instruction indices.
//     a. goto, jsr store the offset in the 32-bit imm.
//     b. if<cond> store the offset in the 16-bit index (interpreted as int16_t).
//     c. tableswitch, lookupswitch store their offsets in the statically allocated tableswitch/lookupswitch data.

Insn Insn::parse(ByteReader *reader, ParseContext* ctx) {
  auto opc = reader->NextU8("instruction opcode");
  using IC = InsnCode;

  // std::cout << "Parsing " << std::to_string(opc) << " at offset " << reader->GetOffs() << '\n';

  switch (opc) {
    case nop: return Insn(IC::nop);
    case aconst_null: return Insn(IC::aconst_null);
    case iconst_m1: case iconst_0: case iconst_1: case iconst_2: case iconst_3: case iconst_4: case iconst_5:
      return Insn(IC::iconst, { .imm = opc - iconst_0 });
    case lconst_0: case lconst_1:
      return Insn(IC::lconst, { .imm = opc - lconst_0 });
    case fconst_0: case fconst_1: case fconst_2:
      return Insn(IC::fconst, { .f_imm = static_cast<float>(opc - fconst_0) });
    case dconst_0: case dconst_1:
      return Insn(IC::dconst, { .d_imm = static_cast<double>(opc - dconst_0) });
    case bipush: return Insn(IC::iconst, { .imm = reader->NextI8("bipush immediate") });
    case sipush: return Insn(IC::iconst, { .imm = reader->NextI16("sipush immediate") });
    case ldc: return Insn(IC::ldc, { .index = reader->NextU8("ldc index") });
    case ldc_w: return Insn(IC::ldc, { .index = reader->NextU16("ldc_w index") });
    case ldc2_w: return Insn(IC::ldc2_w, { .index = reader->NextU16("ldc_w index") });
    case iload: return Insn(IC::iload, { .index = reader->NextU8("iload index") });
    case lload: return Insn(IC::lload, { .index = reader->NextU8("lload index") });
    case fload: return Insn(IC::fload, { .index = reader->NextU8("fload index") });
    case dload: return Insn(IC::dload, { .index = reader->NextU8("dload index") });
    case aload: return Insn(IC::aload, { .index = reader->NextU8("aload index") });
    case iload_0: case iload_1: case iload_2: case iload_3:
      return Insn(IC::iload, { .index = static_cast<uint16_t>(opc - iload_0) });
    case lload_0: case lload_1: case lload_2: case lload_3:
      return Insn(IC::lload, { .index = static_cast<uint16_t>(opc - lload_0) });
    case fload_0: case fload_1: case fload_2: case fload_3:
      return Insn(IC::fload, { .index = static_cast<uint16_t>(opc - fload_0) });
    case dload_0: case dload_1: case dload_2: case dload_3:
      return Insn(IC::dload, { .index = static_cast<uint16_t>(opc - dload_0) });
    case aload_0: case aload_1: case aload_2: case aload_3:
      return Insn(IC::aload, { .index = static_cast<uint16_t>(opc - aload_0) });
    case iaload: return Insn(IC::iaload);
    case laload: return Insn(IC::laload);
    case faload: return Insn(IC::faload);
    case daload: return Insn(IC::daload);
    case aaload: return Insn(IC::aaload);
    case baload: return Insn(IC::baload);
    case caload: return Insn(IC::caload);
    case saload: return Insn(IC::saload);
    case istore: return Insn(IC::istore, { .index = reader->NextU8("istore index") });
    case lstore: return Insn(IC::lstore, { .index = reader->NextU8("lstore index") });
    case fstore: return Insn(IC::fstore, { .index = reader->NextU8("fstore index") });
    case dstore: return Insn(IC::dstore, { .index = reader->NextU8("dstore index") });
    case astore: return Insn(IC::astore, { .index = reader->NextU8("astore index") });
    case istore_0: case istore_1: case istore_2: case istore_3:
      return Insn(IC::istore, { .index = static_cast<uint16_t>(opc - istore_0) });
    case lstore_0: case lstore_1: case lstore_2: case lstore_3:
      return Insn(IC::lstore, { .index = static_cast<uint16_t>(opc - lstore_0) });
    case fstore_0: case fstore_1: case fstore_2: case fstore_3:
      return Insn(IC::fstore, { .index = static_cast<uint16_t>(opc - fstore_0) });
    case dstore_0: case dstore_1: case dstore_2: case dstore_3:
      return Insn(IC::dstore, { .index = static_cast<uint16_t>(opc - dstore_0) });
    case astore_0: case astore_1: case astore_2: case astore_3:
      return Insn(IC::astore, { .index = static_cast<uint16_t>(opc - astore_0) });
    case iastore: return Insn(IC::iastore);
    case lastore: return Insn(IC::lastore);
    case fastore: return Insn(IC::fastore);
    case dastore: return Insn(IC::dastore);
    case aastore: return Insn(IC::aastore);
    case bastore: return Insn(IC::bastore);
    case castore: return Insn(IC::castore);
    case sastore: return Insn(IC::sastore);
    case pop: return Insn(IC::pop);
    case pop2: return Insn(IC::pop2);
    case dup: return Insn(IC::dup);
    case dup_x1: return Insn(IC::dup_x1);
    case dup_x2: return Insn(IC::dup_x2);
    case dup2: return Insn(IC::dup2);
    case dup2_x1: return Insn(IC::dup2_x1);
    case dup2_x2: return Insn(IC::dup2_x2);
    case swap: return Insn(IC::swap);
    case iadd: return Insn(IC::iadd);
    case ladd: return Insn(IC::ladd);
    case fadd: return Insn(IC::fadd);
    case dadd: return Insn(IC::dadd);
    case isub: return Insn(IC::isub);
    case lsub: return Insn(IC::lsub);
    case fsub: return Insn(IC::fsub);
    case dsub: return Insn(IC::dsub);
    case imul: return Insn(IC::imul);
    case lmul: return Insn(IC::lmul);
    case fmul: return Insn(IC::fmul);
    case dmul: return Insn(IC::dmul);
    case idiv: return Insn(IC::idiv);
    case ldiv: return Insn(IC::ldiv);
    case fdiv: return Insn(IC::fdiv);
    case ddiv: return Insn(IC::ddiv);
    case irem: return Insn(IC::irem);
    case lrem: return Insn(IC::lrem);
    case frem: return Insn(IC::frem);
    case drem: return Insn(IC::drem);
    case ineg: return Insn(IC::ineg);
    case lneg: return Insn(IC::lneg);
    case fneg: return Insn(IC::fneg);
    case dneg: return Insn(IC::dneg);
    case ishl: return Insn(IC::ishl);
    case lshl: return Insn(IC::lshl);
    case ishr: return Insn(IC::ishr);
    case lshr: return Insn(IC::lshr);
    case iushr: return Insn(IC::iushr);
    case lushr: return Insn(IC::lushr);
    case iand: return Insn(IC::iand);
    case land: return Insn(IC::land);
    case ior: return Insn(IC::ior);
    case lor: return Insn(IC::lor);
    case ixor: return Insn(IC::ixor);
    case lxor: return Insn(IC::lxor);
    case iinc: {
      auto index = reader->NextU8("iinc index");
      auto imm = reader->NextI8("iinc immediate");
      return Insn(IC::iinc, { .iinc = { index, imm }});
    }
    case i2l: return Insn(IC::i2l);
    case i2f: return Insn(IC::i2f);
    case i2d: return Insn(IC::i2d);
    case l2i: return Insn(IC::l2i);
    case l2f: return Insn(IC::l2f);
    case l2d: return Insn(IC::l2d);
    case f2i: return Insn(IC::f2i);
    case f2l: return Insn(IC::f2l);
    case f2d: return Insn(IC::f2d);
    case d2i: return Insn(IC::d2i);
    case d2l: return Insn(IC::d2l);
    case d2f: return Insn(IC::d2f);
    case i2b: return Insn(IC::i2b);
    case i2c: return Insn(IC::i2c);
    case i2s: return Insn(IC::i2s);
    case lcmp: return Insn(IC::lcmp);
    case fcmpl: return Insn(IC::fcmpl);
    case fcmpg: return Insn(IC::fcmpg);
    case dcmpl: return Insn(IC::dcmpl);
    case dcmpg: return Insn(IC::dcmpg);

    // TEMPORARY: Program counter offsets will be converted later to instruction indices
    case ifeq: return Insn(IC::ifeq, { .index = reader->NextU16("ifeq offset") });
    case ifne: return Insn(IC::ifne, { .index = reader->NextU16("ifne offset") });
    case iflt: return Insn(IC::iflt, { .index = reader->NextU16("iflt offset") });
    case ifge: return Insn(IC::ifge, { .index = reader->NextU16("ifge offset") });
    case ifgt: return Insn(IC::ifgt, { .index = reader->NextU16("ifgt offset") });
    case ifle: return Insn(IC::ifle, { .index = reader->NextU16("ifle offset") });
    case if_icmpeq: return Insn(IC::if_icmpeq, { .index = reader->NextU16("if_icmpeq offset") });
    case if_icmpne: return Insn(IC::if_icmpne, { .index = reader->NextU16("if_icmpne offset") });
    case if_icmplt: return Insn(IC::if_icmplt, { .index = reader->NextU16("if_icmplt offset") });
    case if_icmpge: return Insn(IC::if_icmpge, { .index = reader->NextU16("if_icmpge offset") });
    case if_icmpgt: return Insn(IC::if_icmpgt, { .index = reader->NextU16("if_icmpgt offset") });
    case if_icmple: return Insn(IC::if_icmple, { .index = reader->NextU16("if_icmple offset") });
    case if_acmpeq: return Insn(IC::if_acmpeq, { .index = reader->NextU16("if_acmpeq offset") });
    case if_acmpne: return Insn(IC::if_acmpne, { .index = reader->NextU16("if_acmpne offset") });
    case goto_: return Insn(IC::goto_, { .imm = reader->NextI16("goto offset") });
    case jsr: return Insn(IC::jsr, { .imm = reader->NextI16("jsr offset") });
    case ret: return Insn(IC::ret, { .index = reader->NextU8("ret index") });
    case tableswitch: {
      // Tableswitch data is 4-byte aligned for some reason
      auto padding = (4 - (reader->GetOffs() % 4)) % 4;
      reader->NextNBytes(padding, "tableswitch padding");

      auto default_offset = reader->NextI32("tableswitch default offset");
      auto low = reader->NextI32("tableswitch low");
      auto high = reader->NextI32("tableswitch high");

      std::vector<int> offsets;
      for (int i = low; i <= high; i++) {
        offsets.push_back(reader->NextI32("tableswitch offset"));
      }

      // TEMPORARY: Will convert offsets to instruction index, and tableswitch index to tableswitch pointer once
      // all have been allocated
      auto ts_index = ctx->MakeTableswitch({
        SwitchDataBase { .m_default_target = default_offset, .m_targets = std::move(offsets) }, .m_low = low, .m_high = high
      });
      return Insn(IC::tableswitch, { .imm = ts_index });
    }
    case lookupswitch: {
      auto padding = (4 - (reader->GetOffs() % 4)) % 4;
      reader->NextNBytes(padding, "lookupswitch padding");

      auto default_offset = reader->NextI32("lookupswitch default offset");
      auto npairs = reader->NextI32("lookupswitch npairs");

      std::vector<int> keys, pairs;
      for (int i = 0; i < npairs; i++) {
        auto match = reader->NextI32("lookupswitch match");
        auto offset = reader->NextI32("lookupswitch offset");

        keys.push_back(match);
        pairs.push_back(offset);
      }

      auto ls_index = ctx->MakeLookupswitch({
        { .m_default_target = default_offset, .m_targets = std::move(pairs) }, .m_keys = std::move(keys)
      });
      return Insn(IC::lookupswitch, { .imm = ls_index });
    }
    case ireturn: return Insn(IC::ireturn);
    case lreturn: return Insn(IC::lreturn);
    case freturn: return Insn(IC::freturn);
    case dreturn: return Insn(IC::dreturn);
    case areturn: return Insn(IC::areturn);
    case return_: return Insn(IC::return_);

    case getstatic: return Insn(IC::getstatic, { .index = reader->NextU16("getstatic index") });
    case putstatic: return Insn(IC::putstatic, { .index = reader->NextU16("putstatic index") });

    case getfield: return Insn(IC::getfield, { .index = reader->NextU16("getfield index") });
    case putfield: return Insn(IC::putfield, { .index = reader->NextU16("putfield index") });

    case invokevirtual: return Insn(IC::invokevirtual, { .index = reader->NextU16("invokevirtual index") });
    case invokespecial: return Insn(IC::invokespecial, { .index = reader->NextU16("invokespecial index") });
    case invokestatic: return Insn(IC::invokestatic, { .index = reader->NextU16("invokestatic index") });
    case invokeinterface: {
      auto index = reader->NextU16("invokeinterface index");
      auto count = reader->NextU8("invokeinterface count");
      reader->NextU8("invokeinterface padding");
      return Insn(IC::invokeinterface, { .ii = { index, count } });
    }
    case invokedynamic: {
      auto index = reader->NextU16("invokedynamic index");
      reader->NextU16("invokedynamic padding");
      return Insn(IC::invokedynamic, { .index = index });
    }
    case new_: {
      auto index = reader->NextU16("new index");
      return Insn(IC::new_, { .index = index });
    }
    case newarray: return Insn(IC::newarray, { .atype = CheckAType(reader->NextU8("newarray atype")) });
    case anewarray: return Insn(IC::anewarray, { .index = reader->NextU16("anewarray index") });
    case arraylength: return Insn(IC::arraylength);
    case athrow: return Insn(IC::athrow);
    case checkcast: return Insn(IC::checkcast, { .index = reader->NextU16("checkcast index") });
    case instanceof: return Insn(IC::instanceof, { .index = reader->NextU16("instanceof index") });
    case monitorenter: return Insn(IC::monitorenter);
    case monitorexit: return Insn(IC::monitorexit);

    case wide: {
      switch (auto widened_opc = reader->NextU8("wide opcode")) {
        case iload: return Insn(IC::iload, { .index = reader->NextU16("wide iload index") });
        case lload: return Insn(IC::lload, { .index = reader->NextU16("wide lload index") });
        case fload: return Insn(IC::fload, { .index = reader->NextU16("wide fload index") });
        case dload: return Insn(IC::dload, { .index = reader->NextU16("wide dload index") });
        case aload: return Insn(IC::aload, { .index = reader->NextU16("wide aload index") });
        case istore: return Insn(IC::istore, { .index = reader->NextU16("wide istore index") });
        case lstore: return Insn(IC::lstore, { .index = reader->NextU16("wide lstore index") });
        case fstore: return Insn(IC::fstore, { .index = reader->NextU16("wide fstore index") });
        case dstore: return Insn(IC::dstore, { .index = reader->NextU16("wide dstore index") });
        case astore: return Insn(IC::astore, { .index = reader->NextU16("wide astore index") });
        case iinc: return Insn(IC::iinc, { .iinc = { reader->NextU16("wide iinc index"), reader->NextI16("wide iinc immediate") } });
        case ret: return Insn(IC::ret, { .index = reader->NextU16("wide ret index") });

        default:
          throw VerifyError("Unknown wide opcode " + std::to_string(widened_opc), reader->GetOriginalOffs());
      }
    }

    case multianewarray: {
      auto index = reader->NextU16("multianewarray index");
      auto dimensions = reader->NextU8("multianewarray dimensions");
      return Insn(IC::multianewarray, { .mna = { index, dimensions } });
    }

    case ifnull: return Insn(IC::ifnull, { .index = reader->NextU16("ifnull offset") });
    case ifnonnull: return Insn(IC::ifnonnull, { .index = reader->NextU16("ifnonnull offset") });

    case goto_w: return Insn(IC::goto_, { .imm = reader->NextI32("goto_w offset") });
    case jsr_w: return Insn(IC::jsr, { .imm = reader->NextI32("jsr_w offset") });

    default:
      throw VerifyError("Unknown opcode " + std::to_string(opc), reader->GetOriginalOffs());
  }
}

bool Insn::IsIf() const {
  return m_code >= InsnCode::if_acmpeq && m_code <= InsnCode::ifnull;
}

bool Insn::ContainsBranch() const {
  return m_code >= InsnCode::goto_ && m_code <= InsnCode::ifnull || m_code == InsnCode::tableswitch || m_code == InsnCode::lookupswitch;
}

BootstrapMethodsAttribute BootstrapMethodsAttribute::parse(ByteReader *reader) {
  BootstrapMethodsAttribute attr;

  uint16_t num_bootstrap_methods = reader->NextU16("num bootstrap methods");
  for (int i = 0; i < num_bootstrap_methods; i++) {
    BootstrapMethod method {
      .m_method_ref = reader->NextU16("bootstrap method ref")
    };

    uint16_t num_bootstrap_arguments = reader->NextU16("num bootstrap arguments");
    for (int j = 0; j < num_bootstrap_arguments; j++) {
      method.m_arguments.push_back(reader->NextU16("bootstrap argument"));
    }

    attr.m_methods.push_back(method);
  }

  return attr;
}

std::optional<int> CodeAttribute::ProgramCounterToInsnIndex(int pc) const {
  if (0 <= pc && pc < m_pc_to_index.size()) {
    return m_pc_to_index[pc];
  }
  return std::nullopt;
}

CodeAttribute CodeAttribute::parse(ByteReader *reader, ParseContext *parse_context) {
  auto max_stack = reader->NextU16("max stack");
  auto max_locals = reader->NextU16("max locals");

  uint32_t code_length = reader->NextU32("code length");
  auto code_reader = reader->slice("code", code_length);

  std::vector<std::optional<uint16_t>> pc_to_index(code_length);

  std::vector<Insn> code;
  int instruction_index = 0;
  while (!code_reader.Eof()) {
    uint16_t pc = code_reader.GetOffs();

    auto instruction = Insn::parse(&code_reader, parse_context);
    instruction.SetPC(pc);

    code.push_back(instruction);
    pc_to_index[pc] = instruction_index++;
  }

  const auto CheckedPcToIndex = [&](int pc) {
    if (pc < 0 || pc >= pc_to_index.size() || !pc_to_index[pc].has_value())
      throw VerifyError("Invalid program counter", pc);
    return pc_to_index[pc].value();
  };

  // Now that we know where all instructions are, replace PC offsets with branch indices
  for (auto& instruction : code) {
    int pc = instruction.GetPC();

    const auto PcToIndex = [&](int& branch) {
      branch = CheckedPcToIndex(pc + branch);
    };

    if (instruction.ContainsBranch()) {
      if (instruction.GetCode() == InsnCode::lookupswitch) {
        parse_context->m_lookupswitches.at(instruction.m_data.imm).TransformTargets(PcToIndex);
      } else if (instruction.GetCode() == InsnCode::tableswitch) {
        parse_context->m_tableswitches.at(instruction.m_data.imm).TransformTargets(PcToIndex);
      } else if (instruction.GetCode() == InsnCode::jsr || instruction.GetCode() == InsnCode::goto_) {
        instruction.m_data.index = CheckedPcToIndex(pc + instruction.m_data.imm);
      } else {
        instruction.m_data.index = CheckedPcToIndex(pc + static_cast<int16_t>(instruction.m_data.index));
      }
    }
  }

  reader->NextNBytes(code_length, "code");

  uint16_t exception_table_length = reader->NextU16("exception table length");
  ExceptionTableAttribute table;

  for (int i = 0; i < exception_table_length; i++) {
    ExceptionTableEntry ent {
      .m_start = CheckedPcToIndex(reader->NextU16("start pc")),
      .m_end = CheckedPcToIndex(reader->NextU16("end pc")),
      .m_handler = CheckedPcToIndex(reader->NextU16("handler pc")),
      .m_catch_type = reader->NextU16("catch type")
    };
    table.m_exceptions.push_back(ent);
  }

  std::optional<LineNumberTable> lnt;

  uint16_t attributes_count = reader->NextU16("attributes count");
  for (int i = 0; i < attributes_count; i++) {
    auto name_index = reader->NextU16("attribute name index");
    auto length = reader->NextU32("attribute length");

    if (parse_context->cp->GetUtf8(name_index) == "LineNumberTable") {
      lnt = LineNumberTable {};
      uint16_t table_length = reader->NextU16("line number table length");

      for (int j = 0; j < table_length; j++) {
        LineNumberTableEntry ent {
          .m_start = CheckedPcToIndex(reader->NextU16("start pc")),
          .m_line_number = reader->NextU16("line number")
        };
        lnt.value().m_entries.push_back(ent);
      }
    } else {
      reader->NextNBytes(length, "attribute");
    }
  }

  return CodeAttribute {
    .m_max_stack = max_stack,
    .m_max_locals = max_locals,
    .m_code = code,
    .m_exceptions = {},
    .m_exception_table = table,
    .m_line_number_table = lnt
  };
}

FieldInfo FieldInfo::parse(ByteReader *reader, ParseContext *ctx) {
  FieldInfo info;

  info.m_access_flags = static_cast<FieldAccessFlags>(reader->NextU16("field access flags"));
  info.m_name_index = reader->NextU16("field name index");
  info.m_descriptor_index = reader->NextU16("field descriptor index");

  uint16_t attributes_count = reader->NextU16("field attributes count");
  for (int i = 0; i < attributes_count; i++) {
    auto name_index = reader->NextU16("field attribute name index");
    auto length = reader->NextU32("field attribute length");

    if (ctx->cp->Get<EntryUtf8>(name_index)->m_value == "ConstantValue") {
      info.m_constant_value = ConstantValueAttribute { .m_index = reader->NextU16("constant value index") };
    } else {
      reader->NextNBytes(length, "field attribute");
    }
  }

  return info;
}

MethodInfo MethodInfo::parse(ByteReader *reader, ParseContext *parse_context) {
  MethodInfo info;

  info.m_access_flags = static_cast<MethodAccessFlags>(reader->NextU16("method access flags"));
  info.m_name_index = reader->NextU16("method name index");
  info.m_descriptor_index = reader->NextU16("method descriptor index");

  // std::cout << "Parsing method " << parse_context->cp->GetUtf8(info.m_name_index) << '\n';

  uint16_t attributes_count = reader->NextU16("method attributes count");
  for (int i = 0; i < attributes_count; i++) {
    auto name_index = reader->NextU16("method attribute name index");
    auto length = reader->NextU32("method attribute length");

    const auto &attrib_name = parse_context->cp->Get<EntryUtf8>(name_index)->m_value;

    if (attrib_name == "Code") {
      info.m_code = CodeAttribute::parse(reader, parse_context);
    } else {
      reader->NextNBytes(length, "method attribute");
    }
  }

  return info;
}

void MethodInfo::FixupSwitchInstructions(ParseContext *ctx) {
  if (!m_code.has_value()) return;

  auto& code = m_code.value();
  for (auto& insn : code.m_code) {
    if (insn.GetCode() == InsnCode::tableswitch) {
      insn.m_data.ts = &ctx->m_tableswitches.at(insn.m_data.imm);
    } else if (insn.GetCode() == InsnCode::lookupswitch) {
      insn.m_data.ls = &ctx->m_lookupswitches.at(insn.m_data.imm);
    }
  }
}

long ParseContext::MakeTableswitch(TableswitchData &&data) {
  m_tableswitches.push_back(data);
  return static_cast<long>(m_tableswitches.size()) - 1;
}

long ParseContext::MakeLookupswitch(LookupswitchData &&data) {
  m_lookupswitches.push_back(data);
  return static_cast<long>(m_lookupswitches.size()) - 1;
}

Insn::Insn(InsnCode code) : m_code(code) {
  m_data.imm = 0;
}

Insn::Insn(InsnCode code, decltype(Insn::m_data) data) : m_code(code), m_data(data) {}

InsnCode Insn::GetCode() const {
  return m_code;
}

uint16_t Insn::Index() const {
  assert(m_code >= InsnCode::dload && m_code <= InsnCode::ifnull);
  return m_data.index;
}

uint16_t Insn::GetPC() const {
  return m_pc;
}

void Insn::SetPC(uint16_t pc) {
  m_pc = pc;
}

double Insn::GetDoubleData() const {
  assert(m_code == InsnCode::dconst);
  return m_data.d_imm;
}

int Insn::GetIntData() const {
  assert(m_code == InsnCode::iconst);
  return static_cast<int>(m_data.imm);
}

long Insn::GetLongData() const {
  assert(m_code == InsnCode::lconst);
  return m_data.imm;
}

float Insn::GetFloatData() const {
  assert(m_code == InsnCode::fconst);
  return m_data.f_imm;
}

const TableswitchData * Insn::GetTableswitchData() const {
  assert(m_code == InsnCode::tableswitch);
  return m_data.ts;
}

IIncData Insn::GetIIncData() const {
  assert(m_code == InsnCode::iinc);
  return m_data.iinc;
}

MultianewarrayData Insn::GetMultianewarrayData() const {
  assert(m_code == InsnCode::multianewarray);
  return m_data.mna;
}

const LookupswitchData * Insn::GetLookupswitchData() const {
  assert(m_code == InsnCode::lookupswitch);
  return m_data.ls;
}

const InvokeInterfaceData * Insn::GetInvokeInterfaceData() const {
  assert(m_code == InsnCode::invokeinterface);
  return &m_data.ii;
}

PrimitiveType Insn::GetArrayType() const {
  assert(m_code == InsnCode::newarray);
  return m_data.atype;
}

VerifyError::VerifyError(const std::string &what, int offset) : std::runtime_error(what), m_offset(offset) {}

const char *CodeName(InsnCode code) {
  using I = InsnCode;
  switch (code) {
    case I::aaload: return "aaload";
    case I::aastore: return "aastore";
    case I::aconst_null: return "aconst_null";
    case I::areturn: return "areturn";
    case I::arraylength: return "arraylength";
    case I::athrow: return "athrow";
    case I::baload: return "baload";
    case I::bastore: return "bastore";
    case I::caload: return "caload";
    case I::castore: return "castore";
    case I::d2f: return "d2f";
    case I::d2i: return "d2i";
    case I::d2l: return "d2l";
    case I::dadd: return "dadd";
    case I::daload: return "daload";
    case I::dastore: return "dastore";
    case I::dcmpg: return "dcmpg";
    case I::dcmpl: return "dcmpl";
    case I::ddiv: return "ddiv";
    case I::dmul: return "dmul";
    case I::dneg: return "dneg";
    case I::drem: return "drem";
    case I::dreturn: return "dreturn";
    case I::dsub: return "dsub";
    case I::dup: return "dup";
    case I::dup_x1: return "dup_x1";
    case I::dup_x2: return "dup_x2";
    case I::dup2: return "dup2";
    case I::dup2_x1: return "dup2_x1";
    case I::dup2_x2: return "dup2_x2";
    case I::f2d: return "f2d";
    case I::f2i: return "f2i";
    case I::f2l: return "f2l";
    case I::fadd: return "fadd";
    case I::faload: return "faload";
    case I::fastore: return "fastore";
    case I::fcmpg: return "fcmpg";
    case I::fcmpl: return "fcmpl";
    case I::fdiv: return "fdiv";
    case I::fmul: return "fmul";
    case I::fneg: return "fneg";
    case I::frem: return "frem";
    case I::freturn: return "freturn";
    case I::fsub: return "fsub";
    case I::i2b: return "i2b";
    case I::i2c: return "i2c";
    case I::i2d: return "i2d";
    case I::i2f: return "i2f";
    case I::i2l: return "i2l";
    case I::i2s: return "i2s";
    case I::iadd: return "iadd";
    case I::iaload: return "iaload";
    case I::iand: return "iand";
    case I::iastore: return "iastore";
    case I::idiv: return "idiv";
    case I::imul: return "imul";
    case I::ineg: return "ineg";
    case I::ior: return "ior";
    case I::irem: return "irem";
    case I::ireturn: return "ireturn";
    case I::ishl: return "ishl";
    case I::ishr: return "ishr";
    case I::isub: return "isub";
    case I::iushr: return "iushr";
    case I::ixor: return "ixor";
    case I::l2d: return "l2d";
    case I::l2f: return "l2f";
    case I::l2i: return "l2i";
    case I::ladd: return "ladd";
    case I::laload: return "laload";
    case I::land: return "land";
    case I::lastore: return "lastore";
    case I::lcmp: return "lcmp";
    case I::ldc: return "ldc";
    case I::ldc2_w: return "ldc2_w";
    case I::ldiv: return "ldiv";
    case I::lmul: return "lmul";
    case I::lneg: return "lneg";
    case I::lor: return "lor";
    case I::lrem: return "lrem";
    case I::lreturn: return "lreturn";
    case I::lshl: return "lshl";
    case I::lshr: return "lshr";
    case I::lsub: return "lsub";
    case I::lushr: return "lushr";
    case I::lxor: return "lxor";
    case I::monitorenter: return "monitorenter";
    case I::monitorexit: return "monitorexit";
    case I::nop: return "nop";
    case I::pop: return "pop";
    case I::pop2: return "pop2";
    case I::return_: return "return_";
    case I::saload: return "saload";
    case I::sastore: return "sastore";
    case I::swap: return "swap";
    case I::dload: return "dload";
    case I::fload: return "fload";
    case I::iload: return "iload";
    case I::lload: return "lload";
    case I::dstore: return "dstore";
    case I::fstore: return "fstore";
    case I::istore: return "istore";
    case I::lstore: return "lstore";
    case I::aload: return "aload";
    case I::astore: return "astore";
    case I::anewarray: return "anewarray";
    case I::checkcast: return "checkcast";
    case I::getfield: return "getfield";
    case I::getstatic: return "getstatic";
    case I::instanceof: return "instanceof";
    case I::invokedynamic: return "invokedynamic";
    case I::new_: return "new_";
    case I::putfield: return "putfield";
    case I::putstatic: return "putstatic";
    case I::invokevirtual: return "invokevirtual";
    case I::invokespecial: return "invokespecial";
    case I::invokestatic: return "invokestatic";
    case I::goto_: return "goto_";
    case I::jsr: return "jsr";
    case I::ret: return "ret";
    case I::if_acmpeq: return "if_acmpeq";
    case I::if_acmpne: return "if_acmpne";
    case I::if_icmpeq: return "if_icmpeq";
    case I::if_icmpne: return "if_icmpne";
    case I::if_icmplt: return "if_icmplt";
    case I::if_icmpge: return "if_icmpge";
    case I::if_icmpgt: return "if_icmpgt";
    case I::if_icmple: return "if_icmple";
    case I::ifeq: return "ifeq";
    case I::ifne: return "ifne";
    case I::iflt: return "iflt";
    case I::ifge: return "ifge";
    case I::ifgt: return "ifgt";
    case I::ifle: return "ifle";
    case I::ifnonnull: return "ifnonnull";
    case I::ifnull: return "ifnull";
    case I::iconst: return "iconst";
    case I::dconst: return "dconst";
    case I::fconst: return "fconst";
    case I::lconst: return "lconst";
    case I::iinc: return "iinc";
    case I::invokeinterface: return "invokeinterface";
    case I::multianewarray: return "multianewarray";
    case I::newarray: return "newarray";
    case I::tableswitch: return "tableswitch";
    case I::lookupswitch: return "lookupswitch";
  }

  throw std::runtime_error("Unreachable");
}

Classfile Classfile::parse(ByteReader *reader) {
  uint32_t magic = reader->NextU32("magic");
  if (magic != 0xCAFEBABE) {
    throw VerifyError("Invalid magic number", 0);
  }

  uint16_t minor = reader->NextU16("minor version");
  uint16_t major = reader->NextU16("major version");

  ClassfileVersion version { major, minor };

  ConstantPool cp = ConstantPool::parse(reader);

  std::vector<LookupswitchData> ls;
  std::vector<TableswitchData> ts;

  ParseContext ctx { ts, ls, &cp };

  auto access_flags = static_cast<AccessFlags>(reader->NextU16("access flags"));
  uint16_t this_class = reader->NextU16("this class");
  assert(cp.Has<EntryClass>(this_class));

  uint16_t super_class = reader->NextU16("super class");
  assert(super_class == 0 || cp.Has<EntryClass>(super_class));

  uint16_t interfaces_count = reader->NextU16("interfaces count");
  std::vector<uint16_t> interfaces;

  for (int i = 0; i < interfaces_count; i++) {
    uint16_t v = reader->NextU16("interface");
    interfaces.push_back(v);
    assert(cp.Has<EntryClass>(v));
  }

  uint16_t fields_count = reader->NextU16("fields count");
  std::vector<FieldInfo> fields;

  for (int i = 0; i < fields_count; i++) {
    fields.push_back(FieldInfo::parse(reader, &ctx));
  }

  uint16_t methods_count = reader->NextU16("methods count");
  std::vector<MethodInfo> methods;

  for (int i = 0; i < methods_count; i++) {
    methods.push_back(MethodInfo::parse(reader, &ctx));
  }

  uint16_t attributes_count = reader->NextU16("attributes count");
  std::optional<BootstrapMethodsAttribute> bootstrap;

  for (int i = 0; i < attributes_count; i++) {
    const auto& name = cp.GetUtf8(reader->NextU16("attribute name"));
    auto length = reader->NextU32("attribute length");

    if (name == "BootstrapMethods") {
      bootstrap = BootstrapMethodsAttribute::parse(reader);
    } else {
      reader->NextNBytes(length, "attribute data");
    }
  }

  for (auto & method : methods) {
    method.FixupSwitchInstructions(&ctx);
  }

  Classfile cf { std::move(cp) };

  cf.m_version = version;
  cf.m_access_flags = access_flags;
  cf.m_this_class = this_class;
  cf.m_super_class = super_class;
  cf.m_interfaces = std::move(interfaces);
  cf.m_fields = std::move(fields);
  cf.m_methods = std::move(methods);
  cf.m_bootstrap_methods = std::move(bootstrap);

  return cf;
}

std::string Classfile::ToString() const {
  std::stringstream ss;

  ss << "class " << GetName() << "\n";

  return ss.str();
}

const std::string & Classfile::GetName() const {
  return m_cp.GetUtf8(m_cp.Get<EntryClass>(m_this_class)->m_name_index);
}

std::optional<std::string> Classfile::GetSuperclassName() const {
  return m_super_class ? (std::optional { m_cp.GetUtf8(m_cp.Get<EntryClass>(m_super_class)->m_name_index) }) : std::nullopt;
}

std::vector<std::string> Classfile::GetInterfaceNames() const {
  std::vector<std::string> result;

  for (auto idx : m_interfaces) {
    result.push_back(m_cp.GetUtf8(m_cp.Get<EntryClass>(idx)->m_name_index));
  }

  return result;
}
} // bjvm