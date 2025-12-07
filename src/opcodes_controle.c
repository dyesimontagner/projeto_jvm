#include "opcodes.h"
#include "engine.h"

void goto_op(JVM* jvm, Frame* frame) {
    int16_t offset = read_s2(frame);
    frame->pc = (frame->pc - 3) + offset;
}

void ifne_op(JVM* jvm, Frame* frame) {
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val != 0) frame->pc = (frame->pc - 3) + offset;
}

void ifle_op(JVM* jvm, Frame* frame) {
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val <= 0) frame->pc = (frame->pc - 3) + offset;
}

void if_icmpgt_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 > v2) frame->pc = (frame->pc - 3) + offset;
}
void if_icmple_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 <= v2) frame->pc = (frame->pc - 3) + offset;
}
void if_icmpge_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 >= v2) frame->pc = (frame->pc - 3) + offset;
}
