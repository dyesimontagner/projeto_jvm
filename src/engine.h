#ifndef ENGINE_H
#define ENGINE_H

#include "jvm.h" 
#include <stdint.h>

typedef void (*OpcodeHandler)(JVM* jvm, Frame* frame);

extern OpcodeHandler opcode_table[256];

// Leitura de operandos auxiliares
int16_t read_s2(Frame* f);

// Inicializa a tabela de opcodes 
void engine_init(void);
// Executa o loop principal de bytecodes 
void execute_bytecode(JVM* jvm, Frame* frame);

#endif 
