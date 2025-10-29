#ifndef LEITOR_CLASS_H
#define LEITOR_CLASS_H

#include "common.h"
#include "constant_pool.h"
#include <stdio.h>
#include <stddef.h> // Para offsetof

// --- DEFINIÇÕES DE FLAGS E ENUM ---
#define ACC_PUBLIC       0x0001
#define ACC_PRIVATE      0x0002
#define ACC_PROTECTED    0x0004
#define ACC_STATIC       0x0008
#define ACC_FINAL        0x0010
#define ACC_SUPER        0x0020
#define ACC_SYNCHRONIZED 0x0020
#define ACC_VOLATILE     0x0040
#define ACC_BRIDGE       0x0040
#define ACC_TRANSIENT    0x0080
#define ACC_VARARGS      0x0080
#define ACC_NATIVE       0x0100
#define ACC_INTERFACE    0x0200
#define ACC_ABSTRACT     0x0400
#define ACC_STRICT       0x0800
#define ACC_SYNTHETIC    0x1000
#define ACC_ANNOTATION   0x2000
#define ACC_ENUM         0x4000

typedef enum { TYPE_CLASS, TYPE_FIELD, TYPE_METHOD } FlagType;
// --- FIM DAS DEFINIÇÕES ---

// --- ESTRUTURAS DE ATRIBUTOS ---
typedef struct { u2 start_pc; u2 end_pc; u2 handler_pc; u2 catch_type; } exception_table_entry;
typedef struct { u2 start_pc; u2 line_number; } line_number_table_entry;
typedef struct { u2 line_number_table_length; line_number_table_entry* table; } LineNumberTable_attribute;
typedef struct { u2 sourcefile_index; } SourceFile_attribute;

struct attribute_info; // Forward declaration
typedef struct {
    u2 max_stack; u2 max_locals; u4 code_length; u1* code;
    u2 exception_table_length; exception_table_entry* exception_table;
    u2 attributes_count; struct attribute_info* attributes;
} Code_attribute;

typedef struct attribute_info {
    u2 attribute_name_index; u4 attribute_length;
    union {
        u1* info; Code_attribute* code_info; SourceFile_attribute* source_file_info;
        LineNumberTable_attribute* line_number_table_info;
    } attr_info;
} attribute_info;
// --- FIM DAS ESTRUTURAS DE ATRIBUTOS ---

// --- ESTRUTURAS PRINCIPAIS ---
typedef struct {
    u2 access_flags; u2 name_index; u2 descriptor_index;
    u2 attributes_count; attribute_info* attributes;
} field_info;

typedef struct {
    u2 access_flags; u2 name_index; u2 descriptor_index;
    u2 attributes_count; attribute_info* attributes;
} method_info;

typedef struct ClassFile {
    u4 magic; u2 minor_version; u2 major_version; u2 constant_pool_count; cp_info** constant_pool;
    u2 access_flags; u2 this_class; u2 super_class; u2 interfaces_count; u2* interfaces;
    u2 fields_count; field_info* fields; u2 methods_count; method_info* methods;
    u2 attributes_count; attribute_info* attributes;
} ClassFile;
// --- FIM DAS ESTRUTURAS PRINCIPAIS ---

// --- DECLARAÇÕES DE FUNÇÕES DE LEITURA E LIBERAÇÃO ---
ClassFile* read_class_file(const char* filename);
void read_attributes(u2 count, attribute_info** attributes, FILE* file, cp_info** cp, u2 cp_count);
Code_attribute* read_code_attribute(FILE* file, cp_info** cp, u2 cp_count);
SourceFile_attribute* read_source_file_attribute(FILE* file);
LineNumberTable_attribute* read_line_number_table_attribute(FILE* file);

void free_class_file(ClassFile* class_file);
void free_attributes(u2 count, attribute_info* attributes, cp_info** cp, u2 cp_count);
void free_fields(u2 count, field_info* fields, cp_info** cp, u2 cp_count);
void free_methods(u2 count, method_info* methods, cp_info** cp, u2 cp_count);

// Funções swap (podem ficar aqui ou em common.h/c)
u2 swap_u2(u2 val);
u4 swap_u4(u4 val);
// --- FIM DAS DECLARAÇÕES ---

#endif // LEITOR_CLASS_H