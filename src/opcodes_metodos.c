#include "opcodes.h"
#include "engine.h"
#include <stdio.h>

void invokestatic_op(JVM* jvm, Frame* frame) {
    if (!jvm || !frame) return;
    
    // Ler índice do constant pool (2 bytes)
    u1 byte1 = frame->code[frame->pc++];
    u1 byte2 = frame->code[frame->pc++];
    u2 method_ref_idx = (u2)((byte1 << 8) | byte2);
    
    printf("[invokestatic] Resolving method reference #%d\n", method_ref_idx);
    
    // TODO: Resolver método no constant pool da classe carregada
    // Por agora, apenas avança sem fazer nada (placeholder)
    // Versão futura: procurar método na Method Area e criar novo frame
}

void ireturn_op(JVM* jvm, Frame* frame) {
    int32_t ret_val = operand_stack_pop_int(&frame->operand_stack);
    printf("[ireturn] Retornando: %d\n", ret_val);
    frame_stack_pop(&jvm->frame_stack);
    Frame* prev = jvm->frame_stack.top;
    if (prev) operand_stack_push_int(&prev->operand_stack, ret_val);
}

void return_op(JVM* jvm, Frame* frame) {
    (void)frame;
    printf("[return] Exiting method\n");
    frame_stack_pop(&jvm->frame_stack);
}
