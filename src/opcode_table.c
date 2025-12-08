#include "engine.h"
#include "opcodes.h"

OpcodeHandler opcode_table[256];

void engine_init(void) {
    // 1. Limpa tabela
    for (int i = 0; i < 256; i++) {
        opcode_table[i] = unimplemented;
    }

    // 2. Registra Opcodes

    // Gerais
    opcode_table[op_nop] = nop_op;

    // Constantes
    opcode_table[iconst_m1] = iconst_m1_op;
    opcode_table[iconst_0]  = iconst_0_op;
    opcode_table[iconst_1]  = iconst_1_op;
    opcode_table[iconst_2]  = iconst_2_op;
    opcode_table[iconst_3]  = iconst_3_op;
    opcode_table[iconst_4]  = iconst_4_op;
    opcode_table[iconst_5]  = iconst_5_op;
    opcode_table[bipush]    = bipush_op;
    opcode_table[sipush]    = sipush_op; // <--- ADICIONADO
    opcode_table[ldc]       = ldc_op;

    // Loads
    opcode_table[iload]   = iload_op;
    opcode_table[iload_0] = iload_0_op;
    opcode_table[iload_1] = iload_1_op;
    opcode_table[iload_2] = iload_2_op;
    opcode_table[iload_3] = iload_3_op;
    
    opcode_table[aload]   = aload_op;
    opcode_table[aload_0] = aload_0_op;
    opcode_table[aload_1] = aload_1_op;
    opcode_table[aload_2] = aload_2_op;
    opcode_table[aload_3] = aload_3_op;

    // Stores
    opcode_table[istore]   = istore_op;
    opcode_table[istore_0] = istore_0_op;
    opcode_table[istore_1] = istore_1_op;
    opcode_table[istore_2] = istore_2_op;
    opcode_table[istore_3] = istore_3_op;

    opcode_table[astore]   = astore_op;
    opcode_table[astore_0] = astore_0_op;
    opcode_table[astore_1] = astore_1_op;
    opcode_table[astore_2] = astore_2_op;
    opcode_table[astore_3] = astore_3_op;

    // Arrays
    opcode_table[multianewarray] = multianewarray_op;
    opcode_table[aaload]         = aaload_op;
    opcode_table[aastore]        = aastore_op;

    // Aritmética
    opcode_table[iadd] = iadd_op;
    opcode_table[isub] = isub_op;
    opcode_table[imul] = imul_op;
    opcode_table[iinc] = iinc_op;

    // Controle e Desvios
    opcode_table[goto_]     = goto_op;
    opcode_table[ifne]      = ifne_op;
    opcode_table[ifle]      = ifle_op;
    opcode_table[if_icmpgt] = if_icmpgt_op;
    opcode_table[if_icmple] = if_icmple_op;
    opcode_table[if_icmpge] = if_icmpge_op;
    
    // Switch
    opcode_table[tableswitch]  = tableswitch_op;
    opcode_table[lookupswitch] = lookupswitch_op;

    // Métodos e Campos
    opcode_table[getstatic]     = getstatic_op;
    opcode_table[invokevirtual] = invokevirtual_op;
    opcode_table[invokestatic]  = invokestatic_op;
    opcode_table[invokespecial] = invokespecial_op;

    // Retorno
    opcode_table[return_] = return_op;
    opcode_table[ireturn] = ireturn_op;
}
