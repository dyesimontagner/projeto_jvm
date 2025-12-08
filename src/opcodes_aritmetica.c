#include "opcodes.h"
#include "engine.h"
#include <stdio.h>

void iadd_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int32_t result = v1 + v2;
    printf("[iadd] %d + %d = %d\n", v1, v2, result);
    operand_stack_push_int(&frame->operand_stack, result);
}
void isub_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int32_t result = v1 - v2;
    printf("[isub] %d - %d = %d\n", v1, v2, result);
    operand_stack_push_int(&frame->operand_stack, result);
}
void imul_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int32_t result = v1 * v2;
    printf("[imul] %d * %d = %d\n", v1, v2, result);
    operand_stack_push_int(&frame->operand_stack, result);
}
void iinc_op(JVM* jvm, Frame* frame) {
    u1 idx = frame->code[frame->pc++];
    int8_t val = (int8_t)frame->code[frame->pc++];
    int32_t current = local_var_get_int(&frame->local_vars, idx);
    int32_t newval = current + val;
    printf("[iinc] var[%d] += %d (%d -> %d)\n", idx, val, current, newval);
    local_var_set_int(&frame->local_vars, idx, newval);
}

void idiv_op(JVM* jvm, Frame* frame) {
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    if (v2 == 0) {
        fprintf(stderr, "[idiv] ArithmeticException: division by zero\n");
        if (jvm) {
            jvm->exit_code = 1;
            jvm->running = false;
        }
        return;
    }
    int32_t result = v1 / v2;
    printf("[idiv] %d / %d = %d\n", v1, v2, result);
    operand_stack_push_int(&frame->operand_stack, result);
}
