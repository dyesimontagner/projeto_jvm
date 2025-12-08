#include "opcodes.h"
#include "engine.h"

void goto_op(JVM* jvm, Frame* frame) {
    int16_t offset = read_s2(frame);
    int old_pc = frame->pc - 3;
    frame->pc = old_pc + offset;
    printf("[goto] Jump to offset %d (new PC: %d)\n", offset, frame->pc);
}

void ifne_op(JVM* jvm, Frame* frame) {
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val != 0) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[ifne] %d != 0 (TRUE) -> jump to offset %d\n", val, offset);
    } else {
        printf("[ifne] %d != 0 (FALSE) -> continue\n", val);
    }
}

void ifle_op(JVM* jvm, Frame* frame) {
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val <= 0) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[ifle] %d <= 0 (TRUE) -> jump\n", val);
    } else {
        printf("[ifle] %d <= 0 (FALSE) -> continue\n", val);
    }
}

void if_icmpgt_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 > v2) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[if_icmpgt] %d > %d (TRUE) -> jump\n", v1, v2);
    } else {
        printf("[if_icmpgt] %d > %d (FALSE) -> continue\n", v1, v2);
    }
}
void if_icmple_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 <= v2) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[if_icmple] %d <= %d (TRUE) -> jump\n", v1, v2);
    } else {
        printf("[if_icmple] %d <= %d (FALSE) -> continue\n", v1, v2);
    }
}
void if_icmpge_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 >= v2) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[if_icmpge] %d >= %d (TRUE) -> jump\n", v1, v2);
    } else {
        printf("[if_icmpge] %d >= %d (FALSE) -> continue\n", v1, v2);
    }
}

void ifeq_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val == 0) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[ifeq] %d == 0 (TRUE) -> jump\n", val);
    } else {
        printf("[ifeq] %d == 0 (FALSE) -> continue\n", val);
    }
}

void if_icmpeq_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 == v2) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[if_icmpeq] %d == %d (TRUE) -> jump\n", v1, v2);
    } else {
        printf("[if_icmpeq] %d == %d (FALSE) -> continue\n", v1, v2);
    }
}

void if_icmpne_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 != v2) {
        frame->pc = (frame->pc - 3) + offset;
        printf("[if_icmpne] %d != %d (TRUE) -> jump\n", v1, v2);
    } else {
        printf("[if_icmpne] %d != %d (FALSE) -> continue\n", v1, v2);
    }
}
