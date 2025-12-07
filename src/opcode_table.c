#include "engine.h"
#include "opcodes.h"


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
    opcode_table[iinc] = iinc_op;

    // Controle 
    opcode_table[goto_]     = goto_op;
    opcode_table[ifne]      = ifne_op;
    opcode_table[ifle]      = ifle_op;
    opcode_table[if_icmpgt] = if_icmpgt_op;
    opcode_table[if_icmple] = if_icmple_op;
    opcode_table[if_icmpge] = if_icmpge_op;

    // Chamadas 
    opcode_table[invokestatic] = invokestatic_op;
    opcode_table[ireturn]      = ireturn_op;
    opcode_table[return_]      = return_op;
}

