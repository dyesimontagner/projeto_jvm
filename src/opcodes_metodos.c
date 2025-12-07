#include "opcodes.h"
#include "engine.h"

void invokestatic_op(JVM* jvm, Frame* frame) {
    
}

void ireturn_op(JVM* jvm, Frame* frame) {
    int32_t ret_val = operand_stack_pop_int(&frame->operand_stack);
    frame_stack_pop(&jvm->frame_stack);
    Frame* prev = jvm->frame_stack.top;
    if (prev) operand_stack_push_int(&prev->operand_stack, ret_val);
}

void return_op(JVM* jvm, Frame* frame) {
    frame_stack_pop(&jvm->frame_stack);
}
