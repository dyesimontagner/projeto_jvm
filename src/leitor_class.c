#include "leitor_class.h"
#include "constant_pool.h"
#include <stdlib.h>
#include <string.h> // Incluído para usar a função strcmp

// Funções de swap 
u2 swap_u2(u2 val) { return (val << 8) | (val >> 8); }

u4 swap_u4(u4 val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val << 16) | (val >> 16);
}

/// Protótipo necessário porque as funções se chamam recursivamente
void read_attributes(u2 count, attribute_info** attributes, FILE* file, cp_info** cp);

// Função para ler o atributo "Code"
Code_attribute* read_code_attribute(FILE* file, cp_info** cp) {
    Code_attribute* code_attr = (Code_attribute*) calloc(1, sizeof(Code_attribute));
    fread(&code_attr->max_stack, sizeof(u2), 1, file);
    code_attr->max_stack = swap_u2(code_attr->max_stack);
    fread(&code_attr->max_locals, sizeof(u2), 1, file);
    code_attr->max_locals = swap_u2(code_attr->max_locals);
    fread(&code_attr->code_length, sizeof(u4), 1, file);
    code_attr->code_length = swap_u4(code_attr->code_length);
    code_attr->code = (u1*) malloc(code_attr->code_length);
    fread(code_attr->code, sizeof(u1), code_attr->code_length, file);
    fread(&code_attr->exception_table_length, sizeof(u2), 1, file);
    code_attr->exception_table_length = swap_u2(code_attr->exception_table_length);
    fseek(file, code_attr->exception_table_length * 8, SEEK_CUR); // Pula a tabela de exceções
    fread(&code_attr->attributes_count, sizeof(u2), 1, file);
    code_attr->attributes_count = swap_u2(code_attr->attributes_count);
    read_attributes(code_attr->attributes_count, &code_attr->attributes, file, cp);
    return code_attr;
}
// Função para ler atributos (corrigida)
void read_attributes(u2 count, attribute_info** attributes, FILE* file, cp_info** cp) {
    *attributes = (attribute_info*) calloc(count, sizeof(attribute_info));
    for (int i = 0; i < count; i++) {
        attribute_info* attr = &(*attributes)[i];
        fread(&attr->attribute_name_index, sizeof(u2), 1, file);
        attr->attribute_name_index = swap_u2(attr->attribute_name_index);
        fread(&attr->attribute_length, sizeof(u4), 1, file);
        attr->attribute_length = swap_u4(attr->attribute_length);
        
        const char* attr_name = get_utf8_from_pool(attr->attribute_name_index, cp);
        if (strcmp(attr_name, "Code") == 0) {
            attr->attr_info.code_info = read_code_attribute(file, cp);
        } else {
            attr->attr_info.info = (u1*) malloc(attr->attribute_length);
            fread(attr->attr_info.info, sizeof(u1), attr->attribute_length, file);
        }
    }
}

// Funções read_fields e read_methods ATUALIZADAS para passar o constant pool
void read_fields(u2 count, field_info** fields, FILE* file, cp_info** cp) {
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
        read_attributes((*fields)[i].attributes_count, &(*fields)[i].attributes, file, cp);
    }
}

void read_methods(u2 count, method_info** methods, FILE* file, cp_info** cp) {
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
        read_attributes((*methods)[i].attributes_count, &(*methods)[i].attributes, file, cp);
    }
}

// Implementação da função principal de leitura
ClassFile* read_class_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) { perror("Erro ao abrir arquivo"); return NULL; }

    ClassFile* class_file = (ClassFile*) calloc(1, sizeof(ClassFile));
    if (!class_file) { fprintf(stderr, "Erro de alocacao de memoria.\n"); fclose(file); return NULL; }
    
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
    fread(&class_file->constant_pool_count, sizeof(u2), 1, file);
    class_file->constant_pool_count = swap_u2(class_file->constant_pool_count);
    u2 cp_count = class_file->constant_pool_count;
    if (cp_count > 0) {
        class_file->constant_pool = (cp_info**) calloc(cp_count, sizeof(cp_info*));
        read_constant_pool(cp_count, class_file->constant_pool, file);
    }

    // Leitura do resto do arquivo
    fread(&class_file->access_flags, sizeof(u2), 1, file);
    class_file->access_flags = swap_u2(class_file->access_flags);
    fread(&class_file->this_class, sizeof(u2), 1, file);
    class_file->this_class = swap_u2(class_file->this_class);
    fread(&class_file->super_class, sizeof(u2), 1, file);
    class_file->super_class = swap_u2(class_file->super_class);

    fread(&class_file->interfaces_count, sizeof(u2), 1, file);
    class_file->interfaces_count = swap_u2(class_file->interfaces_count);
    if (class_file->interfaces_count > 0) { /* ... (código para ler interfaces) ... */ }

    fread(&class_file->fields_count, sizeof(u2), 1, file);
    class_file->fields_count = swap_u2(class_file->fields_count);
    read_fields(class_file->fields_count, &class_file->fields, file, class_file->constant_pool);

    fread(&class_file->methods_count, sizeof(u2), 1, file);
    class_file->methods_count = swap_u2(class_file->methods_count);
    read_methods(class_file->methods_count, &class_file->methods, file, class_file->constant_pool);
    
    fread(&class_file->attributes_count, sizeof(u2), 1, file);
    class_file->attributes_count = swap_u2(class_file->attributes_count);
    read_attributes(class_file->attributes_count, &class_file->attributes, file, class_file->constant_pool);

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
    
    if (class_file->fields_count > 0 && class_file->constant_pool) {
        printf("\n---- Fields ----\n");
        for (int i = 0; i < class_file->fields_count; i++) {
            printf("[%d] Name: cp_info #%u <%s>\n", i, class_file->fields[i].name_index, get_utf8_from_pool(class_file->fields[i].name_index, class_file->constant_pool));
        }
    }

    if (class_file->methods_count > 0 && class_file->constant_pool) {
        printf("\n---- Methods ----\n");
        for (int i = 0; i < class_file->methods_count; i++) {
            method_info* method = &class_file->methods[i];
            printf("[%d] Name: cp_info #%u <%s>\n", i, method->name_index, get_utf8_from_pool(method->name_index, class_file->constant_pool));
            printf("     Descriptor: cp_info #%u <%s>\n", method->descriptor_index, get_utf8_from_pool(method->descriptor_index, class_file->constant_pool));
            
            for (int j = 0; j < method->attributes_count; j++) {
                attribute_info* attr = &method->attributes[j];
                const char* attr_name = get_utf8_from_pool(attr->attribute_name_index, class_file->constant_pool);
                if (strcmp(attr_name, "Code") == 0) {
                    const Code_attribute* code = attr->attr_info.code_info;
                    printf("     Code:\n");
                    printf("       max_stack=%u, max_locals=%u, code_length=%u\n", code->max_stack, code->max_locals, code->code_length);
                    for (int k = 0; k < code->code_length; k++) {
                        printf("         %d: 0x%.2x\n", k, code->code[k]);
                    }
                }
            }
        }
    }
}

// NOVAS FUNÇÕES HELPER PARA LIBERAR MEMÓRIA
void free_attributes(u2 count, attribute_info* attributes, cp_info** cp) {
    if (!attributes) return;
    for (int i = 0; i < count; i++) {
        const char* attr_name = get_utf8_from_pool(attributes[i].attribute_name_index, cp);
        if (strcmp(attr_name, "Code") == 0) {
            Code_attribute* code = attributes[i].attr_info.code_info;
            if (code) {
                if (code->code) free(code->code);
                free_attributes(code->attributes_count, code->attributes, cp);
                free(code);
            }
        } else {
            if (attributes[i].attr_info.info) free(attributes[i].attr_info.info);
        }
    }
    free(attributes);
}

void free_fields(u2 count, field_info* fields, cp_info** cp) {
    if (!fields) return;
    for (int i = 0; i < count; i++) {
        free_attributes(fields[i].attributes_count, fields[i].attributes, cp);
    }
    free(fields);
}

void free_methods(u2 count, method_info* methods, cp_info** cp) {
    if (!methods) return;
    for (int i = 0; i < count; i++) {
        free_attributes(methods[i].attributes_count, methods[i].attributes, cp);
    }
    free(methods);
}


// VERSÃO FINAL E COMPLETA DA FUNÇÃO free_class_file
void free_class_file(ClassFile* class_file) {
    if (class_file) {
        if (class_file->constant_pool) free_constant_pool(class_file->constant_pool_count, class_file->constant_pool);
        if (class_file->interfaces) free(class_file->interfaces);
        free_fields(class_file->fields_count, class_file->fields, class_file->constant_pool);
        free_methods(class_file->methods_count, class_file->methods, class_file->constant_pool);
        free_attributes(class_file->attributes_count, class_file->attributes, class_file->constant_pool);
        free(class_file);
    }
}