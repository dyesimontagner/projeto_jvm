#include "opcodes.h"
#include "jvm.h"
#include "frame.h"
#include "engine.h" // Para read_u1, read_s2
#include <stdio.h>
#include <string.h> // Necessário para memcpy

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

// CONSTANTES DOUBLE
void dconst_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, 0.0); }
void dconst_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, 1.0); }

void bipush_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 byte = read_u1(frame);
    // Extende sinal para int32
    operand_stack_push_int(&frame->operand_stack, (int8_t)byte);
}

// 0x11 - sipush (Short Integer Push)
void sipush_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    // Lê 2 bytes (short) com sinal
    int16_t val = read_s2(frame);
    operand_stack_push_int(&frame->operand_stack, val);
}

// ldc2_w (Long e Double)
void ldc2_w_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index_byte1 = read_u1(frame);
    u1 index_byte2 = read_u1(frame);
    u2 index = (index_byte1 << 8) | index_byte2;

    cp_info* cp = frame->class_file->constant_pool[index];
    
    u4 high, low;

    if (cp->tag == CONSTANT_Double) {
        high = cp->info.double_info.high_bytes;
        low  = cp->info.double_info.low_bytes;

        int64_t val_64 = ((int64_t)high << 32) | low;
        double d_val;
        memcpy(&d_val, &val_64, sizeof(double));
        operand_stack_push_double(&frame->operand_stack, d_val);
    } 
    else if (cp->tag == CONSTANT_Long) {
        high = cp->info.long_info.high_bytes;
        low  = cp->info.long_info.low_bytes;

        int64_t val_64 = ((int64_t)high << 32) | low;
        operand_stack_push_long(&frame->operand_stack, val_64);
    }
    else {
        printf("Erro: ldc2_w invocado com indice apontando para tag invalida: %d\n", cp->tag);
    }
}

// ============================================================================
// LOADS (Inteiro, Referência e Double)
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

// REFERENCE
void aload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, index));
}
void aload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 0)); }
void aload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 1)); }
void aload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 2)); }
void aload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 3)); }

// DOUBLE LOADS
void dload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, index));
}
void dload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 0)); }
void dload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 1)); }
void dload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 2)); }
void dload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 3)); }

// ============================================================================
// STORES (Inteiro, Referência e Double)
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

// DOUBLE STORES
void dstore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    local_var_set_double(&frame->local_vars, index, operand_stack_pop_double(&frame->operand_stack));
}
void dstore_0_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 0, operand_stack_pop_double(&frame->operand_stack)); }
void dstore_1_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 1, operand_stack_pop_double(&frame->operand_stack)); }
void dstore_2_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 2, operand_stack_pop_double(&frame->operand_stack)); }
void dstore_3_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 3, operand_stack_pop_double(&frame->operand_stack)); }
