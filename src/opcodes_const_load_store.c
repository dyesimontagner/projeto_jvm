#include "opcodes.h"
#include "jvm.h"
#include "frame.h"
#include "engine.h" 
#include "heap.h"
#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 

// ============================================================================
// CONSTANTES (Int, Long, Float, Double)
// ============================================================================

void iconst_m1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, -1); }
void iconst_0_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 0); }
void iconst_1_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 1); }
void iconst_2_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 2); }
void iconst_3_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 3); }
void iconst_4_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 4); }
void iconst_5_op(JVM* jvm, Frame* frame)  { (void)jvm; operand_stack_push_int(&frame->operand_stack, 5); }

void lconst_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_long(&frame->operand_stack, 0); }
void lconst_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_long(&frame->operand_stack, 1); }

void fconst_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_float(&frame->operand_stack, 0.0f); }
void fconst_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_float(&frame->operand_stack, 1.0f); }
void fconst_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_float(&frame->operand_stack, 2.0f); }

void dconst_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, 0.0); }
void dconst_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, 1.0); }

void bipush_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 byte = read_u1(frame);
    operand_stack_push_int(&frame->operand_stack, (int8_t)byte);
}

void sipush_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int16_t val = read_s2(frame);
    operand_stack_push_int(&frame->operand_stack, val);
}

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
    } else if (cp->tag == CONSTANT_Long) {
        high = cp->info.long_info.high_bytes;
        low  = cp->info.long_info.low_bytes;
        int64_t val_64 = ((int64_t)high << 32) | low;
        operand_stack_push_long(&frame->operand_stack, val_64);
    }
}

// ============================================================================
// LOADS (Local Variables)
// ============================================================================
void iload_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, idx)); }
void iload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 0)); }
void iload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 1)); }
void iload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 2)); }
void iload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 3)); }

void lload_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); operand_stack_push_long(&frame->operand_stack, local_var_get_long(&frame->local_vars, idx)); }
void lload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_long(&frame->operand_stack, local_var_get_long(&frame->local_vars, 0)); }
void lload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_long(&frame->operand_stack, local_var_get_long(&frame->local_vars, 1)); }
void lload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_long(&frame->operand_stack, local_var_get_long(&frame->local_vars, 2)); }
void lload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_long(&frame->operand_stack, local_var_get_long(&frame->local_vars, 3)); }

void fload_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); operand_stack_push_float(&frame->operand_stack, local_var_get_float(&frame->local_vars, idx)); }
void fload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_float(&frame->operand_stack, local_var_get_float(&frame->local_vars, 0)); }
void fload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_float(&frame->operand_stack, local_var_get_float(&frame->local_vars, 1)); }
void fload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_float(&frame->operand_stack, local_var_get_float(&frame->local_vars, 2)); }
void fload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_float(&frame->operand_stack, local_var_get_float(&frame->local_vars, 3)); }

void dload_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, idx)); }
void dload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 0)); }
void dload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 1)); }
void dload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 2)); }
void dload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_double(&frame->operand_stack, local_var_get_double(&frame->local_vars, 3)); }

void aload_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, idx)); }
void aload_0_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 0)); }
void aload_1_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 1)); }
void aload_2_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 2)); }
void aload_3_op(JVM* jvm, Frame* frame) { (void)jvm; operand_stack_push_reference(&frame->operand_stack, local_var_get_reference(&frame->local_vars, 3)); }

// ============================================================================
// STORES (Local Variables)
// ============================================================================
void istore_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); local_var_set_int(&frame->local_vars, idx, operand_stack_pop_int(&frame->operand_stack)); }
void istore_0_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 0, operand_stack_pop_int(&frame->operand_stack)); }
void istore_1_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 1, operand_stack_pop_int(&frame->operand_stack)); }
void istore_2_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 2, operand_stack_pop_int(&frame->operand_stack)); }
void istore_3_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_int(&frame->local_vars, 3, operand_stack_pop_int(&frame->operand_stack)); }

void lstore_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); local_var_set_long(&frame->local_vars, idx, operand_stack_pop_long(&frame->operand_stack)); }
void lstore_0_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_long(&frame->local_vars, 0, operand_stack_pop_long(&frame->operand_stack)); }
void lstore_1_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_long(&frame->local_vars, 1, operand_stack_pop_long(&frame->operand_stack)); }
void lstore_2_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_long(&frame->local_vars, 2, operand_stack_pop_long(&frame->operand_stack)); }
void lstore_3_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_long(&frame->local_vars, 3, operand_stack_pop_long(&frame->operand_stack)); }

void fstore_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); local_var_set_float(&frame->local_vars, idx, operand_stack_pop_float(&frame->operand_stack)); }
void fstore_0_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_float(&frame->local_vars, 0, operand_stack_pop_float(&frame->operand_stack)); }
void fstore_1_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_float(&frame->local_vars, 1, operand_stack_pop_float(&frame->operand_stack)); }
void fstore_2_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_float(&frame->local_vars, 2, operand_stack_pop_float(&frame->operand_stack)); }
void fstore_3_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_float(&frame->local_vars, 3, operand_stack_pop_float(&frame->operand_stack)); }

void dstore_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); local_var_set_double(&frame->local_vars, idx, operand_stack_pop_double(&frame->operand_stack)); }
void dstore_0_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 0, operand_stack_pop_double(&frame->operand_stack)); }
void dstore_1_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 1, operand_stack_pop_double(&frame->operand_stack)); }
void dstore_2_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 2, operand_stack_pop_double(&frame->operand_stack)); }
void dstore_3_op(JVM* jvm, Frame* frame) { (void)jvm; local_var_set_double(&frame->local_vars, 3, operand_stack_pop_double(&frame->operand_stack)); }

void astore_op(JVM* jvm, Frame* frame) { (void)jvm; u1 idx = read_u1(frame); void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, idx, ref); }
void astore_0_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 0, ref); }
void astore_1_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 1, ref); }
void astore_2_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 2, ref); }
void astore_3_op(JVM* jvm, Frame* frame) { (void)jvm; void* ref = operand_stack_pop_reference(&frame->operand_stack); local_var_set_reference(&frame->local_vars, 3, ref); }

// ============================================================================
// STACK & MISC
// ============================================================================
void dup_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    OperandStack* stack = &frame->operand_stack;
    if (stack->top < 0 || stack->top >= stack->max_size - 1) exit(EXIT_FAILURE);
    stack->top++;
    stack->elements[stack->top] = stack->elements[stack->top - 1];
}

void dup2_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    OperandStack* stack = &frame->operand_stack;
    if (stack->top < 1 || stack->top + 2 >= stack->max_size) exit(EXIT_FAILURE);
    StackElement v1 = stack->elements[stack->top];
    StackElement v2 = stack->elements[stack->top - 1];
    stack->top++; stack->elements[stack->top] = v2;
    stack->top++; stack->elements[stack->top] = v1;
}

void pop_op(JVM* jvm, Frame* frame) { (void)jvm; if (frame->operand_stack.top >= 0) frame->operand_stack.top--; }
void swap_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    OperandStack* stack = &frame->operand_stack;
    if (stack->top >= 1) {
        StackElement temp = stack->elements[stack->top];
        stack->elements[stack->top] = stack->elements[stack->top - 1];
        stack->elements[stack->top - 1] = temp;
    }
}

// ============================================================================
// ARRAY INSTRUCTIONS (Load/Store/Length)
// ============================================================================

void arraylength_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array) { fprintf(stderr, "NullPtr (arraylength)\n"); exit(EXIT_FAILURE); }
    operand_stack_push_int(&frame->operand_stack, array->tamanho);
}

// Inteiro
void iaload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || !array->dados || index < 0 || index >= (int32_t)array->tamanho) { fprintf(stderr, "Erro iaload\n"); exit(EXIT_FAILURE); }
    operand_stack_push_int(&frame->operand_stack, ((int32_t*)array->dados)[index]);
}

void iastore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || !array->dados || index < 0 || index >= (int32_t)array->tamanho) { fprintf(stderr, "Erro iastore\n"); exit(EXIT_FAILURE); }
    ((int32_t*)array->dados)[index] = val;
}

// Referencia
void aaload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    void* ref = operand_stack_pop_reference(&frame->operand_stack);
    if (!ref) { fprintf(stderr, "NullPointerException aaload\n"); exit(EXIT_FAILURE); }
    // Hack simplificado: trata como array de ponteiros (void**)
    void** arrayref = (void**)ref; 
    operand_stack_push_reference(&frame->operand_stack, arrayref[index]);
}

void aastore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    void* val = operand_stack_pop_reference(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    void** arrayref = (void**)operand_stack_pop_reference(&frame->operand_stack);
    if (!arrayref) { fprintf(stderr, "NullPointerException aastore\n"); exit(EXIT_FAILURE); }
    arrayref[index] = val;
}

// Float
void faload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    operand_stack_push_float(&frame->operand_stack, ((float*)array->dados)[index]);
}

void fastore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    float val = operand_stack_pop_float(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    ((float*)array->dados)[index] = val;
}

// Byte/Boolean
void baload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    operand_stack_push_int(&frame->operand_stack, (int32_t)((int8_t*)array->dados)[index]);
}

void bastore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    ((int8_t*)array->dados)[index] = (int8_t)val;
}

// Char
void caload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    operand_stack_push_int(&frame->operand_stack, (int32_t)((uint16_t*)array->dados)[index]);
}

void castore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    ((uint16_t*)array->dados)[index] = (uint16_t)val;
}

// Short
void saload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    operand_stack_push_int(&frame->operand_stack, (int32_t)((int16_t*)array->dados)[index]);
}

void sastore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    ((int16_t*)array->dados)[index] = (int16_t)val;
}

// Double (NOVO)
void daload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    double val = ((double*)array->dados)[index];
    operand_stack_push_double(&frame->operand_stack, val);
}

void dastore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double val = operand_stack_pop_double(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    ((double*)array->dados)[index] = val;
}

// Long (NOVO)
void laload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    int64_t val = ((int64_t*)array->dados)[index];
    operand_stack_push_long(&frame->operand_stack, val);
}

void lastore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int64_t val = operand_stack_pop_long(&frame->operand_stack);
    int32_t index = operand_stack_pop_int(&frame->operand_stack);
    ObjetoArray* array = (ObjetoArray*) operand_stack_pop_reference(&frame->operand_stack);
    if (!array || index < 0 || index >= (int32_t)array->tamanho) exit(EXIT_FAILURE);
    ((int64_t*)array->dados)[index] = val;
}
