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

    // Stack Management
    opcode_table[dup] = dup_op;
    opcode_table[dup2] = dup2_op;
    opcode_table[pop] = pop_op;
    opcode_table[swap] = swap_op;

    // Constantes
    opcode_table[iconst_m1] = iconst_m1_op;
    opcode_table[iconst_0]  = iconst_0_op;
    opcode_table[iconst_1]  = iconst_1_op;
    opcode_table[iconst_2]  = iconst_2_op;
    opcode_table[iconst_3]  = iconst_3_op;
    opcode_table[iconst_4]  = iconst_4_op;
    opcode_table[iconst_5]  = iconst_5_op;
    
    opcode_table[lconst_0]  = lconst_0_op;
    opcode_table[lconst_1]  = lconst_1_op;
    
    opcode_table[fconst_0]  = fconst_0_op;
    opcode_table[fconst_1]  = fconst_1_op;
    opcode_table[fconst_2]  = fconst_2_op;
    
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
    
    opcode_table[lload]   = lload_op;
    opcode_table[lload_0] = lload_0_op;
    opcode_table[lload_1] = lload_1_op;
    opcode_table[lload_2] = lload_2_op;
    opcode_table[lload_3] = lload_3_op;
    
    opcode_table[fload]   = fload_op;
    opcode_table[fload_0] = fload_0_op;
    opcode_table[fload_1] = fload_1_op;
    opcode_table[fload_2] = fload_2_op;
    opcode_table[fload_3] = fload_3_op;
    
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

    // Array Loads
    opcode_table[iaload] = iaload_op;
    opcode_table[laload] = laload_op; // REGISTRADO
    opcode_table[faload] = faload_op;
    opcode_table[daload] = daload_op; // REGISTRADO
    opcode_table[aaload] = aaload_op;
    opcode_table[baload] = baload_op;
    opcode_table[caload] = caload_op;
    opcode_table[saload] = saload_op;

    // Stores
    opcode_table[istore]   = istore_op;
    opcode_table[istore_0] = istore_0_op;
    opcode_table[istore_1] = istore_1_op;
    opcode_table[istore_2] = istore_2_op;
    opcode_table[istore_3] = istore_3_op;
    
    opcode_table[lstore]   = lstore_op;
    opcode_table[lstore_0] = lstore_0_op;
    opcode_table[lstore_1] = lstore_1_op;
    opcode_table[lstore_2] = lstore_2_op;
    opcode_table[lstore_3] = lstore_3_op;
    
    opcode_table[fstore]   = fstore_op;
    opcode_table[fstore_0] = fstore_0_op;
    opcode_table[fstore_1] = fstore_1_op;
    opcode_table[fstore_2] = fstore_2_op;
    opcode_table[fstore_3] = fstore_3_op;

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

    // Array Stores
    opcode_table[iastore] = iastore_op;
    opcode_table[lastore] = lastore_op; // REGISTRADO
    opcode_table[fastore] = fastore_op;
    opcode_table[dastore] = dastore_op; // REGISTRADO
    opcode_table[aastore] = aastore_op;
    opcode_table[bastore] = bastore_op;
    opcode_table[castore] = castore_op;
    opcode_table[sastore] = sastore_op;

    // Arrays Creation & Misc
    opcode_table[multianewarray] = multianewarray_op;
    opcode_table[newarray]       = newarray_op;
    opcode_table[anewarray]      = anewarray_op;
    opcode_table[arraylength]    = arraylength_op;

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

    // Conversões e Outros
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

    opcode_table[dcmpl] = dcmpl_op;
    opcode_table[dcmpg] = dcmpg_op;

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
    
    opcode_table[tableswitch]  = tableswitch_op;
    opcode_table[lookupswitch] = lookupswitch_op;

    opcode_table[getstatic]     = getstatic_op;
    opcode_table[invokevirtual] = invokevirtual_op;
    opcode_table[invokestatic]  = invokestatic_op;
    opcode_table[invokespecial] = invokespecial_op;

    opcode_table[return_] = return_op;
    opcode_table[ireturn] = ireturn_op;
}
