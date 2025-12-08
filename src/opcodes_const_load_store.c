#include "opcodes.h"
#include "engine.h"

// Constantes
void iconst_m1_op(JVM* jvm, Frame* frame) { printf("[iconst_m1] Push -1\n"); operand_stack_push_int(&frame->operand_stack, -1); }
void iconst_0_op(JVM* jvm, Frame* frame)  { printf("[iconst_0] Push 0\n"); operand_stack_push_int(&frame->operand_stack, 0); }
void iconst_1_op(JVM* jvm, Frame* frame)  { printf("[iconst_1] Push 1\n"); operand_stack_push_int(&frame->operand_stack, 1); }
void iconst_2_op(JVM* jvm, Frame* frame)  { printf("[iconst_2] Push 2\n"); operand_stack_push_int(&frame->operand_stack, 2); }
void iconst_3_op(JVM* jvm, Frame* frame)  { printf("[iconst_3] Push 3\n"); operand_stack_push_int(&frame->operand_stack, 3); }
void iconst_4_op(JVM* jvm, Frame* frame)  { printf("[iconst_4] Push 4\n"); operand_stack_push_int(&frame->operand_stack, 4); }
void iconst_5_op(JVM* jvm, Frame* frame)  { printf("[iconst_5] Push 5\n"); operand_stack_push_int(&frame->operand_stack, 5); }
void bipush_op(JVM* jvm, Frame* frame) {
    int8_t val = (int8_t)frame->code[frame->pc++];
    printf("[bipush] Push %d\n", val);
    operand_stack_push_int(&frame->operand_stack, (int32_t)val);
}

// Loads
void iload_op(JVM* jvm, Frame* frame) {
    u1 idx = frame->code[frame->pc++];
    int32_t val = local_var_get_int(&frame->local_vars, idx);
    printf("[iload] Load var[%d] = %d\n", idx, val);
    operand_stack_push_int(&frame->operand_stack, val);
}
void iload_0_op(JVM* jvm, Frame* frame) { int32_t val = local_var_get_int(&frame->local_vars, 0); printf("[iload_0] Load var[0] = %d\n", val); operand_stack_push_int(&frame->operand_stack, val); }
void iload_1_op(JVM* jvm, Frame* frame) { int32_t val = local_var_get_int(&frame->local_vars, 1); printf("[iload_1] Load var[1] = %d\n", val); operand_stack_push_int(&frame->operand_stack, val); }
void iload_2_op(JVM* jvm, Frame* frame) { int32_t val = local_var_get_int(&frame->local_vars, 2); printf("[iload_2] Load var[2] = %d\n", val); operand_stack_push_int(&frame->operand_stack, val); }
void iload_3_op(JVM* jvm, Frame* frame) { int32_t val = local_var_get_int(&frame->local_vars, 3); printf("[iload_3] Load var[3] = %d\n", val); operand_stack_push_int(&frame->operand_stack, val); }

// Stores
void istore_op(JVM* jvm, Frame* frame) {
    u1 idx = frame->code[frame->pc++];
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    printf("[istore] Store %d -> var[%d]\n", val, idx);
    local_var_set_int(&frame->local_vars, idx, val);
}
void istore_0_op(JVM* jvm, Frame* frame) { int32_t val = operand_stack_pop_int(&frame->operand_stack); printf("[istore_0] Store %d -> var[0]\n", val); local_var_set_int(&frame->local_vars, 0, val); }
void istore_1_op(JVM* jvm, Frame* frame) { int32_t val = operand_stack_pop_int(&frame->operand_stack); printf("[istore_1] Store %d -> var[1]\n", val); local_var_set_int(&frame->local_vars, 1, val); }
void istore_2_op(JVM* jvm, Frame* frame) { int32_t val = operand_stack_pop_int(&frame->operand_stack); printf("[istore_2] Store %d -> var[2]\n", val); local_var_set_int(&frame->local_vars, 2, val); }
void istore_3_op(JVM* jvm, Frame* frame) { int32_t val = operand_stack_pop_int(&frame->operand_stack); printf("[istore_3] Store %d -> var[3]\n", val); local_var_set_int(&frame->local_vars, 3, val); }

// Referências (aload/astore - simplified, apenas gerenciam referências genéricas)
void aload_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 idx = frame->code[frame->pc++];
    void* ref = local_var_get_reference(&frame->local_vars, idx);
    printf("[aload] Load reference from var[%d]\n", idx);
    operand_stack_push_reference(&frame->operand_stack, ref);
}

void aload_0_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    void* ref = local_var_get_reference(&frame->local_vars, 0);
    printf("[aload_0] Load reference from var[0]\n");
    operand_stack_push_reference(&frame->operand_stack, ref);
}

void astore_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 idx = frame->code[frame->pc++];
    void* ref = operand_stack_pop_reference(&frame->operand_stack);
    printf("[astore] Store reference -> var[%d]\n", idx);
    local_var_set_reference(&frame->local_vars, idx, ref);
}

void astore_0_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    void* ref = operand_stack_pop_reference(&frame->operand_stack);
    printf("[astore_0] Store reference -> var[0]\n");
    local_var_set_reference(&frame->local_vars, 0, ref);
}

void astore_1_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    void* ref = operand_stack_pop_reference(&frame->operand_stack);
    printf("[astore_1] Store reference -> var[1]\n");
    local_var_set_reference(&frame->local_vars, 1, ref);
}

void astore_2_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    void* ref = operand_stack_pop_reference(&frame->operand_stack);
    printf("[astore_2] Store reference -> var[2]\n");
    local_var_set_reference(&frame->local_vars, 2, ref);
}

void astore_3_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    void* ref = operand_stack_pop_reference(&frame->operand_stack);
    printf("[astore_3] Store reference -> var[3]\n");
    local_var_set_reference(&frame->local_vars, 3, ref);
}

// Stack manipulation
void dup_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    // dup: duplicate top value and push it onto stack
    // Assumes top value is a reference (for object allocation pattern)
    void* top = operand_stack_pop_reference(&frame->operand_stack);
    printf("[dup] Duplicate stack top\n");
    operand_stack_push_reference(&frame->operand_stack, top);
    operand_stack_push_reference(&frame->operand_stack, top);
}
