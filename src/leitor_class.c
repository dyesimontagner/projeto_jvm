#include "leitor_class.h"
#include "constant_pool.h" // Necessário para get_utf8_from_pool em read_attributes
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h> // Para offsetof

// Funções de swap (mantidas aqui)
u2 swap_u2(u2 val) { return (val << 8) | (val >> 8); }
u4 swap_u4(u4 val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val << 16) | (val >> 16);
}

// Protótipos internos
void read_fields(u2 count, field_info** fields, FILE* file, cp_info** cp, u2 cp_count);
void read_methods(u2 count, method_info** methods, FILE* file, cp_info** cp, u2 cp_count);

// --- Funções de Leitura de Atributos Específicos ---
Code_attribute* read_code_attribute(FILE* file, cp_info** cp, u2 cp_count) {
    Code_attribute* code_attr = (Code_attribute*) calloc(1, sizeof(Code_attribute));
    if (!code_attr) { fprintf(stderr, "Erro de alocacao para Code attribute.\n"); return NULL; }

    fread(&code_attr->max_stack, sizeof(u2), 1, file);
    code_attr->max_stack = swap_u2(code_attr->max_stack);
    fread(&code_attr->max_locals, sizeof(u2), 1, file);
    code_attr->max_locals = swap_u2(code_attr->max_locals);
    fread(&code_attr->code_length, sizeof(u4), 1, file);
    code_attr->code_length = swap_u4(code_attr->code_length);

    if (code_attr->code_length > 0) {
        code_attr->code = (u1*) malloc(code_attr->code_length);
        if (!code_attr->code) {
             fprintf(stderr, "Erro de alocacao para bytecode.\n");
             free(code_attr); return NULL;
        }
        fread(code_attr->code, sizeof(u1), code_attr->code_length, file);
    } else {
        code_attr->code = NULL;
    }

    fread(&code_attr->exception_table_length, sizeof(u2), 1, file);
    code_attr->exception_table_length = swap_u2(code_attr->exception_table_length);
    if (code_attr->exception_table_length > 0) {
        code_attr->exception_table = (exception_table_entry*) calloc(code_attr->exception_table_length, sizeof(exception_table_entry));
         if (!code_attr->exception_table) {
             fprintf(stderr, "Erro de alocacao para exception table.\n");
             if(code_attr->code) free(code_attr->code);
             free(code_attr); return NULL;
         }
        for(int i=0; i < code_attr->exception_table_length; ++i) {
            fread(&code_attr->exception_table[i].start_pc, sizeof(u2), 1, file);
            code_attr->exception_table[i].start_pc = swap_u2(code_attr->exception_table[i].start_pc);
            fread(&code_attr->exception_table[i].end_pc, sizeof(u2), 1, file);
            code_attr->exception_table[i].end_pc = swap_u2(code_attr->exception_table[i].end_pc);
            fread(&code_attr->exception_table[i].handler_pc, sizeof(u2), 1, file);
            code_attr->exception_table[i].handler_pc = swap_u2(code_attr->exception_table[i].handler_pc);
            fread(&code_attr->exception_table[i].catch_type, sizeof(u2), 1, file);
            code_attr->exception_table[i].catch_type = swap_u2(code_attr->exception_table[i].catch_type);
        }
    } else {
        code_attr->exception_table = NULL;
    }

    fread(&code_attr->attributes_count, sizeof(u2), 1, file);
    code_attr->attributes_count = swap_u2(code_attr->attributes_count);
    read_attributes(code_attr->attributes_count, &code_attr->attributes, file, cp, cp_count);

    return code_attr;
}

SourceFile_attribute* read_source_file_attribute(FILE* file) {
    SourceFile_attribute* sf_attr = (SourceFile_attribute*) calloc(1, sizeof(SourceFile_attribute));
    if (!sf_attr) { fprintf(stderr, "Erro de alocacao para SourceFile attribute.\n"); return NULL; }
    fread(&sf_attr->sourcefile_index, sizeof(u2), 1, file);
    sf_attr->sourcefile_index = swap_u2(sf_attr->sourcefile_index);
    return sf_attr;
}

LineNumberTable_attribute* read_line_number_table_attribute(FILE* file) {
    LineNumberTable_attribute* lnt_attr = (LineNumberTable_attribute*) calloc(1, sizeof(LineNumberTable_attribute));
    if (!lnt_attr) { fprintf(stderr, "Erro de alocacao para LineNumberTable attribute.\n"); return NULL; }
    fread(&lnt_attr->line_number_table_length, sizeof(u2), 1, file);
    lnt_attr->line_number_table_length = swap_u2(lnt_attr->line_number_table_length);

    if (lnt_attr->line_number_table_length > 0) {
        lnt_attr->table = (line_number_table_entry*) calloc(lnt_attr->line_number_table_length, sizeof(line_number_table_entry));
        if (!lnt_attr->table) {
             fprintf(stderr, "Erro de alocacao para LineNumberTable entries.\n");
             free(lnt_attr);
             return NULL;
        }
        for (int i = 0; i < lnt_attr->line_number_table_length; i++) {
            fread(&lnt_attr->table[i].start_pc, sizeof(u2), 1, file);
            lnt_attr->table[i].start_pc = swap_u2(lnt_attr->table[i].start_pc);
            fread(&lnt_attr->table[i].line_number, sizeof(u2), 1, file);
            lnt_attr->table[i].line_number = swap_u2(lnt_attr->table[i].line_number);
        }
    } else {
        lnt_attr->table = NULL;
    }
    return lnt_attr;
}
// --- Fim das Funções de Leitura de Atributos Específicos ---

// Função para ler atributos
void read_attributes(u2 count, attribute_info** attributes, FILE* file, cp_info** cp, u2 cp_count) {
    if (count == 0) {
         *attributes = NULL;
         return;
    }
    *attributes = (attribute_info*) calloc(count, sizeof(attribute_info));
     if (!*attributes) { fprintf(stderr, "Erro de alocacao para attributes array.\n"); return; }

    for (int i = 0; i < count; i++) {
        attribute_info* attr = &(*attributes)[i];
        fread(&attr->attribute_name_index, sizeof(u2), 1, file);
        attr->attribute_name_index = swap_u2(attr->attribute_name_index);
        fread(&attr->attribute_length, sizeof(u4), 1, file);
        attr->attribute_length = swap_u4(attr->attribute_length);

        long current_pos = ftell(file);
        const char* attr_name = get_utf8_from_pool(attr->attribute_name_index, cp, cp_count);

        memset(&attr->attr_info, 0, sizeof(attr->attr_info)); // Zera a union

        if (strcmp(attr_name, "Code") == 0) {
            attr->attr_info.code_info = read_code_attribute(file, cp, cp_count);
        } else if (strcmp(attr_name, "SourceFile") == 0) {
             attr->attr_info.source_file_info = read_source_file_attribute(file);
        } else if (strcmp(attr_name, "LineNumberTable") == 0) {
             attr->attr_info.line_number_table_info = read_line_number_table_attribute(file);
        }
        // Adicionar 'else if' para outros atributos
        else {
             attr->attr_info.info = (u1*) malloc(attr->attribute_length);
             if (attr->attr_info.info) {
                 fread(attr->attr_info.info, sizeof(u1), attr->attribute_length, file);
             } else if (attr->attribute_length > 0) {
                 fprintf(stderr, "Erro de alocacao para bytes de atributo genérico '%s'.\n", attr_name);
                 fseek(file, attr->attribute_length, SEEK_CUR);
             }
        }
        long expected_end_pos = current_pos + attr->attribute_length;
        long actual_end_pos = ftell(file);
        if (actual_end_pos != expected_end_pos) {
             fprintf(stderr, "Aviso: Atributo '%s' (indice %u) leu %ld bytes, mas o tamanho esperado era %u. Ajustando posição do ficheiro.\n",
                     attr_name, attr->attribute_name_index, actual_end_pos - current_pos, attr->attribute_length);
             fseek(file, expected_end_pos, SEEK_SET);
        }
    }
}


// Funções read_fields e read_methods
void read_fields(u2 count, field_info** fields, FILE* file, cp_info** cp, u2 cp_count) {
    if (count == 0) { *fields = NULL; return; }
    *fields = (field_info*) calloc(count, sizeof(field_info));
    if (!*fields) { fprintf(stderr, "Erro de alocacao para fields array.\n"); return; }

    for (int i = 0; i < count; i++) {
        fread(&(*fields)[i].access_flags, sizeof(u2), 1, file);
        (*fields)[i].access_flags = swap_u2((*fields)[i].access_flags);
        fread(&(*fields)[i].name_index, sizeof(u2), 1, file);
        (*fields)[i].name_index = swap_u2((*fields)[i].name_index);
        fread(&(*fields)[i].descriptor_index, sizeof(u2), 1, file);
        (*fields)[i].descriptor_index = swap_u2((*fields)[i].descriptor_index);
        fread(&(*fields)[i].attributes_count, sizeof(u2), 1, file);
        (*fields)[i].attributes_count = swap_u2((*fields)[i].attributes_count);
        read_attributes((*fields)[i].attributes_count, &(*fields)[i].attributes, file, cp, cp_count);
    }
}

void read_methods(u2 count, method_info** methods, FILE* file, cp_info** cp, u2 cp_count) {
    if (count == 0) { *methods = NULL; return; }
    *methods = (method_info*) calloc(count, sizeof(method_info));
    if (!*methods) { fprintf(stderr, "Erro de alocacao para methods array.\n"); return; }

     for (int i = 0; i < count; i++) {
        fread(&(*methods)[i].access_flags, sizeof(u2), 1, file);
        (*methods)[i].access_flags = swap_u2((*methods)[i].access_flags);
        fread(&(*methods)[i].name_index, sizeof(u2), 1, file);
        (*methods)[i].name_index = swap_u2((*methods)[i].name_index);
        fread(&(*methods)[i].descriptor_index, sizeof(u2), 1, file);
        (*methods)[i].descriptor_index = swap_u2((*methods)[i].descriptor_index);
        fread(&(*methods)[i].attributes_count, sizeof(u2), 1, file);
        (*methods)[i].attributes_count = swap_u2((*methods)[i].attributes_count);
        read_attributes((*methods)[i].attributes_count, &(*methods)[i].attributes, file, cp, cp_count);
    }
}

// Implementação da função principal de leitura
ClassFile* read_class_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) { perror("Erro ao abrir arquivo"); return NULL; }

    ClassFile* class_file = (ClassFile*) calloc(1, sizeof(ClassFile));
    if (!class_file) { fprintf(stderr, "Erro de alocacao de memoria para ClassFile.\n"); fclose(file); return NULL; }

    int read_status = 0;

    if (fread(&class_file->magic, sizeof(u4), 1, file) != 1) read_status = 1;
    class_file->magic = swap_u4(class_file->magic);
    if (class_file->magic != 0xCAFEBABE) read_status = 2;

    if (read_status == 0 && fread(&class_file->minor_version, sizeof(u2), 1, file) != 1) read_status = 3;
    class_file->minor_version = swap_u2(class_file->minor_version);
    if (read_status == 0 && fread(&class_file->major_version, sizeof(u2), 1, file) != 1) read_status = 4;
    class_file->major_version = swap_u2(class_file->major_version);

    if (read_status == 0 && fread(&class_file->constant_pool_count, sizeof(u2), 1, file) != 1) read_status = 5;
    class_file->constant_pool_count = swap_u2(class_file->constant_pool_count);
    u2 cp_count = class_file->constant_pool_count;
    if (read_status == 0 && cp_count > 0) {
        class_file->constant_pool = (cp_info**) calloc(cp_count, sizeof(cp_info*));
        if (!class_file->constant_pool) read_status = 6;
        else read_constant_pool(cp_count, class_file->constant_pool, file);
    } else if (cp_count == 0) {
        class_file->constant_pool = NULL;
    }

    if (read_status == 0 && fread(&class_file->access_flags, sizeof(u2), 1, file) != 1) read_status = 7;
    class_file->access_flags = swap_u2(class_file->access_flags);
    if (read_status == 0 && fread(&class_file->this_class, sizeof(u2), 1, file) != 1) read_status = 8;
    class_file->this_class = swap_u2(class_file->this_class);
    if (read_status == 0 && fread(&class_file->super_class, sizeof(u2), 1, file) != 1) read_status = 9;
    class_file->super_class = swap_u2(class_file->super_class);

    if (read_status == 0 && fread(&class_file->interfaces_count, sizeof(u2), 1, file) != 1) read_status = 10;
    class_file->interfaces_count = swap_u2(class_file->interfaces_count);
    if (read_status == 0 && class_file->interfaces_count > 0) {
        class_file->interfaces = (u2*) calloc(class_file->interfaces_count, sizeof(u2));
        if (!class_file->interfaces) read_status = 11;
        else {
            for(int i=0; i<class_file->interfaces_count; ++i) {
                if (fread(&class_file->interfaces[i], sizeof(u2), 1, file) != 1) { read_status = 12; break; }
                class_file->interfaces[i] = swap_u2(class_file->interfaces[i]);
            }
        }
    } else {
        class_file->interfaces = NULL;
    }

    if (read_status == 0 && fread(&class_file->fields_count, sizeof(u2), 1, file) != 1) read_status = 13;
    class_file->fields_count = swap_u2(class_file->fields_count);
    if (read_status == 0) read_fields(class_file->fields_count, &class_file->fields, file, class_file->constant_pool, cp_count);

    if (read_status == 0 && fread(&class_file->methods_count, sizeof(u2), 1, file) != 1) read_status = 14;
    class_file->methods_count = swap_u2(class_file->methods_count);
    if (read_status == 0) read_methods(class_file->methods_count, &class_file->methods, file, class_file->constant_pool, cp_count);

    if (read_status == 0 && fread(&class_file->attributes_count, sizeof(u2), 1, file) != 1) read_status = 15;
    class_file->attributes_count = swap_u2(class_file->attributes_count);
    if (read_status == 0) read_attributes(class_file->attributes_count, &class_file->attributes, file, class_file->constant_pool, cp_count);

    fclose(file);

    if (read_status != 0) {
        fprintf(stderr, "Erro durante a leitura do ficheiro .class (código %d).\n", read_status);
        if(read_status == 2) fprintf(stderr, "   -> Magic number invalido! Esperado 0xCAFEBABE, mas o arquivo tem 0x%X.\n", class_file->magic);
        free_class_file(class_file);
        return NULL;
    }

    return class_file;
}

// --- Funções de Liberação de Memória ---
void free_attributes(u2 count, attribute_info* attributes, cp_info** cp, u2 cp_count) {
    if (!attributes || count == 0) return;

    for (int i = 0; i < count; i++) {
        const char* attr_name = "<unknown_or_invalid_cp>";
        if (cp_count > 0 && attributes[i].attribute_name_index > 0 && attributes[i].attribute_name_index < cp_count && cp[attributes[i].attribute_name_index]) {
           attr_name = get_utf8_from_pool(attributes[i].attribute_name_index, cp, cp_count);
        }

        if (strcmp(attr_name, "Code") == 0) {
            Code_attribute* code = attributes[i].attr_info.code_info;
            if (code) {
                if (code->code) free(code->code);
                if (code->exception_table) free(code->exception_table);
                free_attributes(code->attributes_count, code->attributes, cp, cp_count);
                free(code);
            }
        } else if (strcmp(attr_name, "SourceFile") == 0) {
             if (attributes[i].attr_info.source_file_info) free(attributes[i].attr_info.source_file_info);
        } else if (strcmp(attr_name, "LineNumberTable") == 0) {
             LineNumberTable_attribute* lnt = attributes[i].attr_info.line_number_table_info;
             if (lnt) {
                 if (lnt->table) free(lnt->table);
                 free(lnt);
             }
        }
        // Adicionar 'else if' para outros atributos
        else {
             if (attributes[i].attr_info.info) free(attributes[i].attr_info.info);
        }
         memset(&attributes[i].attr_info, 0, sizeof(attributes[i].attr_info));
    }
    free(attributes);
}

void free_fields(u2 count, field_info* fields, cp_info** cp, u2 cp_count) {
    if (!fields || count == 0) return;
    for (int i = 0; i < count; i++) {
        free_attributes(fields[i].attributes_count, fields[i].attributes, cp, cp_count);
    }
    free(fields);
}

void free_methods(u2 count, method_info* methods, cp_info** cp, u2 cp_count) {
    if (!methods || count == 0) return;
    for (int i = 0; i < count; i++) {
        free_attributes(methods[i].attributes_count, methods[i].attributes, cp, cp_count);
    }
    free(methods);
}

void free_class_file(ClassFile* class_file) {
    if (class_file) {
        cp_info** cp = class_file->constant_pool;
        u2 cp_count = class_file->constant_pool_count;

        free_fields(class_file->fields_count, class_file->fields, cp, cp_count);
        free_methods(class_file->methods_count, class_file->methods, cp, cp_count);
        free_attributes(class_file->attributes_count, class_file->attributes, cp, cp_count);

        if (cp) free_constant_pool(cp_count, cp);
        if (class_file->interfaces) free(class_file->interfaces);

        free(class_file);
    }
}