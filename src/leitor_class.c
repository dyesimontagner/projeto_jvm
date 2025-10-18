#include "leitor_class.h"
#include "constant_pool.h"
#include <stdlib.h>

// Implementação das funções de swap de bytes
u2 swap_u2(u2 val) {
    return (val << 8) | (val >> 8);
}

u4 swap_u4(u4 val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val << 16) | (val >> 16);
}

// FUNÇÕES HELPER PARA LEITURA (NOVAS)
void read_attributes(u2 count, attribute_info** attributes, FILE* file) {
    *attributes = (attribute_info*) calloc(count, sizeof(attribute_info));
    for (int i = 0; i < count; i++) {
        fread(&(*attributes)[i].attribute_name_index, sizeof(u2), 1, file);
        (*attributes)[i].attribute_name_index = swap_u2((*attributes)[i].attribute_name_index);
        fread(&(*attributes)[i].attribute_length, sizeof(u4), 1, file);
        (*attributes)[i].attribute_length = swap_u4((*attributes)[i].attribute_length);
        
        (*attributes)[i].info = (u1*) malloc((*attributes)[i].attribute_length);
        fread((*attributes)[i].info, sizeof(u1), (*attributes)[i].attribute_length, file);
    }
}

void read_fields(u2 count, field_info** fields, FILE* file) {
    *fields = (field_info*) calloc(count, sizeof(field_info));
    for (int i = 0; i < count; i++) {
        fread(&(*fields)[i].access_flags, sizeof(u2), 1, file);
        (*fields)[i].access_flags = swap_u2((*fields)[i].access_flags);
        fread(&(*fields)[i].name_index, sizeof(u2), 1, file);
        (*fields)[i].name_index = swap_u2((*fields)[i].name_index);
        fread(&(*fields)[i].descriptor_index, sizeof(u2), 1, file);
        (*fields)[i].descriptor_index = swap_u2((*fields)[i].descriptor_index);
        fread(&(*fields)[i].attributes_count, sizeof(u2), 1, file);
        (*fields)[i].attributes_count = swap_u2((*fields)[i].attributes_count);
        read_attributes((*fields)[i].attributes_count, &(*fields)[i].attributes, file);
    }
}

void read_methods(u2 count, method_info** methods, FILE* file) {
    *methods = (method_info*) calloc(count, sizeof(method_info));
     for (int i = 0; i < count; i++) {
        fread(&(*methods)[i].access_flags, sizeof(u2), 1, file);
        (*methods)[i].access_flags = swap_u2((*methods)[i].access_flags);
        fread(&(*methods)[i].name_index, sizeof(u2), 1, file);
        (*methods)[i].name_index = swap_u2((*methods)[i].name_index);
        fread(&(*methods)[i].descriptor_index, sizeof(u2), 1, file);
        (*methods)[i].descriptor_index = swap_u2((*methods)[i].descriptor_index);
        fread(&(*methods)[i].attributes_count, sizeof(u2), 1, file);
        (*methods)[i].attributes_count = swap_u2((*methods)[i].attributes_count);
        read_attributes((*methods)[i].attributes_count, &(*methods)[i].attributes, file);
    }
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

    // LER ACCESS FLAGS, THIS_CLASS, SUPER_CLASS
    fread(&class_file->access_flags, sizeof(u2), 1, file);
    class_file->access_flags = swap_u2(class_file->access_flags);
    fread(&class_file->this_class, sizeof(u2), 1, file);
    class_file->this_class = swap_u2(class_file->this_class);
    fread(&class_file->super_class, sizeof(u2), 1, file);
    class_file->super_class = swap_u2(class_file->super_class);

    // LER INTERFACES
    fread(&class_file->interfaces_count, sizeof(u2), 1, file);
    class_file->interfaces_count = swap_u2(class_file->interfaces_count);
    if (class_file->interfaces_count > 0) {
        class_file->interfaces = (u2*) calloc(class_file->interfaces_count, sizeof(u2));
        for (int i = 0; i < class_file->interfaces_count; i++) {
            fread(&class_file->interfaces[i], sizeof(u2), 1, file);
            class_file->interfaces[i] = swap_u2(class_file->interfaces[i]);
        }
    }

    // LER FIELDS
    fread(&class_file->fields_count, sizeof(u2), 1, file);
    class_file->fields_count = swap_u2(class_file->fields_count);
    read_fields(class_file->fields_count, &class_file->fields, file);

    // LER METHODS
    fread(&class_file->methods_count, sizeof(u2), 1, file);
    class_file->methods_count = swap_u2(class_file->methods_count);
    read_methods(class_file->methods_count, &class_file->methods, file);
    
    // LER ATTRIBUTES (da classe)
    fread(&class_file->attributes_count, sizeof(u2), 1, file);
    class_file->attributes_count = swap_u2(class_file->attributes_count);
    read_attributes(class_file->attributes_count, &class_file->attributes, file);

    fclose(file);
    return class_file;
}

// Implementação da função de exibição
void print_class_file_info(ClassFile* class_file) {
    printf("---- General Information ----\n");
    printf("Magic: 0x%X\n", class_file->magic);
    printf("Minor version: %u\n", class_file->minor_version);
    printf("Major version: %u (Java %ld)\n", class_file->major_version, (long)class_file->major_version - 44);
    printf("Constant pool count: %u\n", class_file->constant_pool_count);
    printf("Access flags: 0x%.4X\n", class_file->access_flags); // Adicionar decodificador aqui depois

    // CORREÇÃO: Checa se o constant_pool existe ANTES de usá-lo.
    if (class_file->constant_pool) {
        printf("This class: cp_info #%u <%s>\n", class_file->this_class, get_utf8_from_pool(class_file->constant_pool[class_file->this_class]->info.class_info.name_index, class_file->constant_pool));
        printf("Super class: cp_info #%u <%s>\n", class_file->super_class, get_utf8_from_pool(class_file->constant_pool[class_file->super_class]->info.class_info.name_index, class_file->constant_pool));
    }

    printf("Interfaces count: %u\n", class_file->interfaces_count);
    printf("Fields count: %u\n", class_file->fields_count);
    printf("Methods count: %u\n", class_file->methods_count);
    printf("Attributes count: %u\n", class_file->attributes_count);

    if (class_file->constant_pool) {
        printf("\n---- Constant Pool ----\n");
        print_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
    }
    
    if (class_file->fields_count > 0) {
        printf("\n---- Fields ----\n");
        for (int i = 0; i < class_file->fields_count; i++) {
            printf("[%d] Name: cp_info #%u <%s>\n", i, class_file->fields[i].name_index, get_utf8_from_pool(class_file->fields[i].name_index, class_file->constant_pool));
        }
    }

    if (class_file->methods_count > 0) {
        printf("\n---- Methods ----\n");
        for (int i = 0; i < class_file->methods_count; i++) {
            printf("[%d] Name: cp_info #%u <%s>\n", i, class_file->methods[i].name_index, get_utf8_from_pool(class_file->methods[i].name_index, class_file->constant_pool));
            printf("     Descriptor: cp_info #%u <%s>\n", class_file->methods[i].descriptor_index, get_utf8_from_pool(class_file->methods[i].descriptor_index, class_file->constant_pool));
        }
    }
}

// NOVAS FUNÇÕES HELPER PARA LIBERAR MEMÓRIA
void free_attributes(u2 count, attribute_info* attributes) {
    if (!attributes) return;
    for (int i = 0; i < count; i++) {
        if (attributes[i].info) {
            free(attributes[i].info); // Libera o conteúdo de cada atributo
        }
    }
    free(attributes); // Libera o array de atributos
}

void free_fields(u2 count, field_info* fields) {
    if (!fields) return;
    for (int i = 0; i < count; i++) {
        // Libera os atributos dentro de cada field
        free_attributes(fields[i].attributes_count, fields[i].attributes);
    }
    free(fields); // Libera o array de fields
}

void free_methods(u2 count, method_info* methods) {
    if (!methods) return;
    for (int i = 0; i < count; i++) {
        // Libera os atributos dentro de cada method
        free_attributes(methods[i].attributes_count, methods[i].attributes);
    }
    free(methods); // Libera o array de methods
}


// VERSÃO FINAL E COMPLETA DA FUNÇÃO free_class_file
void free_class_file(ClassFile* class_file) {
    if (class_file) {
        if (class_file->constant_pool) free_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
        if (class_file->interfaces) free(class_file->interfaces);
        
        // Chama as novas funções de free
        free_fields(class_file->fields_count, class_file->fields);
        free_methods(class_file->methods_count, class_file->methods);
        free_attributes(class_file->attributes_count, class_file->attributes);
        
        // Libera a struct principal
        free(class_file);
    }
}