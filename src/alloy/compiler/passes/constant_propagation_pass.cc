/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <alloy/compiler/passes/constant_propagation_pass.h>

using namespace alloy;
using namespace alloy::compiler;
using namespace alloy::compiler::passes;
using namespace alloy::hir;


ConstantPropagationPass::ConstantPropagationPass() :
    Pass() {
}

ConstantPropagationPass::~ConstantPropagationPass() {
}

int ConstantPropagationPass::Run(FunctionBuilder* builder) {
  // Once ContextPromotion has run there will likely be a whole slew of
  // constants that can be pushed through the function.
  // Example:
  //   store_context +100, 1000
  //   v0 = load_context +100
  //   v1 = add v0, v0
  //   store_context +200, v1
  // after PromoteContext:
  //   store_context +100, 1000
  //   v0 = 1000
  //   v1 = add v0, v0
  //   store_context +200, v1
  // after PropagateConstants:
  //   store_context +100, 1000
  //   v0 = 1000
  //   v1 = add 1000, 1000
  //   store_context +200, 2000
  // A DCE run after this should clean up any of the values no longer needed.

  Block* block = builder->first_block();
  while (block) {
    Instr* i = block->instr_head;
    while (i) {
      Value* v = i->dest;
      switch (i->opcode->num) {
      case OPCODE_CAST:
        if (i->src1.value->IsConstant()) {
          TypeName target_type = v->type;
          v->set_from(i->src1.value);
          v->Cast(target_type);
          i->Remove();
        }
        break;
      case OPCODE_ZERO_EXTEND:
        if (i->src1.value->IsConstant()) {
          TypeName target_type = v->type;
          v->set_from(i->src1.value);
          v->ZeroExtend(target_type);
          i->Remove();
        }
        break;
      case OPCODE_SIGN_EXTEND:
        if (i->src1.value->IsConstant()) {
          TypeName target_type = v->type;
          v->set_from(i->src1.value);
          v->SignExtend(target_type);
          i->Remove();
        }
        break;
      case OPCODE_TRUNCATE:
        if (i->src1.value->IsConstant()) {
          TypeName target_type = v->type;
          v->set_from(i->src1.value);
          v->Truncate(target_type);
          i->Remove();
        }
        break;

      case OPCODE_ADD:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Add(i->src2.value);
          i->Remove();
        }
        break;
      // TODO(benvanik): ADD_CARRY
      case OPCODE_SUB:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Sub(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_MUL:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Mul(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_DIV:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Div(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_REM:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Rem(i->src2.value);
          i->Remove();
        }
        break;
      // case OPCODE_MULADD:
      // case OPCODE_MULSUB
      case OPCODE_NEG:
        if (i->src1.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Neg();
          i->Remove();
        }
        break;
      case OPCODE_ABS:
        if (i->src1.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Abs();
          i->Remove();
        }
        break;
      case OPCODE_SQRT:
        if (i->src1.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Sqrt();
          i->Remove();
        }
        break;
      case OPCODE_RSQRT:
        if (i->src1.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->RSqrt();
          i->Remove();
        }
        break;

      case OPCODE_AND:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->And(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_OR:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Or(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_XOR:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Xor(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_NOT:
        if (i->src1.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Not();
          i->Remove();
        }
        break;
      case OPCODE_SHL:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Shl(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_SHR:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Shr(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_SHA:
        if (i->src1.value->IsConstant() && i->src2.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->Sha(i->src2.value);
          i->Remove();
        }
        break;
      case OPCODE_BYTE_SWAP:
        if (i->src1.value->IsConstant()) {
          v->set_from(i->src1.value);
          v->ByteSwap();
          i->Remove();
        }
        break;
      }
      i = i->next;
    }

    block = block->next;
  }

  return 0;
}
