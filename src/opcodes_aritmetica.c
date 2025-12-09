#include "opcodes.h"
#include "engine.h"
#include <math.h> // Necessário para isnan, fmod, INFINITY
#include <stdint.h> // Para INT32_MAX, INT64_MIN, etc

// ============================================================================
// ARITMÉTICA INT
// ============================================================================
void iadd_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, v1 + v2);
}

void isub_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, v1 - v2);
}

void imul_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, v1 * v2);
}

void iinc_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 idx = frame->code[frame->pc++];
    int8_t val = (int8_t)frame->code[frame->pc++];
    int32_t current = local_var_get_int(&frame->local_vars, idx);
    local_var_set_int(&frame->local_vars, idx, current + val);
}

// ============================================================================
// ARITMÉTICA DOUBLE
// ============================================================================

void dadd_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v2 = operand_stack_pop_double(&frame->operand_stack);
    double v1 = operand_stack_pop_double(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, v1 + v2);
}

void dsub_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v2 = operand_stack_pop_double(&frame->operand_stack);
    double v1 = operand_stack_pop_double(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, v1 - v2);
}

void dmul_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v2 = operand_stack_pop_double(&frame->operand_stack);
    double v1 = operand_stack_pop_double(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, v1 * v2);
}

void ddiv_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v2 = operand_stack_pop_double(&frame->operand_stack);
    double v1 = operand_stack_pop_double(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, v1 / v2);
}

void drem_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v2 = operand_stack_pop_double(&frame->operand_stack);
    double v1 = operand_stack_pop_double(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, fmod(v1, v2));
}

void dneg_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v = operand_stack_pop_double(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, -v);
}

// ============================================================================
// COMPARAÇÃO DOUBLE (dcmpl, dcmpg)
// ============================================================================

void dcmpl_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v2 = operand_stack_pop_double(&frame->operand_stack);
    double v1 = operand_stack_pop_double(&frame->operand_stack);
    
    if (isnan(v1) || isnan(v2)) {
        operand_stack_push_int(&frame->operand_stack, -1);
    } else if (v1 > v2) {
        operand_stack_push_int(&frame->operand_stack, 1);
    } else if (v1 == v2) {
        operand_stack_push_int(&frame->operand_stack, 0);
    } else { 
        operand_stack_push_int(&frame->operand_stack, -1);
    }
}

void dcmpg_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v2 = operand_stack_pop_double(&frame->operand_stack);
    double v1 = operand_stack_pop_double(&frame->operand_stack);
    
    if (isnan(v1) || isnan(v2)) {
        operand_stack_push_int(&frame->operand_stack, 1);
    } else if (v1 > v2) {
        operand_stack_push_int(&frame->operand_stack, 1);
    } else if (v1 == v2) {
        operand_stack_push_int(&frame->operand_stack, 0);
    } else { 
        operand_stack_push_int(&frame->operand_stack, -1);
    }
}

// ============================================================================
// CONVERSÕES (CASTING)
// ============================================================================

// Int conversions
void i2l_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_long(&frame->operand_stack, (int64_t)val);
}
void i2f_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_float(&frame->operand_stack, (float)val);
}
void i2d_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, (double)val);
}
void i2b_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, (int8_t)val); // Truncate and sign-extend
}
void i2c_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, (uint16_t)val); // Zero-extend
}
void i2s_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, (int16_t)val); // Sign-extend
}

// Long conversions
void l2i_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int64_t val = operand_stack_pop_long(&frame->operand_stack);
    operand_stack_push_int(&frame->operand_stack, (int32_t)val);
}
void l2f_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int64_t val = operand_stack_pop_long(&frame->operand_stack);
    operand_stack_push_float(&frame->operand_stack, (float)val);
}
void l2d_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int64_t val = operand_stack_pop_long(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, (double)val);
}

// Float conversions
void f2i_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    float v = operand_stack_pop_float(&frame->operand_stack);
    int32_t res;
    if (isnan(v)) res = 0;
    else if (v >= (float)INT32_MAX) res = INT32_MAX;
    else if (v <= (float)INT32_MIN) res = INT32_MIN;
    else res = (int32_t)v;
    operand_stack_push_int(&frame->operand_stack, res);
}
void f2l_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    float v = operand_stack_pop_float(&frame->operand_stack);
    int64_t res;
    if (isnan(v)) res = 0;
    else if (v >= (float)INT64_MAX) res = INT64_MAX;
    else if (v <= (float)INT64_MIN) res = INT64_MIN;
    else res = (int64_t)v;
    operand_stack_push_long(&frame->operand_stack, res);
}
void f2d_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    float v = operand_stack_pop_float(&frame->operand_stack);
    operand_stack_push_double(&frame->operand_stack, (double)v);
}

// Double conversions
void d2i_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v = operand_stack_pop_double(&frame->operand_stack);
    int32_t res;
    if (isnan(v)) res = 0;
    else if (v >= (double)INT32_MAX) res = INT32_MAX;
    else if (v <= (double)INT32_MIN) res = INT32_MIN;
    else res = (int32_t)v;
    operand_stack_push_int(&frame->operand_stack, res);
}
void d2l_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v = operand_stack_pop_double(&frame->operand_stack);
    int64_t res;
    if (isnan(v)) res = 0;
    else if (v >= (double)INT64_MAX) res = INT64_MAX;
    else if (v <= (double)INT64_MIN) res = INT64_MIN;
    else res = (int64_t)v;
    operand_stack_push_long(&frame->operand_stack, res);
}
void d2f_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    double v = operand_stack_pop_double(&frame->operand_stack);
    operand_stack_push_float(&frame->operand_stack, (float)v);
}
