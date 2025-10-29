#ifndef EXIBIDOR_H
#define EXIBIDOR_H

#include "common.h"
#include "leitor_class.h" // Precisa das definições das estruturas (ClassFile, etc.)
#include "constant_pool.h"

// --- DECLARAÇÕES DAS FUNÇÕES DE EXIBIÇÃO ---

// Função principal de exibição
void print_class_file_info(ClassFile* class_file);

// Funções auxiliares de exibição
void print_attributes(u2 count, attribute_info* attributes, cp_info** cp, u2 cp_count, const char* indent);
void print_source_file_attribute(const SourceFile_attribute* attr, cp_info** cp, u2 cp_count);
void print_line_number_table_attribute(const LineNumberTable_attribute* attr);
void print_instructions(u1* code, u4 code_length, cp_info** cp, u2 cp_count, const char* indent);
void print_constant_pool(u2 count, cp_info** constant_pool); // Mover declaração de constant_pool.h para cá? Ou deixar lá? Melhor deixar em constant_pool.h por enquanto.

// Funções utilitárias de exibição
const char* get_access_flags_string(u2 flags, FlagType type);
float get_float_from_bytes(u4 bytes);
double get_double_from_bytes(u4 high_bytes, u4 low_bytes);

#endif // EXIBIDOR_H