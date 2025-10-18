#ifndef LEITOR_CLASS_H
#define LEITOR_CLASS_H

#include "common.h"
#include "constant_pool.h"
#include <stdio.h>

// As definições de attribute, field e method
typedef struct attribute_info {
    u2 attribute_name_index;
    u4 attribute_length;
    u1* info;
} attribute_info;

typedef struct field_info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info* attributes;
} field_info;

typedef struct method_info {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info* attributes;
} method_info;

// Estrutura para armazenar as informações do arquivo .class
typedef struct {
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    cp_info** constant_pool;
    u2 access_flags;
    u2 this_class;
    u2 super_class;
    u2 interfaces_count;
    u2* interfaces; // Array de índices para o Constant Pool
    u2 fields_count;
    field_info* fields;
    u2 methods_count;
    method_info* methods;
    u2 attributes_count;
    attribute_info* attributes;
} ClassFile;

// Declaração da função que vai ler o arquivo .class
// Ela retorna um ponteiro para a struct com os dados lidos
ClassFile* read_class_file(const char* filename);

// Declaração da função para liberar a memória alocada para a struct
void free_class_file(ClassFile* class_file);

// Declaração da função para exibir as informações lidas
void print_class_file_info(ClassFile* class_file);


#endif // LEITOR_CLASS_H