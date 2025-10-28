#include "exibidor.h"
#include "opcodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stddef.h>

// --- Funções Auxiliares de Conversão ---
float get_float_from_bytes(u4 bytes) {
    float f;
    u4 swapped_bytes = swap_u4(bytes);
    memcpy(&f, &swapped_bytes, sizeof(float));
    return f;
}

double get_double_from_bytes(u4 high_bytes, u4 low_bytes) {
    double d;
    uint64_t val = ((uint64_t)high_bytes << 32) | low_bytes;
    uint64_t swapped_val = 0;
    const u1* p_val = (u1*)&val;
    u1* p_swapped = (u1*)&swapped_val;
    for(int i=0; i<8; ++i) {
        p_swapped[i] = p_val[7-i];
    }
    memcpy(&d, &swapped_val, sizeof(double));
    return d;
}
// --- Fim Funções Auxiliares ---

// Exibe Access Flags
const char* get_access_flags_string(u2 flags, FlagType type) {
    char* flags_str = (char*) malloc(256 * sizeof(char));
    if (!flags_str) return "Erro de alocacao";
    flags_str[0] = '\0';

    if (flags & ACC_PUBLIC) strcat(flags_str, "PUBLIC ");
    if (flags & ACC_PRIVATE) strcat(flags_str, "PRIVATE ");
    if (flags & ACC_PROTECTED) strcat(flags_str, "PROTECTED ");
    if (flags & ACC_STATIC) strcat(flags_str, "STATIC ");
    if (flags & ACC_FINAL) strcat(flags_str, "FINAL ");

    if (type == TYPE_CLASS) {
        if (flags & ACC_SUPER) strcat(flags_str, "SUPER ");
        if (flags & ACC_INTERFACE) strcat(flags_str, "INTERFACE ");
        if (flags & ACC_ABSTRACT) strcat(flags_str, "ABSTRACT ");
        if (flags & ACC_ANNOTATION) strcat(flags_str, "ANNOTATION ");
    } else if (type == TYPE_FIELD) {
        if (flags & ACC_VOLATILE) strcat(flags_str, "VOLATILE ");
        if (flags & ACC_TRANSIENT) strcat(flags_str, "TRANSIENT ");
    } else if (type == TYPE_METHOD) {
        if (flags & ACC_SYNCHRONIZED) strcat(flags_str, "SYNCHRONIZED ");
        if (flags & ACC_BRIDGE) strcat(flags_str, "BRIDGE ");
        if (flags & ACC_VARARGS) strcat(flags_str, "VARARGS ");
        if (flags & ACC_NATIVE) strcat(flags_str, "NATIVE ");
        if (flags & ACC_ABSTRACT) strcat(flags_str, "ABSTRACT ");
        if (flags & ACC_STRICT) strcat(flags_str, "STRICT ");
    }

    if (flags & ACC_SYNTHETIC) strcat(flags_str, "SYNTHETIC ");
    if (flags & ACC_ENUM) strcat(flags_str, "ENUM ");

    size_t len = strlen(flags_str);
    if (len > 0 && flags_str[len - 1] == ' ') {
        flags_str[len - 1] = '\0';
    }

    return flags_str;
}

// Exibe Bytecode
void print_instructions(u1* code, u4 code_length, cp_info** cp, const char* indent) {
    u4 pc = 0;
    u2 cp_count = 0;
     if (cp) {
         const ClassFile* cf_ptr = (ClassFile*)((char*)cp - offsetof(ClassFile, constant_pool));
         if (cf_ptr) cp_count = cf_ptr->constant_pool_count;
     }

    printf("%sBytecode:\n", indent);
    while (pc < code_length) {
        u4 start_pc = pc;
        u1 opcode_byte = code[pc];
        const InstructionInfo* info = &instruction_table[opcode_byte];
        printf("%s  %4u: %s", indent, start_pc, info->mnemonic ? info->mnemonic : "unknown_opcode");

        pc++;

        if (info->num_operands > 0) {
            if (opcode_byte == bipush || opcode_byte == newarray || opcode_byte == ret) {
                printf(" %d", (int8_t)code[pc]);
                pc += 1;
            } else if (opcode_byte == iinc) {
                 printf(" %u, %d", code[pc], (int8_t)code[pc + 1]);
                 pc += 2;
            } else if (opcode_byte == sipush || info->num_operands == 2) {
                if (pc + 1 >= code_length) { printf(" (erro: fim inesperado do código)\n"); break; }
                u2 index = ((u2)code[pc] << 8) | code[pc + 1];
                printf(" #%u", index);
                if (cp && index > 0 && index < cp_count) {
                    // *** CORREÇÃO PARÊNTESES ***
                    if (((opcode_byte >= ldc_w && opcode_byte <= putfield) ||
                         (opcode_byte >= invokespecial && opcode_byte <= invokestatic) ||
                         opcode_byte == new_ || opcode_byte == anewarray || opcode_byte == checkcast || opcode_byte == instanceof_) ) {
                         if(cp[index] && cp[index]->tag == CONSTANT_Class)
                             printf(" // class %s", get_utf8_from_pool(cp[index]->info.class_info.name_index, cp));
                         else if (cp[index] && cp[index]->tag == CONSTANT_String)
                             printf(" // String \"%s\"", get_utf8_from_pool(cp[index]->info.string_info.string_index, cp));
                         else if (cp[index] && (cp[index]->tag == CONSTANT_Fieldref || cp[index]->tag == CONSTANT_Methodref || cp[index]->tag == CONSTANT_InterfaceMethodref))
                             printf(" // ref");
                         else if (cp[index] && cp[index]->tag == CONSTANT_NameAndType)
                              printf(" // NameAndType");
                    // *** CORREÇÃO PARÊNTESES ***
                    } else if(((opcode_byte >= ifeq && opcode_byte <= jsr) || opcode_byte == ifnull || opcode_byte == ifnonnull) ) {
                         printf(" (%d)", (int)(start_pc + (int16_t)index));
                    }
                } else if((opcode_byte >= ifeq && opcode_byte <= jsr) || opcode_byte == ifnull || opcode_byte == ifnonnull ) {
                     printf(" (%d)", (int)(start_pc + (int16_t)index));
                }
                pc += 2;
             } else if(opcode_byte == multianewarray){
                  if (pc + 2 >= code_length) { printf(" (erro: fim inesperado do código)\n"); break; }
                  u2 index = ((u2)code[pc] << 8) | code[pc + 1];
                  u1 dim = code[pc+2];
                  printf(" #%u dim %u", index, dim);
                  if (cp && index > 0 && index < cp_count && cp[index] && cp[index]->tag == CONSTANT_Class)
                       printf(" // class %s", get_utf8_from_pool(cp[index]->info.class_info.name_index, cp));
                  pc += 3;
             } else if (opcode_byte == invokeinterface || opcode_byte == invokedynamic || opcode_byte == goto_w || opcode_byte == jsr_w) {
                 if (pc + 3 >= code_length) { printf(" (erro: fim inesperado do código)\n"); break; }
                 if (opcode_byte == goto_w || opcode_byte == jsr_w) {
                     int32_t offset = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                     printf(" (%d)", (int)(start_pc + offset));
                 } else {
                     u2 index = ((u2)code[pc] << 8) | code[pc + 1];
                     u1 count_or_zero1 = code[pc+2];
                     // u1 zero = code[pc+3]; // *** VARIÁVEL REMOVIDA ***
                     printf(" #%u", index);
                     if (opcode_byte == invokeinterface) printf(" count %u", count_or_zero1);
                      if (cp && index > 0 && index < cp_count && cp[index]) printf(" // ref");
                 }
                 pc += 4;
            } else {
                 if (pc + info->num_operands > code_length) { printf(" (erro: fim inesperado do código)\n"); break; }
                 printf(" (operandos:");
                 for(int k=0; k < info->num_operands; ++k) printf(" %.2x", code[pc+k]);
                 printf(")");
                 pc += info->num_operands;
            }
        } else if (info->num_operands == -1) { // tableswitch, lookupswitch, wide
             int padding = (4 - (pc % 4)) % 4;
             if (pc + padding >= code_length) { printf(" (erro: fim inesperado do código no padding)\n"); break; }
             pc += padding;

             if (opcode_byte == tableswitch || opcode_byte == lookupswitch) {
                 if (pc + 3 >= code_length) { printf(" (erro: fim inesperado do código no default offset)\n"); break; }
                 int32_t default_offset = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                 pc += 4;
                 printf("\n%s     default -> %d", indent, (int)(start_pc + default_offset));

                 if (opcode_byte == tableswitch) {
                     if (pc + 7 >= code_length) { printf(" (erro: fim inesperado do código em low/high bytes)\n"); break; }
                     int32_t low = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                     pc += 4;
                     int32_t high = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                     pc += 4;
                     printf(" low: %d, high: %d", low, high);
                     int32_t num_offsets = high - low + 1;
                      if (num_offsets < 0 || pc + (num_offsets * 4) > code_length) { printf(" (erro: contagem de offsets inválida ou fim inesperado do código)\n"); break;}
                     for (int32_t i = 0; i < num_offsets; i++) {
                         int32_t offset = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                         pc += 4;
                         printf("\n%s     %d -> %d", indent, low + i, (int)(start_pc + offset));
                     }
                 } else { // lookupswitch
                     if (pc + 3 >= code_length) { printf(" (erro: fim inesperado do código em npairs)\n"); break; }
                     int32_t npairs = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                     pc += 4;
                     printf(" npairs: %d", npairs);
                     if (npairs < 0 || pc + (npairs * 8) > code_length) { printf(" (erro: npairs inválido ou fim inesperado do código)\n"); break;}
                     for (int32_t i = 0; i < npairs; i++) {
                         int32_t match = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                         pc += 4;
                         int32_t offset = ((int32_t)code[pc] << 24) | ((int32_t)code[pc+1] << 16) | ((int32_t)code[pc+2] << 8) | code[pc+3];
                         pc += 4;
                         printf("\n%s     %d -> %d", indent, match, (int)(start_pc + offset));
                     }
                 }
            } else if (opcode_byte == wide) {
                 if (pc >= code_length) { printf(" (erro: fim inesperado do código após wide)\n"); break; }
                 u1 next_opcode = code[pc];
                 pc++;
                 if (pc + 1 >= code_length) { printf(" (erro: fim inesperado do código no índice wide)\n"); break; }
                 u2 index = ((u2)code[pc] << 8) | code[pc + 1];
                 pc += 2;
                 // Verifica se next_opcode é válido antes de aceder à tabela
                 const char* next_mnemonic = (next_opcode < 256 && instruction_table[next_opcode].mnemonic) ? instruction_table[next_opcode].mnemonic : "unknown_wide_opcode";
                 printf(" %s %u", next_mnemonic, index);
                 if (next_opcode == iinc) {
                     if (pc + 1 >= code_length) { printf(" (erro: fim inesperado do código na constante wide iinc)\n"); break; }
                     int16_t const_val = ((int16_t)code[pc] << 8) | code[pc + 1];
                     pc += 2;
                     printf(", %d", const_val);
                 }
            } else {
                 printf(" (tratamento especial não implementado para opcode 0x%x)", opcode_byte);
                 if (pc < code_length) pc++; else break;
            }
        }
        printf("\n");
    }
}


// Exibe Atributos Específicos
void print_source_file_attribute(const SourceFile_attribute* attr, cp_info** cp) {
    if (!attr) return;
    printf("       SourceFile: cp_info #%u <%s>\n", attr->sourcefile_index, get_utf8_from_pool(attr->sourcefile_index, cp));
}

void print_line_number_table_attribute(const LineNumberTable_attribute* attr) {
    if (!attr || attr->line_number_table_length == 0) return;
    printf("       LineNumberTable:\n");
    printf("         line\tpc\n");
    for (int i = 0; i < attr->line_number_table_length; i++) {
        printf("         %u\t%u\n", attr->table[i].line_number, attr->table[i].start_pc);
    }
}

// Função recursiva para exibir atributos
void print_attributes(u2 count, attribute_info* attributes, cp_info** cp, const char* indent) {
     if (!attributes || count == 0) return;

     u2 cp_count = 0;
     const ClassFile* cf_ptr = NULL;
     if (cp) {
         cf_ptr = (ClassFile*)((char*)cp - offsetof(ClassFile, constant_pool));
         if (cf_ptr) cp_count = cf_ptr->constant_pool_count;
     }

     for (int i = 0; i < count; i++) {
         attribute_info* attr = &attributes[i];
         const char* attr_name = "<invalid_name_index>";
         if (cp && attr->attribute_name_index > 0 && attr->attribute_name_index < cp_count && cp[attr->attribute_name_index]) {
            attr_name = get_utf8_from_pool(attr->attribute_name_index, cp);
         }
         printf("%sAttribute: %s (length: %u)\n", indent, attr_name, attr->attribute_length);

         if (strcmp(attr_name, "Code") == 0 && attr->attr_info.code_info) {
             const Code_attribute* code = attr->attr_info.code_info;
             char next_indent[256];
             snprintf(next_indent, sizeof(next_indent), "%s  ", indent);

             printf("%sCode:\n", next_indent);
             printf("%s  max_stack=%u, max_locals=%u, code_length=%u\n", next_indent, code->max_stack, code->max_locals, code->code_length);

             if (code->code && code->code_length > 0) {
                 print_instructions(code->code, code->code_length, cp, next_indent);
             }

             if (code->exception_table_length > 0 && code->exception_table) {
                 printf("%s  Exception table:\n", next_indent);
                 printf("%s   from\tto\ttarget\ttype\n", next_indent);
                 for (int k = 0; k < code->exception_table_length; k++) {
                     printf("%s   %u\t%u\t%u\t", next_indent,
                            code->exception_table[k].start_pc,
                            code->exception_table[k].end_pc,
                            code->exception_table[k].handler_pc);
                     if (code->exception_table[k].catch_type == 0) {
                         printf("any\n");
                     } else if (cp  && code->exception_table[k].catch_type < cp_count && cp[code->exception_table[k].catch_type] && cp[code->exception_table[k].catch_type]->tag == CONSTANT_Class) {
                         printf("Class cp_info #%u <%s>\n", code->exception_table[k].catch_type,
                                get_utf8_from_pool(cp[code->exception_table[k].catch_type]->info.class_info.name_index, cp));
                     } else {
                         printf("cp_info #%u <invalid index or type>\n", code->exception_table[k].catch_type);
                     }
                 }
             }

             if (code->attributes_count > 0 && code->attributes) {
                 printf("%s  Attributes:\n", next_indent);
                 snprintf(next_indent, sizeof(next_indent), "%s    ", indent);
                 print_attributes(code->attributes_count, code->attributes, cp, next_indent);
             }
         } else if (strcmp(attr_name, "SourceFile") == 0 && attr->attr_info.source_file_info) {
             print_source_file_attribute(attr->attr_info.source_file_info, cp);
         } else if (strcmp(attr_name, "LineNumberTable") == 0 && attr->attr_info.line_number_table_info) {
             print_line_number_table_attribute(attr->attr_info.line_number_table_info);
         }
         // Adicionar 'else if' para outros atributos
         else {
             printf("%s  Attribute data (first few bytes): ", indent);
             // *** CORREÇÃO SIGN-COMPARE ***
             int bytes_to_show = attr->attribute_length < 16 ? (int)attr->attribute_length : 16;
             for(int k=0; k < bytes_to_show; ++k) {
                 if(attr->attr_info.info) {
                   printf("%.2x ", attr->attr_info.info[k]);
                 }
             }
             if (attr->attribute_length > (u4)bytes_to_show) printf("..."); // Cast aqui
             printf("\n");
         }
     }
}

// Função principal de exibição
void print_class_file_info(ClassFile* class_file) {
    if (!class_file) {
        printf("Erro: Estrutura ClassFile nula.\n");
        return;
    }
    printf("---- General Information ----\n");
    printf("Magic: 0x%X\n", class_file->magic);
    printf("Minor version: %u\n", class_file->minor_version);
    printf("Major version: %u (Java %ld)\n", class_file->major_version, (long)class_file->major_version - 44);
    printf("Constant pool count: %u\n", class_file->constant_pool_count);

    const char* class_flags_str = get_access_flags_string(class_file->access_flags, TYPE_CLASS);
    printf("Access flags: 0x%.4X [%s]\n", class_file->access_flags, class_flags_str);
    free((void*)class_flags_str);

    u2 cp_size = class_file->constant_pool_count;
    cp_info** cp = class_file->constant_pool;

    if (cp && class_file->this_class > 0 && class_file->this_class < cp_size && cp[class_file->this_class] && cp[class_file->this_class]->tag == CONSTANT_Class) {
         printf("This class: cp_info #%u <%s>\n", class_file->this_class, get_utf8_from_pool(cp[class_file->this_class]->info.class_info.name_index, cp));
    } else {
         printf("This class: cp_info #%u <invalid index or pool>\n", class_file->this_class);
    }
    if (class_file->super_class == 0) {
         printf("Super class: cp_info #0 <N/A>\n");
    } else if (cp && class_file->super_class < cp_size && cp[class_file->super_class] && cp[class_file->super_class]->tag == CONSTANT_Class) {
        printf("Super class: cp_info #%u <%s>\n", class_file->super_class, get_utf8_from_pool(cp[class_file->super_class]->info.class_info.name_index, cp));
    } else {
        printf("Super class: cp_info #%u <invalid index or pool>\n", class_file->super_class);
    }

    printf("Interfaces count: %u\n", class_file->interfaces_count);
    if (class_file->interfaces_count > 0 && class_file->interfaces && cp) {
        printf("---- Interfaces ----\n");
        for (int i = 0; i < class_file->interfaces_count; i++) {
            u2 if_index = class_file->interfaces[i];
             if (if_index > 0 && if_index < cp_size && cp[if_index] && cp[if_index]->tag == CONSTANT_Class) {
                  printf("[%d] Interface: cp_info #%u <%s>\n", i, if_index, get_utf8_from_pool(cp[if_index]->info.class_info.name_index, cp));
             } else {
                  printf("[%d] Interface: cp_info #%u <invalid index or pool>\n", i, if_index);
             }
        }
    }

    printf("Fields count: %u\n", class_file->fields_count);
    printf("Methods count: %u\n", class_file->methods_count);
    printf("Attributes count: %u\n", class_file->attributes_count);

    if (cp && cp_size > 1) {
        // A função print_constant_pool está em constant_pool.c, precisa ser chamada
        print_constant_pool(cp_size, cp);
    }

    if (class_file->fields_count > 0 && class_file->fields && cp) {
        printf("\n---- Fields ----\n");
        for (int i = 0; i < class_file->fields_count; i++) {
             field_info* field = &class_file->fields[i];
             printf("[%d] Name: cp_info #%u <%s>\n", i, field->name_index, get_utf8_from_pool(field->name_index, cp));
             printf("     Descriptor: cp_info #%u <%s>\n", field->descriptor_index, get_utf8_from_pool(field->descriptor_index, cp));
             const char* field_flags_str = get_access_flags_string(field->access_flags, TYPE_FIELD);
             printf("     Access flags: 0x%.4X [%s]\n", field->access_flags, field_flags_str);
             free((void*)field_flags_str);
             printf("     Attributes count: %u\n", field->attributes_count);
             if (field->attributes_count > 0 && field->attributes) {
                  print_attributes(field->attributes_count, field->attributes, cp, "     ");
             }
        }
    }

     if (class_file->methods_count > 0 && class_file->methods && cp) {
         printf("\n---- Methods ----\n");
         for (int i = 0; i < class_file->methods_count; i++) {
             method_info* method = &class_file->methods[i];
             printf("[%d] Name: cp_info #%u <%s>\n", i, method->name_index, get_utf8_from_pool(method->name_index, cp));
             printf("     Descriptor: cp_info #%u <%s>\n", method->descriptor_index, get_utf8_from_pool(method->descriptor_index, cp));
             const char* method_flags_str = get_access_flags_string(method->access_flags, TYPE_METHOD);
             printf("     Access flags: 0x%.4X [%s]\n", method->access_flags, method_flags_str);
             free((void*)method_flags_str);
             printf("     Attributes count: %u\n", method->attributes_count);
             if (method->attributes_count > 0 && method->attributes) {
                  print_attributes(method->attributes_count, method->attributes, cp, "     ");
             }
         }
     }

     if (class_file->attributes_count > 0 && class_file->attributes && cp) {
        printf("\n---- Class Attributes ----\n");
        print_attributes(class_file->attributes_count, class_file->attributes, cp, "");
     }
}
// --- FIM DAS FUNÇÕES DE EXIBIÇÃO ---