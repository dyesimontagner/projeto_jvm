#include "leitor_class.h"
#include <stdlib.h>

// Implementação das funções de swap de bytes
u2 swap_u2(u2 val) {
    return (val << 8) | (val >> 8);
}

u4 swap_u4(u4 val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val << 16) | (val >> 16);
}

// Implementação da função principal de leitura
ClassFile* read_class_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Erro especifico ao abrir arquivo");
        return NULL;
    }

    // Usar calloc para garantir que a memória seja inicializada com zeros
    ClassFile* class_file = (ClassFile*) calloc(1, sizeof(ClassFile));
    if (!class_file) {
        fprintf(stderr, "Erro: Falha na alocacao de memoria.\n");
        fclose(file);
        return NULL;
    }
    
    // Lê o magic number
    if (fread(&class_file->magic, sizeof(u4), 1, file) != 1) {
        fprintf(stderr, "Erro: Falha ao ler o magic number do arquivo.\n");
        free_class_file(class_file); // free_class_file cuida de liberar o que já foi alocado
        fclose(file);
        return NULL;
    }
    class_file->magic = swap_u4(class_file->magic);

    // Valida o magic number
    if (class_file->magic != 0xCAFEBABE) {
        fprintf(stderr, "Erro: Magic number invalido! Esperado 0xCAFEBABE, mas o arquivo tem 0x%X.\n", class_file->magic);
        free_class_file(class_file);
        fclose(file);
        return NULL;
    }

    // Lê a minor version
    if (fread(&class_file->minor_version, sizeof(u2), 1, file) != 1) {
        fprintf(stderr, "Erro: Falha ao ler a minor version.\n");
        free_class_file(class_file);
        fclose(file);
        return NULL;
    }
    class_file->minor_version = swap_u2(class_file->minor_version);
    
    // Lê a major version
    if (fread(&class_file->major_version, sizeof(u2), 1, file) != 1) {
        fprintf(stderr, "Erro: Falha ao ler a major version.\n");
        free_class_file(class_file);
        fclose(file);
        return NULL;
    }
    class_file->major_version = swap_u2(class_file->major_version);

    // LÊ O CONSTANT POOL COUNT
    if (fread(&class_file->constant_pool_count, sizeof(u2), 1, file) != 1) {
        fprintf(stderr, "Erro: Falha ao ler o constant_pool_count.\n");
        free_class_file(class_file);
        fclose(file);
        return NULL;
    }
    class_file->constant_pool_count = swap_u2(class_file->constant_pool_count);
    
    // Aloca memória para o array de ponteiros do constant pool
    u2 cp_count = class_file->constant_pool_count;
    if (cp_count > 0) {
        class_file->constant_pool = (cp_info**) calloc(cp_count, sizeof(cp_info*));
        if (!class_file->constant_pool) {
            fprintf(stderr, "Erro: Falha na alocacao de memoria para o constant pool.\n");
            free_class_file(class_file);
            fclose(file);
            return NULL;
        }
        // CHAMA A FUNÇÃO PARA LER O CONSTANT POOL
        read_constant_pool(cp_count, class_file->constant_pool, file);
    }

    fclose(file);
    return class_file;
}

// Implementação da função de exibição
void print_class_file_info(ClassFile* class_file) {
    printf("---- Informacoes do .class ----\n");
    printf("Magic Number: 0x%X\n", class_file->magic);
    printf("Minor Version: %u\n", class_file->minor_version);
    printf("Major Version: %u (Java %ld)\n", class_file->major_version, (long)class_file->major_version - 44);
    printf("Constant Pool Count: %u\n", class_file->constant_pool_count);

    // Chama a função para exibir o constant pool
    if (class_file->constant_pool) {
        printf("\n---- Constant Pool ----\n");
        print_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
    }
}

// Implementação da função para liberar memória
void free_class_file(ClassFile* class_file) {
    if (class_file) {
        // Se o constant pool foi alocado, chama a função para liberá-lo
        if (class_file->constant_pool) {
            free_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
        }
        // Libera a struct principal
        free(class_file);
    }
}