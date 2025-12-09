#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "leitor_class.h"
#include "jvm.h"
#include "engine.h"
#include "frame.h"
#include "opcodes.h"

// Removido 'static' para permitir uso em outros arquivos (ex: opcodes_controle.c)
u1 read_u1(Frame* f) { return f->code[f->pc++]; }

// Removido 'static'
int16_t read_s2(Frame* f) {
    u1 b1 = read_u1(f);
    u1 b2 = read_u1(f);
    return (int16_t)((b1 << 8) | b2);
}

void unimplemented(JVM* jvm, Frame* frame) {
    (void)jvm; // Silencia o warning
    fprintf(stderr, "Erro: Opcode nao implementado ou invalido (PC: %d)\n", frame->pc);
    exit(EXIT_FAILURE);
}

extern OpcodeHandler opcode_table[256];

void execute_bytecode(JVM* jvm, Frame* frame) {
    if (!frame || !frame->code) return;

    while (jvm->running && frame == jvm->frame_stack.top && frame->pc < frame->code_length) {
        u1 opcode = frame->code[frame->pc];
        frame->pc++;
        opcode_table[opcode](jvm, frame);
    }
}
