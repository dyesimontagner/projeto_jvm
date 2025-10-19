#ifndef LEITOR_CLASS_H
#define LEITOR_CLASS_H

#include "common.h"
#include "constant_pool.h"
#include <stdio.h>

// Estrutura para a tabela de exceções dentro do atributo Code
typedef struct {
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
} exception_table_entry;

// ESTRUTURA PARA O ATRIBUTO "CODE"
typedef struct {
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1* code; // O array de bytecodes!
    u2 exception_table_length;
    exception_table_entry* exception_table;
    u2 attributes_count;
    struct attribute_info* attributes; // Atributos podem ser aninhados
} Code_attribute;

// Estrutura genérica de atributo agora inclui um ponteiro para o Code_attribute
typedef struct attribute_info {
    u2 attribute_name_index;
    u4 attribute_length;
    union { // Usaremos uma union para armazenar ou os bytes brutos ou a estrutura Code
        u1* info;
        Code_attribute* code_info;
    } attr_info;
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