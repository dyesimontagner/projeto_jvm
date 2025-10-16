#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// Definindo os tipos u1, u2 e u4 para ficar igual à especificação da JVM
typedef uint8_t  u1;
typedef uint16_t u2;
typedef uint32_t u4;

// Declarações (protótipos) das funções de swap de bytes.
// A implementação ficará no arquivo .c
u2 swap_u2(u2 val);
u4 swap_u4(u4 val);

#endif // COMMON_H