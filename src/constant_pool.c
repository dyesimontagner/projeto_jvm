#include "constant_pool.h"
#include <stdlib.h>
#include <inttypes.h> // Para conversão de Long
#include "leitor_class.h" 

// Função para ler Utf8 (sem alterações)
void read_utf8_info(cp_info* entry, FILE* file) {
    fread(&entry->info.utf8_info.length, sizeof(u2), 1, file);
    entry->info.utf8_info.length = swap_u2(entry->info.utf8_info.length);
    u2 length = entry->info.utf8_info.length;
    entry->info.utf8_info.bytes = (u1*) malloc(length + 1);
    fread(entry->info.utf8_info.bytes, sizeof(u1), length, file);
    entry->info.utf8_info.bytes[length] = '\0';
}

// Função principal de leitura com os 'cases' corrigidos
void read_constant_pool(u2 count, cp_info** constant_pool, FILE* file) {
    for (int i = 1; i < count; i++) {
        constant_pool[i] = (cp_info*) calloc(1, sizeof(cp_info));
        cp_info* entry = constant_pool[i];

        if (fread(&entry->tag, sizeof(u1), 1, file) != 1) {
            fprintf(stderr, "Erro de leitura na tag do constant pool no indice %d.\n", i);
            exit(1);
        }

        switch(entry->tag) {
            case CONSTANT_Utf8:
                read_utf8_info(entry, file);
                break;
            case CONSTANT_Class:
                fread(&entry->info.class_info.name_index, sizeof(u2), 1, file);
                entry->info.class_info.name_index = swap_u2(entry->info.class_info.name_index);
                break;
            case CONSTANT_String:
                fread(&entry->info.string_info.string_index, sizeof(u2), 1, file);
                entry->info.string_info.string_index = swap_u2(entry->info.string_info.string_index);
                break;
            case CONSTANT_Fieldref:
                fread(&entry->info.fieldref_info.class_index, sizeof(u2), 1, file);
                entry->info.fieldref_info.class_index = swap_u2(entry->info.fieldref_info.class_index);
                fread(&entry->info.fieldref_info.name_and_type_index, sizeof(u2), 1, file);
                entry->info.fieldref_info.name_and_type_index = swap_u2(entry->info.fieldref_info.name_and_type_index);
                break;
            case CONSTANT_Methodref:
                fread(&entry->info.methodref_info.class_index, sizeof(u2), 1, file);
                entry->info.methodref_info.class_index = swap_u2(entry->info.methodref_info.class_index);
                fread(&entry->info.methodref_info.name_and_type_index, sizeof(u2), 1, file);
                entry->info.methodref_info.name_and_type_index = swap_u2(entry->info.methodref_info.name_and_type_index);
                break;
            case CONSTANT_InterfaceMethodref:
                 fread(&entry->info.interface_methodref_info.class_index, sizeof(u2), 1, file);
                entry->info.interface_methodref_info.class_index = swap_u2(entry->info.interface_methodref_info.class_index);
                fread(&entry->info.interface_methodref_info.name_and_type_index, sizeof(u2), 1, file);
                entry->info.interface_methodref_info.name_and_type_index = swap_u2(entry->info.interface_methodref_info.name_and_type_index);
                break;
            case CONSTANT_NameAndType:
                fread(&entry->info.name_and_type_info.name_index, sizeof(u2), 1, file);
                entry->info.name_and_type_info.name_index = swap_u2(entry->info.name_and_type_info.name_index);
                fread(&entry->info.name_and_type_info.descriptor_index, sizeof(u2), 1, file);
                entry->info.name_and_type_info.descriptor_index = swap_u2(entry->info.name_and_type_info.descriptor_index);
                break;
            case CONSTANT_Integer:
                fread(&entry->info.integer_info.bytes, sizeof(u4), 1, file);
                entry->info.integer_info.bytes = swap_u4(entry->info.integer_info.bytes);
                break;
            case CONSTANT_Float:
                fread(&entry->info.float_info.bytes, sizeof(u4), 1, file);
                entry->info.float_info.bytes = swap_u4(entry->info.float_info.bytes);
                break;
            case CONSTANT_Long:
                fread(&entry->info.long_info.high_bytes, sizeof(u4), 1, file);
                entry->info.long_info.high_bytes = swap_u4(entry->info.long_info.high_bytes);
                fread(&entry->info.long_info.low_bytes, sizeof(u4), 1, file);
                entry->info.long_info.low_bytes = swap_u4(entry->info.long_info.low_bytes);
                i++; // Pula o próximo índice
                break;
            case CONSTANT_Double:
                fread(&entry->info.double_info.high_bytes, sizeof(u4), 1, file);
                entry->info.double_info.high_bytes = swap_u4(entry->info.double_info.high_bytes);
                fread(&entry->info.double_info.low_bytes, sizeof(u4), 1, file);
                entry->info.double_info.low_bytes = swap_u4(entry->info.double_info.low_bytes);
                i++; // Pula o próximo índice
                break;
            default:
                printf("Tag invalida ou nao suportada: %u no indice %d\n", entry->tag, i);
                exit(1);
        }
    }
}

// Função para obter Utf8 (sem alterações)
char* get_utf8_from_pool(u2 index, cp_info** constant_pool) {
    return (char*) constant_pool[index]->info.utf8_info.bytes;
}

// Função de print com os 'cases' corrigidos
void print_constant_pool(u2 count, cp_info** constant_pool) {
    for (int i = 1; i < count; i++) {
        cp_info* entry = constant_pool[i];
        if (!entry) {
            printf("[%d] (Entrada Vazia - usada por Long/Double)\n", i);
            continue;
        };

        printf("[%d] ", i);
        switch(entry->tag) {
            case CONSTANT_Utf8:
                printf("Utf8: %s\n", entry->info.utf8_info.bytes);
                break;
            case CONSTANT_Class:
                printf("Class: indice=#%u <%s>\n", entry->info.class_info.name_index, get_utf8_from_pool(entry->info.class_info.name_index, constant_pool));
                break;
            case CONSTANT_String:
                printf("String: indice=#%u <%s>\n", entry->info.string_info.string_index, get_utf8_from_pool(entry->info.string_info.string_index, constant_pool));
                break;
            case CONSTANT_Fieldref:
                printf("Fieldref: class_indice=#%u, name_and_type_indice=#%u\n", entry->info.fieldref_info.class_index, entry->info.fieldref_info.name_and_type_index);
                break;
            case CONSTANT_Methodref:
                printf("Methodref: class_indice=#%u, name_and_type_indice=#%u\n", entry->info.methodref_info.class_index, entry->info.methodref_info.name_and_type_index);
                break;
            case CONSTANT_InterfaceMethodref:
                 printf("InterfaceMethodref: class_indice=#%u, name_and_type_indice=#%u\n", entry->info.interface_methodref_info.class_index, entry->info.interface_methodref_info.name_and_type_index);
                break;
            case CONSTANT_NameAndType:
                printf("NameAndType: name_indice=#%u <%s>, descriptor_indice=#%u <%s>\n", 
                    entry->info.name_and_type_info.name_index, get_utf8_from_pool(entry->info.name_and_type_info.name_index, constant_pool),
                    entry->info.name_and_type_info.descriptor_index, get_utf8_from_pool(entry->info.name_and_type_info.descriptor_index, constant_pool));
                break;
            case CONSTANT_Integer:
                printf("Integer: %d\n", (int32_t)entry->info.integer_info.bytes);
                break;
            case CONSTANT_Float:
                 printf("Float: %f\n", (float)entry->info.float_info.bytes);
                 break;
            case CONSTANT_Long: ; // Ponto e vírgula para permitir declaração de variável
                 int64_t long_val = ((int64_t)entry->info.long_info.high_bytes << 32) | entry->info.long_info.low_bytes;
                 printf("Long: %" PRId64 "\n", long_val);
                 i++;
                 break;
            case CONSTANT_Double:
                 printf("Double\n"); // Implementação da conversão de double é mais complexa
                 i++;
                 break;
            default:
                printf("Tag Desconhecida: %u\n", entry->tag);
                break;
        }
    }
}

// Função free (sem alterações)
void free_constant_pool(u2 count, cp_info** constant_pool) {
    for (int i = 1; i < count; i++) {
        cp_info* entry = constant_pool[i];
        if (entry) {
            if (entry->tag == CONSTANT_Utf8) {
                free(entry->info.utf8_info.bytes);
            }
            free(entry);
        }
    }
    free(constant_pool);
}