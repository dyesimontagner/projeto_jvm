#include "opcodes.h"
#include "engine.h"

void iadd_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, v1 + v2);
}
void isub_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, v1 - v2);
}
void imul_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, v1 * v2);
}
void iinc_op(JVM* jvm, Frame* frame) {
    u1 idx = frame->code[frame->pc++];
    int8_t val = (int8_t)frame->code[frame->pc++];
    int32_t current = local_var_get_int(&frame->local_vars, idx);
    local_var_set_int(&frame->local_vars, idx, current + val);
}
