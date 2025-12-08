#include "opcodes.h"
#include "engine.h"
#include "jvm.h"
#include "frame.h"
#include <stdio.h>

// Função auxiliar local para ler 4 bytes (int32)
static int32_t read_s4(Frame* f) {
    u1 b1 = read_u1(f);
    u1 b2 = read_u1(f);
    u1 b3 = read_u1(f);
    u1 b4 = read_u1(f);
    return (int32_t)((b1 << 24) | (b2 << 16) | (b3 << 8) | b4);
}

// ============================================================================
// INSTRUÇÕES DE DESVIO CONDICIONAL E INCONDICIONAL
// ============================================================================

void goto_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int16_t offset = read_s2(frame);
    frame->pc = (frame->pc - 3) + offset;
}

void ifne_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val != 0) frame->pc = (frame->pc - 3) + offset;
}

void ifle_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t val = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (val <= 0) frame->pc = (frame->pc - 3) + offset;
}

void if_icmpgt_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 > v2) frame->pc = (frame->pc - 3) + offset;
}

void if_icmple_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 <= v2) frame->pc = (frame->pc - 3) + offset;
}

void if_icmpge_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
    int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
    int16_t offset = read_s2(frame);
    if (v1 >= v2) frame->pc = (frame->pc - 3) + offset;
}

// ============================================================================
// SWITCH (TABLESWITCH e LOOKUPSWITCH)
// ============================================================================

// 0xAA - tableswitch
void tableswitch_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    // Opcodes de switch têm alinhamento de 4 bytes relativo ao início do método.
    // O opcode estava em (frame->pc - 1).
    u4 opcode_pc = frame->pc - 1;
    
    // Calcula padding necessário
    u1 padding = (4 - (frame->pc % 4)) % 4;
    
    // Consome os bytes de padding
    for (int i = 0; i < padding; i++) {
        read_u1(frame);
    }

    // Lê operandos
    int32_t default_offset = read_s4(frame);
    int32_t low = read_s4(frame);
    int32_t high = read_s4(frame);
    int32_t num_offsets = high - low + 1;

    // Valor do índice do switch (vem da pilha)
    int32_t index = operand_stack_pop_int(&frame->operand_stack);

    if (index >= low && index <= high) {
        // Índice válido: lê a tabela de offsets até achar o correto
        int32_t target_offset = 0;
        // Precisamos ler TODOS os offsets para avançar o PC corretamente,
        // mas só guardamos o que nos interessa.
        for (int i = 0; i < num_offsets; i++) {
            int32_t off = read_s4(frame);
            if (i == (index - low)) {
                target_offset = off;
            }
        }
        frame->pc = opcode_pc + target_offset;
    } 
    else {
        // Índice inválido: usa default.
        // Ainda precisamos pular todos os offsets da tabela para o PC ficar coerente
        // se fôssemos continuar, mas como vamos dar um salto (jump), basta calcular o destino.
        // POREM: O frame->pc tem que ser consistente se quiséssemos debugar.
        // Vamos apenas aplicar o default offset a partir do opcode_pc.
        frame->pc = opcode_pc + default_offset;
    }
}

// 0xAB - lookupswitch
void lookupswitch_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u4 opcode_pc = frame->pc - 1;
    u1 padding = (4 - (frame->pc % 4)) % 4;
    
    for (int i = 0; i < padding; i++) read_u1(frame);

    int32_t default_offset = read_s4(frame);
    int32_t npairs = read_s4(frame);

    int32_t key = operand_stack_pop_int(&frame->operand_stack);
    int32_t target_offset = default_offset; // Assume default
    
    // Procura a chave na lista de pares
    for (int i = 0; i < npairs; i++) {
        int32_t match = read_s4(frame);
        int32_t offset = read_s4(frame);
        
        if (key == match) {
            target_offset = offset;
        }
    }

    frame->pc = opcode_pc + target_offset;
}
