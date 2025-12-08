#include <stdio.h>
#include "engine.h"
#include "opcodes.h"

// Handlers implementados em outros arquivos
void iconst_m1_op(JVM* jvm, Frame* frame);
void iconst_0_op(JVM* jvm, Frame* frame);
void iconst_1_op(JVM* jvm, Frame* frame);
void iconst_2_op(JVM* jvm, Frame* frame);
void iconst_3_op(JVM* jvm, Frame* frame);
void iconst_4_op(JVM* jvm, Frame* frame);
void iconst_5_op(JVM* jvm, Frame* frame);
void bipush_op(JVM* jvm, Frame* frame);

void iload_op(JVM* jvm, Frame* frame);
void iload_0_op(JVM* jvm, Frame* frame);
void iload_1_op(JVM* jvm, Frame* frame);
void iload_2_op(JVM* jvm, Frame* frame);
void iload_3_op(JVM* jvm, Frame* frame);

void istore_op(JVM* jvm, Frame* frame);
void istore_0_op(JVM* jvm, Frame* frame);
void istore_1_op(JVM* jvm, Frame* frame);
void istore_2_op(JVM* jvm, Frame* frame);
void istore_3_op(JVM* jvm, Frame* frame);

void iadd_op(JVM* jvm, Frame* frame);
void isub_op(JVM* jvm, Frame* frame);
void imul_op(JVM* jvm, Frame* frame);
void idiv_op(JVM* jvm, Frame* frame);
void iinc_op(JVM* jvm, Frame* frame);

void goto_op(JVM* jvm, Frame* frame);
void ifeq_op(JVM* jvm, Frame* frame);
void ifne_op(JVM* jvm, Frame* frame);
void ifle_op(JVM* jvm, Frame* frame);
void if_icmpeq_op(JVM* jvm, Frame* frame);
void if_icmpne_op(JVM* jvm, Frame* frame);
void if_icmpgt_op(JVM* jvm, Frame* frame);
void if_icmple_op(JVM* jvm, Frame* frame);
void if_icmpge_op(JVM* jvm, Frame* frame);

void aload_op(JVM* jvm, Frame* frame);
void aload_0_op(JVM* jvm, Frame* frame);
void astore_op(JVM* jvm, Frame* frame);
void astore_0_op(JVM* jvm, Frame* frame);
void astore_1_op(JVM* jvm, Frame* frame);
void astore_2_op(JVM* jvm, Frame* frame);
void astore_3_op(JVM* jvm, Frame* frame);
void dup_op(JVM* jvm, Frame* frame);

void ldc_op(JVM* jvm, Frame* frame);
void ldc_w_op(JVM* jvm, Frame* frame);

void new_op(JVM* jvm, Frame* frame);
void getfield_op(JVM* jvm, Frame* frame);
void putfield_op(JVM* jvm, Frame* frame);
void invokespecial_op(JVM* jvm, Frame* frame);

void invokestatic_op(JVM* jvm, Frame* frame);
void ireturn_op(JVM* jvm, Frame* frame);
void return_op(JVM* jvm, Frame* frame);

// Handlers básicos usados como default
static void unimplemented(JVM* jvm, Frame* frame);
static void nop_op(JVM* jvm, Frame* frame);

OpcodeHandler opcode_table[256];

void engine_init(void) {
    for (int i = 0; i < 256; i++) opcode_table[i] = unimplemented;

    // Constantes & Loads/Stores
    opcode_table[op_nop]    = nop_op;
    opcode_table[iconst_m1] = iconst_m1_op;
    opcode_table[iconst_0]  = iconst_0_op;
    opcode_table[iconst_1]  = iconst_1_op;
    opcode_table[iconst_2]  = iconst_2_op;
    opcode_table[iconst_3]  = iconst_3_op;
    opcode_table[iconst_4]  = iconst_4_op;
    opcode_table[iconst_5]  = iconst_5_op;
    opcode_table[bipush]    = bipush_op;

    opcode_table[iload]   = iload_op;
    opcode_table[iload_0] = iload_0_op;
    opcode_table[iload_1] = iload_1_op;
    opcode_table[iload_2] = iload_2_op;
    opcode_table[iload_3] = iload_3_op;

    opcode_table[istore]   = istore_op;
    opcode_table[istore_0] = istore_0_op;
    opcode_table[istore_1] = istore_1_op;
    opcode_table[istore_2] = istore_2_op;
    opcode_table[istore_3] = istore_3_op;

    // Aritmética
    opcode_table[iadd] = iadd_op;
    opcode_table[isub] = isub_op;
    opcode_table[imul] = imul_op;
    opcode_table[idiv_] = idiv_op;
    opcode_table[iinc] = iinc_op;

    // Controle 
    opcode_table[goto_]     = goto_op;
    opcode_table[ifeq]      = ifeq_op;
    opcode_table[ifne]      = ifne_op;
    opcode_table[ifle]      = ifle_op;
    opcode_table[if_icmpeq] = if_icmpeq_op;
    opcode_table[if_icmpne] = if_icmpne_op;
    opcode_table[if_icmpgt] = if_icmpgt_op;
    opcode_table[if_icmple] = if_icmple_op;
    opcode_table[if_icmpge] = if_icmpge_op;

    // Referências
    opcode_table[aload]   = aload_op;
    opcode_table[aload_0] = aload_0_op;
    opcode_table[astore]  = astore_op;
    opcode_table[astore_0] = astore_0_op;
    opcode_table[astore_1] = astore_1_op;
    opcode_table[astore_2] = astore_2_op;
    opcode_table[astore_3] = astore_3_op;
    opcode_table[dup]     = dup_op;
    
    // Constantes
    opcode_table[ldc]   = ldc_op;
    opcode_table[ldc_w] = ldc_w_op;

    // Objetos
    opcode_table[new_] = new_op;
    opcode_table[getfield] = getfield_op;
    opcode_table[putfield] = putfield_op;
    opcode_table[invokespecial] = invokespecial_op;

    // Chamadas 
    opcode_table[invokestatic] = invokestatic_op;
    opcode_table[ireturn]      = ireturn_op;
    opcode_table[return_]      = return_op;
}

static void nop_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    (void)frame;
}

static void unimplemented(JVM* jvm, Frame* frame) {
    u1 opcode = 0;
    if (frame && frame->pc > 0) opcode = frame->code[frame->pc - 1];
    fprintf(stderr, "Opcode 0x%02x não implementado. Execução interrompida.\n", opcode);
    if (jvm) {
        jvm->exit_code = 1;
        jvm->running = false;
    }
}

