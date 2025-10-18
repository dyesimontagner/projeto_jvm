#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include "common.h"
#include <stdio.h>

// Enum com os valores das tags para cada tipo de constante
typedef enum {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Long = 5,
    CONSTANT_Double = 6,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_InterfaceMethodref = 11,
    CONSTANT_NameAndType = 12
} CONSTANT_Type;

// Estrutura para uma constante do tipo Utf8
typedef struct { u2 length; u1* bytes; } CONSTANT_Utf8_info;
// Novas estruturas para cada tipo de constante
typedef struct { u2 name_index; } CONSTANT_Class_info;
typedef struct { u2 string_index; } CONSTANT_String_info;
typedef struct { u2 class_index; u2 name_and_type_index; } CONSTANT_Ref_info;
typedef struct { u2 name_index; u2 descriptor_index; } CONSTANT_NameAndType_info;
typedef struct { u4 bytes; } CONSTANT_Integer_info;
typedef struct { u4 bytes; } CONSTANT_Float_info;
typedef struct { u4 high_bytes; u4 low_bytes; } CONSTANT_Long_info;
typedef struct { u4 high_bytes; u4 low_bytes; } CONSTANT_Double_info;

// Estrutura genérica para uma entrada do Constant Pool (com a union atualizada e corrigida)
typedef struct {
    u1 tag;
    union {
        // CORREÇÃO: Dando um nome de variável para cada tipo na union
        CONSTANT_Utf8_info           utf8_info;
        CONSTANT_Class_info          class_info;
        CONSTANT_String_info         string_info;
        CONSTANT_Ref_info            fieldref_info;
        CONSTANT_Ref_info            methodref_info;
        CONSTANT_Ref_info            interface_methodref_info;
        CONSTANT_NameAndType_info    name_and_type_info;
        CONSTANT_Integer_info        integer_info;
        CONSTANT_Float_info          float_info;
        CONSTANT_Long_info           long_info;
        CONSTANT_Double_info         double_info;
    } info;
} cp_info;

// Declarações das funções
void read_constant_pool(u2 count, cp_info** constant_pool, FILE* file);
void print_constant_pool(u2 count, cp_info** constant_pool);
void free_constant_pool(u2 count, cp_info** constant_pool);

// NOVA DECLARAÇÃO para que outros arquivos possam usar esta função
char* get_utf8_from_pool(u2 index, cp_info** constant_pool);

#endif // CONSTANT_POOL_H