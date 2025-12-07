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
    CONSTANT_NameAndType = 12,
    CONSTANT_MethodHandle = 15,
    CONSTANT_MethodType = 16,
    CONSTANT_InvokeDynamic = 18
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
typedef struct { u2 bootstrap_method_attr_index; u2 name_and_type_index; } CONSTANT_InvokeDynamic_info;
typedef struct { u1 reference_kind; u2 reference_index; } CONSTANT_MethodHandle_info;
typedef struct { u2 descriptor_index; } CONSTANT_MethodType_info;
// Estrutura genérica para uma entrada do Constant Pool
typedef struct {
    u1 tag;
    union {
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
        CONSTANT_InvokeDynamic_info  invoke_dynamic_info;
        CONSTANT_MethodHandle_info   method_handle_info;
        CONSTANT_MethodType_info     method_type_info;
    } info;
} cp_info;

// Declarações das funções de constant_pool.c
void read_constant_pool(u2 count, cp_info** constant_pool, FILE* file);
void print_constant_pool(u2 count, cp_info** constant_pool); // Declarada aqui
void free_constant_pool(u2 count, cp_info** constant_pool);
char* get_utf8_from_pool(u2 index, cp_info** constant_pool, u2 cp_count);

#endif // CONSTANT_POOL_H