#ifndef LEITOR_CLASS_H
#define LEITOR_CLASS_H

#include "common.h"
#include "constant_pool.h"
#include <stdio.h>

// Estrutura para armazenar as informações do arquivo .class
typedef struct {
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    cp_info** constant_pool;
    // ... vamos adicionar os outros campos aqui depois
} ClassFile;

// Declaração da função que vai ler o arquivo .class
// Ela retorna um ponteiro para a struct com os dados lidos
ClassFile* read_class_file(const char* filename);

// Declaração da função para liberar a memória alocada para a struct
void free_class_file(ClassFile* class_file);

// Declaração da função para exibir as informações lidas
void print_class_file_info(ClassFile* class_file);


#endif // LEITOR_CLASS_H