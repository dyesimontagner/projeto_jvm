#include "opcodes.h"
#include "engine.h"
#include "jvm.h"
#include "frame.h"
#include <stdio.h>

// Função auxiliar local para ler 4 bytes (int32)
static int32_t read_s4(Frame* f) {
    u1 b1 = read_u1(f);
    u1 b2 = read_u1(f);
    u1 b3 = read_u1(f);
    u1 b4 = read_u1(f);
    return (int32_t)((b1 << 24) | (b2 << 16) | (b3 << 8) | b4);
}

// ============================================================================
// INSTRUÇÕES DE DESVIO (BRANCH)
// ============================================================================

void goto_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int16_t offset = read_s2(frame);
    frame->pc = (frame->pc - 3) + offset;
}

// --- Comparação com ZERO (pop value, compare with 0) ---

void ifeq_op(JVM* jvm, Frame* frame) { // 0x99
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val == 0) frame->pc = (frame->pc - 3) + offset;
}

void ifne_op(JVM* jvm, Frame* frame) { // 0x9A
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val != 0) frame->pc = (frame->pc - 3) + offset;
}

void iflt_op(JVM* jvm, Frame* frame) { // 0x9B
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val < 0) frame->pc = (frame->pc - 3) + offset;
}

void ifge_op(JVM* jvm, Frame* frame) { // 0x9C
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val >= 0) frame->pc = (frame->pc - 3) + offset;
}

void ifgt_op(JVM* jvm, Frame* frame) { // 0x9D
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val > 0) frame->pc = (frame->pc - 3) + offset;
}

void ifle_op(JVM* jvm, Frame* frame) { // 0x9E
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val <= 0) frame->pc = (frame->pc - 3) + offset;
}

// --- Comparação entre dois inteiros (pop v2, pop v1, compare v1 with v2) ---

void if_icmpeq_op(JVM* jvm, Frame* frame) { // 0x9F
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 == v2) frame->pc = (frame->pc - 3) + offset;
}

void if_icmpne_op(JVM* jvm, Frame* frame) { // 0xA0
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 != v2) frame->pc = (frame->pc - 3) + offset;
}

void if_icmplt_op(JVM* jvm, Frame* frame) { // 0xA1
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 < v2) frame->pc = (frame->pc - 3) + offset;
}

void if_icmpge_op(JVM* jvm, Frame* frame) { // 0xA2
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 >= v2) frame->pc = (frame->pc - 3) + offset;
}

void if_icmpgt_op(JVM* jvm, Frame* frame) { // 0xA3
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 > v2) frame->pc = (frame->pc - 3) + offset;
}

void if_icmple_op(JVM* jvm, Frame* frame) { // 0xA4
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 <= v2) frame->pc = (frame->pc - 3) + offset;
}

// --- Comparação de Referências ---

void if_acmpeq_op(JVM* jvm, Frame* frame) { // 0xA5
    (void)jvm;
    void* v2 = operand_stack_pop_reference(&frame->operand_stack);
    void* v1 = operand_stack_pop_reference(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 == v2) frame->pc = (frame->pc - 3) + offset;
}

void if_acmpne_op(JVM* jvm, Frame* frame) { // 0xA6
    (void)jvm;
    void* v2 = operand_stack_pop_reference(&frame->operand_stack);
    void* v1 = operand_stack_pop_reference(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 != v2) frame->pc = (frame->pc - 3) + offset;
}

// --- Comparação com NULL ---

void ifnull_op(JVM* jvm, Frame* frame) { // 0xC6
    (void)jvm;
    void* val = operand_stack_pop_reference(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val == NULL) frame->pc = (frame->pc - 3) + offset;
}

void ifnonnull_op(JVM* jvm, Frame* frame) { // 0xC7
    (void)jvm;
    void* val = operand_stack_pop_reference(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val != NULL) frame->pc = (frame->pc - 3) + offset;
}

// ============================================================================
// SWITCH (TABLESWITCH e LOOKUPSWITCH)
// ============================================================================

void tableswitch_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u4 opcode_pc = frame->pc - 1;
    u1 padding = (4 - (frame->pc % 4)) % 4;
    for (int i = 0; i < padding; i++) read_u1(frame);

    int32_t default_offset = read_s4(frame);
    int32_t low = read_s4(frame);
    int32_t high = read_s4(frame);
    int32_t num_offsets = high - low + 1;

    int32_t index = operand_stack_pop_int(&frame->operand_stack);

    if (index >= low && index <= high) {
        int32_t target_offset = 0;
        for (int i = 0; i < num_offsets; i++) {
            int32_t off = read_s4(frame);
            if (i == (index - low)) target_offset = off;
        }
        frame->pc = opcode_pc + target_offset;
    } else {
        frame->pc = opcode_pc + default_offset;
    }
}

void lookupswitch_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u4 opcode_pc = frame->pc - 1;
    u1 padding = (4 - (frame->pc % 4)) % 4;
    for (int i = 0; i < padding; i++) read_u1(frame);

    int32_t default_offset = read_s4(frame);
    int32_t npairs = read_s4(frame);

    int32_t key = operand_stack_pop_int(&frame->operand_stack);
    int32_t target_offset = default_offset;
    
    for (int i = 0; i < npairs; i++) {
        int32_t match = read_s4(frame);
        int32_t offset = read_s4(frame);
        if (key == match) target_offset = offset;
    }

    frame->pc = opcode_pc + target_offset;
}
