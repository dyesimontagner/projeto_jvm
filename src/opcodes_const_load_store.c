#include "opcodes.h"
#include "engine.h"

// Constantes
void iconst_m1_op(JVM* jvm, Frame* frame) { operand_stack_push_int(&frame->operand_stack, -1); }
void iconst_0_op(JVM* jvm, Frame* frame)  { operand_stack_push_int(&frame->operand_stack, 0); }
void iconst_1_op(JVM* jvm, Frame* frame)  { operand_stack_push_int(&frame->operand_stack, 1); }
void iconst_2_op(JVM* jvm, Frame* frame)  { operand_stack_push_int(&frame->operand_stack, 2); }
void iconst_3_op(JVM* jvm, Frame* frame)  { operand_stack_push_int(&frame->operand_stack, 3); }
void iconst_4_op(JVM* jvm, Frame* frame)  { operand_stack_push_int(&frame->operand_stack, 4); }
void iconst_5_op(JVM* jvm, Frame* frame)  { operand_stack_push_int(&frame->operand_stack, 5); }
void bipush_op(JVM* jvm, Frame* frame) {
    int8_t val = (int8_t)frame->code[frame->pc++];
    operand_stack_push_int(&frame->operand_stack, (int32_t)val);
}

// Loads
void iload_op(JVM* jvm, Frame* frame) {
    u1 idx = frame->code[frame->pc++];
    operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, idx));
}
void iload_0_op(JVM* jvm, Frame* frame) { operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 0)); }
void iload_1_op(JVM* jvm, Frame* frame) { operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 1)); }
void iload_2_op(JVM* jvm, Frame* frame) { operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 2)); }
void iload_3_op(JVM* jvm, Frame* frame) { operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 3)); }

// Stores
void istore_op(JVM* jvm, Frame* frame) {
    u1 idx = frame->code[frame->pc++];
    local_var_set_int(&frame->local_vars, idx, operand_stack_pop_int(&frame->operand_stack));
}
void istore_0_op(JVM* jvm, Frame* frame) { local_var_set_int(&frame->local_vars, 0, operand_stack_pop_int(&frame->operand_stack)); }
void istore_1_op(JVM* jvm, Frame* frame) { local_var_set_int(&frame->local_vars, 1, operand_stack_pop_int(&frame->operand_stack)); }
void istore_2_op(JVM* jvm, Frame* frame) { local_var_set_int(&frame->local_vars, 2, operand_stack_pop_int(&frame->operand_stack)); }
void istore_3_op(JVM* jvm, Frame* frame) { local_var_set_int(&frame->local_vars, 3, operand_stack_pop_int(&frame->operand_stack)); }
