#include "opcodes.h"
#include "jvm.h"
#include "frame.h"
#include "engine.h" // Para read_u1, read_s2
#include <stdio.h>

// ============================================================================
// CONSTANTES
// ============================================================================

void iconst_m1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, -1); }
void iconst_0_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 0); }
void iconst_1_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 1); }
void iconst_2_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 2); }
void iconst_3_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 3); }
void iconst_4_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 4); }
void iconst_5_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 5); }

void bipush_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 byte = read_u1(frame);
    // Extende sinal para int32
    operand_stack_push_int(&frame->operand_stack, (int8_t)byte);
}

// 0x11 - sipush (Short Integer Push) - ADICIONADO
void sipush_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    // Lê 2 bytes (short) com sinal
    int16_t val = read_s2(frame);
    operand_stack_push_int(&frame->operand_stack, val);
}

// ============================================================================
// LOADS (Inteiro e Referência)
// ============================================================================

// INT
void iload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, index));
}
void iload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 0)); }
void iload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 1)); }
void iload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 2)); }
void iload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 3)); }

// REFERENCE (Alocações, Arrays, Strings)
void aload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, index));
}
void aload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 0)); }
void aload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 1)); }
void aload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 2)); }
void aload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 3)); }

// ============================================================================
// STORES (Inteiro e Referência)
// ============================================================================

// INT
void istore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    local_var_set_int(&frame->local_vars, index, operand_stack_pop_int(&frame->operand_stack));
}
void istore_0_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 0, operand_stack_pop_int(&frame->operand_stack)); }
void istore_1_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 1, operand_stack_pop_int(&frame->operand_stack)); }
void istore_2_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 2, operand_stack_pop_int(&frame->operand_stack)); }
void istore_3_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 3, operand_stack_pop_int(&frame->operand_stack)); }

// REFERENCE
void astore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    void* ref = operand_stack_pop_reference(&frame->operand_stack);
    local_var_set_reference(&frame->local_vars, index, ref);
}
void astore_0_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 0, ref); }
void astore_1_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 1, ref); }
void astore_2_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 2, ref); }
void astore_3_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 3, ref); }
