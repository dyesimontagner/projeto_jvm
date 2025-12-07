#include "constant_pool.h"
#include <stdlib.h>
#include <inttypes.h> // Para PRId64
#include <string.h>   // Para strcmp
#include "leitor_class.h" // Necessário para swap_u2, swap_u4
#include "exibidor.h"   // <<< ADICIONADO: Necessário para get_float_from_bytes e get_double_from_bytes

// Função para ler Utf8
void read_utf8_info(cp_info* entry, FILE* file) {
    fread(&entry->info.utf8_info.length, sizeof(u2), 1, file);
    entry->info.utf8_info.length = swap_u2(entry->info.utf8_info.length);
    u2 length = entry->info.utf8_info.length;
    entry->info.utf8_info.bytes = (u1*) malloc(length + 1);
    if (!entry->info.utf8_info.bytes) {
        fprintf(stderr, "Erro de alocacao para bytes Utf8.\n");
        exit(1);
    }
    fread(entry->info.utf8_info.bytes, sizeof(u1), length, file);
    entry->info.utf8_info.bytes[length] = '\0';
}

// Função principal de leitura
void read_constant_pool(u2 count, cp_info** constant_pool, FILE* file) {
    for (int i = 1; i < count; i++) {
        constant_pool[i] = (cp_info*) calloc(1, sizeof(cp_info));
        if (!constant_pool[i]) {
            fprintf(stderr, "Erro de alocacao para entrada do constant pool no indice %d.\n", i);
            exit(1);
        }
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
            case CONSTANT_MethodType: // Tag 16
                fread(&entry->info.method_type_info.descriptor_index, sizeof(u2), 1, file);
                entry->info.method_type_info.descriptor_index = swap_u2(entry->info.method_type_info.descriptor_index);
                break;
            case CONSTANT_MethodHandle: 
                fread(&entry->info.method_handle_info.reference_kind, sizeof(u1), 1, file);
                fread(&entry->info.method_handle_info.reference_index, sizeof(u2), 1, file);
                entry->info.method_handle_info.reference_index = swap_u2(entry->info.method_handle_info.reference_index);
                break;
            case CONSTANT_InvokeDynamic: 
                fread(&entry->info.invoke_dynamic_info.bootstrap_method_attr_index, sizeof(u2), 1, file);
                entry->info.invoke_dynamic_info.bootstrap_method_attr_index = swap_u2(entry->info.invoke_dynamic_info.bootstrap_method_attr_index);
                fread(&entry->info.invoke_dynamic_info.name_and_type_index, sizeof(u2), 1, file);
                entry->info.invoke_dynamic_info.name_and_type_index = swap_u2(entry->info.invoke_dynamic_info.name_and_type_index);
                break; 
            case CONSTANT_Integer:
                fread(&entry->info.integer_info.bytes, sizeof(u4), 1, file);
                entry->info.integer_info.bytes = swap_u4(entry->info.integer_info.bytes);
                break;
            case CONSTANT_Float:
                fread(&entry->info.float_info.bytes, sizeof(u4), 1, file);
                // Não precisa swap aqui, será feito na função get_float_from_bytes
                break;
            case CONSTANT_Long:
                fread(&entry->info.long_info.high_bytes, sizeof(u4), 1, file);
                entry->info.long_info.high_bytes = swap_u4(entry->info.long_info.high_bytes);
                fread(&entry->info.long_info.low_bytes, sizeof(u4), 1, file);
                entry->info.long_info.low_bytes = swap_u4(entry->info.long_info.low_bytes);
                i++;
                if (i < count) constant_pool[i] = NULL;
                break;
            case CONSTANT_Double:
                fread(&entry->info.double_info.high_bytes, sizeof(u4), 1, file);
                entry->info.double_info.high_bytes = swap_u4(entry->info.double_info.high_bytes);
                fread(&entry->info.double_info.low_bytes, sizeof(u4), 1, file);
                entry->info.double_info.low_bytes = swap_u4(entry->info.double_info.low_bytes);
                i++;
                 if (i < count) constant_pool[i] = NULL;
                break;
            default:
                fprintf(stderr, "Tag invalida ou nao suportada: %u no indice %d\n", entry->tag, i);
                exit(1);
        }
    }
}

// Função para obter Utf8
char* get_utf8_from_pool(u2 index, cp_info** constant_pool, u2 cp_count) {
    if (!constant_pool) return "<invalid_pool>";
    if (index == 0 || index >= cp_count) return "<invalid_index>";
    if (!constant_pool[index]) return "<null_entry>";
    if (constant_pool[index]->tag != CONSTANT_Utf8) return "<not_utf8>";
    
    return (char*) constant_pool[index]->info.utf8_info.bytes;
}

// Função de print
void print_constant_pool(u2 count, cp_info** constant_pool) {
    printf("\n---- Constant Pool ----\n");
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
                printf("Class: indice=#%u <%s>\n", entry->info.class_info.name_index, get_utf8_from_pool(entry->info.class_info.name_index, constant_pool, count));
                break;
            case CONSTANT_String:
                printf("String: indice=#%u <%s>\n", entry->info.string_info.string_index, get_utf8_from_pool(entry->info.string_info.string_index, constant_pool, count));
                break;
            case CONSTANT_Fieldref:
                 // Verifica se os índices são válidos antes de desreferenciar
                 if (entry->info.fieldref_info.class_index < count && constant_pool[entry->info.fieldref_info.class_index] && constant_pool[entry->info.fieldref_info.class_index]->tag == CONSTANT_Class) {
                    printf("Fieldref: class_indice=#%u <%s>, name_and_type_indice=#%u\n",
                           entry->info.fieldref_info.class_index,
                           get_utf8_from_pool(constant_pool[entry->info.fieldref_info.class_index]->info.class_info.name_index, constant_pool, count),
                           entry->info.fieldref_info.name_and_type_index);
                 } else {
                    printf("Fieldref: class_indice=#%u <invalid>, name_and_type_indice=#%u\n", entry->info.fieldref_info.class_index, entry->info.fieldref_info.name_and_type_index);
                 }
                break;
            case CONSTANT_Methodref:
                 if (entry->info.methodref_info.class_index < count && constant_pool[entry->info.methodref_info.class_index] && constant_pool[entry->info.methodref_info.class_index]->tag == CONSTANT_Class) {
                    printf("Methodref: class_indice=#%u <%s>, name_and_type_indice=#%u\n",
                           entry->info.methodref_info.class_index,
                           get_utf8_from_pool(constant_pool[entry->info.methodref_info.class_index]->info.class_info.name_index, constant_pool, count),
                           entry->info.methodref_info.name_and_type_index);
                 } else {
                     printf("Methodref: class_indice=#%u <invalid>, name_and_type_indice=#%u\n", entry->info.methodref_info.class_index, entry->info.methodref_info.name_and_type_index);
                 }
                break;
            case CONSTANT_InterfaceMethodref:
                  if (entry->info.interface_methodref_info.class_index < count && constant_pool[entry->info.interface_methodref_info.class_index] && constant_pool[entry->info.interface_methodref_info.class_index]->tag == CONSTANT_Class) {
                     printf("InterfaceMethodref: class_indice=#%u <%s>, name_and_type_indice=#%u\n",
                           entry->info.interface_methodref_info.class_index,
                           get_utf8_from_pool(constant_pool[entry->info.interface_methodref_info.class_index]->info.class_info.name_index, constant_pool, count),
                           entry->info.interface_methodref_info.name_and_type_index);
                  } else {
                      printf("InterfaceMethodref: class_indice=#%u <invalid>, name_and_type_indice=#%u\n", entry->info.interface_methodref_info.class_index, entry->info.interface_methodref_info.name_and_type_index);
                  }
                break;
            case CONSTANT_NameAndType:
                printf("NameAndType: name_indice=#%u <%s>, descriptor_indice=#%u <%s>\n",
                    entry->info.name_and_type_info.name_index, 
                    get_utf8_from_pool(entry->info.name_and_type_info.name_index, constant_pool, count), // Passe 'count'
                    entry->info.name_and_type_info.descriptor_index, 
                    get_utf8_from_pool(entry->info.name_and_type_info.descriptor_index, constant_pool, count)); // Passe 'count'
                break;
            case CONSTANT_Integer:
                printf("Integer: %d\n", (int32_t)entry->info.integer_info.bytes);
                break;
            case CONSTANT_Float:
                 printf("Float: %f\n", get_float_from_bytes(entry->info.float_info.bytes)); // Chama a função de exibidor.c
                 break;
            case CONSTANT_Long: ;
                 int64_t long_val = ((int64_t)entry->info.long_info.high_bytes << 32) | entry->info.long_info.low_bytes;
                 printf("Long: %" PRId64 "\n", long_val);
                 i++; // Incrementa aqui para o print
                 break;
            case CONSTANT_Double: ;
                 double double_val = get_double_from_bytes(entry->info.double_info.high_bytes, entry->info.double_info.low_bytes); // Chama a função de exibidor.c
                 printf("Double: %lf\n", double_val);
                 i++; // Incrementa aqui para o print
                 break;
            default:
                printf("Tag Desconhecida: %u\n", entry->tag);
                break;
        }
    }
}


// Função free
void free_constant_pool(u2 count, cp_info** constant_pool) {
    if (!constant_pool) return;
    for (int i = 1; i < count; i++) {
        cp_info* entry = constant_pool[i];
        if (entry) {
            if (entry->tag == CONSTANT_Utf8 && entry->info.utf8_info.bytes) {
                free(entry->info.utf8_info.bytes);
            }
            free(entry);
        }
    }
    free(constant_pool);
}