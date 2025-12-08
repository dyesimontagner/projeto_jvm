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
    
    opcode_table[dconst_0]  = dconst_0_op;
    opcode_table[dconst_1]  = dconst_1_op;

    opcode_table[bipush]    = bipush_op;
    opcode_table[sipush]    = sipush_op;
    opcode_table[ldc]       = ldc_op;
    opcode_table[ldc2_w]    = ldc2_w_op;

    // Loads
    opcode_table[iload]   = iload_op;
    opcode_table[iload_0] = iload_0_op;
    opcode_table[iload_1] = iload_1_op;
    opcode_table[iload_2] = iload_2_op;
    opcode_table[iload_3] = iload_3_op;
    
    opcode_table[dload]   = dload_op;
    opcode_table[dload_0] = dload_0_op;
    opcode_table[dload_1] = dload_1_op;
    opcode_table[dload_2] = dload_2_op;
    opcode_table[dload_3] = dload_3_op;

    opcode_table[aload]   = aload_op;
    opcode_table[aload_0] = aload_0_op;
    opcode_table[aload_1] = aload_1_op;
    opcode_table[aload_2] = aload_2_op;
    opcode_table[aload_3] = aload_3_op;

    opcode_table[iaload] = iaload_op;
    opcode_table[faload] = faload_op;
    opcode_table[aaload] = aaload_op;

    // Stores
    opcode_table[istore]   = istore_op;
    opcode_table[istore_0] = istore_0_op;
    opcode_table[istore_1] = istore_1_op;
    opcode_table[istore_2] = istore_2_op;
    opcode_table[istore_3] = istore_3_op;

    opcode_table[dstore]   = dstore_op;
    opcode_table[dstore_0] = dstore_0_op;
    opcode_table[dstore_1] = dstore_1_op;
    opcode_table[dstore_2] = dstore_2_op;
    opcode_table[dstore_3] = dstore_3_op;

    opcode_table[astore]   = astore_op;
    opcode_table[astore_0] = astore_0_op;
    opcode_table[astore_1] = astore_1_op;
    opcode_table[astore_2] = astore_2_op;
    opcode_table[astore_3] = astore_3_op;

    opcode_table[iastore] = iastore_op;
    opcode_table[fastore] = fastore_op;
    opcode_table[aastore] = aastore_op;

    // Arrays Creation
    opcode_table[multianewarray] = multianewarray_op;
    opcode_table[newarray]       = newarray_op;
    opcode_table[anewarray]      = anewarray_op;

    // Aritmética
    opcode_table[iadd] = iadd_op;
    opcode_table[isub] = isub_op;
    opcode_table[imul] = imul_op;
    opcode_table[iinc] = iinc_op;

    opcode_table[dadd] = dadd_op;
    opcode_table[dsub] = dsub_op;
    opcode_table[dmul] = dmul_op;
    opcode_table[ddiv_] = ddiv_op;
    opcode_table[drem_] = drem_op;
    opcode_table[dneg] = dneg_op;

    // Conversões
    opcode_table[i2l] = i2l_op;
    opcode_table[i2f] = i2f_op;
    opcode_table[i2d] = i2d_op;
    opcode_table[l2i] = l2i_op;
    opcode_table[l2f] = l2f_op;
    opcode_table[l2d] = l2d_op;
    opcode_table[f2i] = f2i_op;
    opcode_table[f2l] = f2l_op;
    opcode_table[f2d] = f2d_op;
    opcode_table[d2i] = d2i_op;
    opcode_table[d2l] = d2l_op;
    opcode_table[d2f] = d2f_op;
    opcode_table[i2b] = i2b_op;
    opcode_table[i2c] = i2c_op;
    opcode_table[i2s] = i2s_op;

    // Comparação
    opcode_table[dcmpl] = dcmpl_op;
    opcode_table[dcmpg] = dcmpg_op;

    // Controle e Desvios (CORRIGIDO: Lista Completa)
    opcode_table[goto_]     = goto_op;
    opcode_table[ifeq]      = ifeq_op;
    opcode_table[ifne]      = ifne_op;
    opcode_table[iflt]      = iflt_op;
    opcode_table[ifge]      = ifge_op;
    opcode_table[ifgt]      = ifgt_op;
    opcode_table[ifle]      = ifle_op;
    
    opcode_table[if_icmpeq] = if_icmpeq_op;
    opcode_table[if_icmpne] = if_icmpne_op;
    opcode_table[if_icmplt] = if_icmplt_op;
    opcode_table[if_icmpge] = if_icmpge_op;
    opcode_table[if_icmpgt] = if_icmpgt_op;
    opcode_table[if_icmple] = if_icmple_op;
    
    opcode_table[if_acmpeq] = if_acmpeq_op;
    opcode_table[if_acmpne] = if_acmpne_op;
    
    opcode_table[ifnull]    = ifnull_op;
    opcode_table[ifnonnull] = ifnonnull_op;
    
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
