#ifndef ENGINE_H
#define ENGINE_H

#include "jvm.h"
#include <stdint.h>

// Definição do tipo de função para os handlers de opcode
typedef void (*OpcodeHandler)(JVM* jvm, Frame* frame);

// Tabela global de opcodes
extern OpcodeHandler opcode_table[256];

// Inicializa a tabela de opcodes
void engine_init(void);

// Executa o loop principal de bytecodes
void execute_bytecode(JVM* jvm, Frame* frame);

// Funções auxiliares de leitura de bytecode (agora globais)
u1 read_u1(Frame* f);
int16_t read_s2(Frame* f);

// Handler padrão para opcodes não implementados
void unimplemented(JVM* jvm, Frame* frame);

#endif
