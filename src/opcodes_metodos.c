#include "opcodes.h"
#include "jvm.h"
#include "frame.h"
#include "engine.h"       // Para read_u1, read_s2
#include "leitor_class.h" // Para constantes de tag
#include "constant_pool.h" // Para get_utf8_from_pool
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// AUXILIARES
// ============================================================================

// Conta quantos slots de parâmetros um método consome da pilha baseado no descritor
static int count_parameters(const char* descriptor) {
    int count = 0;
    const char* p = descriptor;
    
    if (*p != '(') return 0; // Descritor inválido
    p++;

    while (*p != ')') {
        if (*p == 'L') { // Referência
            count++;
            while (*p != ';') p++;
        }
        else if (*p == '[') { // Array
            count++;
            while (*p == '[') p++;
            if (*p == 'L') while (*p != ';') p++;
        }
        else if (*p == 'D' || *p == 'J') { // Double ou Long (64 bits)
            count += 2;
        }
        else { // I, F, B, C, S, Z (32 bits)
            count++;
        }
        p++;
    }
    return count;
}

// ============================================================================
// CONSTANTES (LDC)
// ============================================================================

// LDC para String, Int e Float. (Long/Double usam ldc2_w em outro arquivo)
void ldc_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    u1 index = read_u1(frame);
    
    if (index >= frame->class_file->constant_pool_count) {
        fprintf(stderr, "Erro: Índice LDC inválido (%d)\n", index);
        exit(1);
    }

    cp_info* cp = frame->class_file->constant_pool[index];

    if (cp->tag == CONSTANT_String) {
        u2 string_index = cp->info.string_info.string_index;
        char* str_val = (char*)get_utf8_from_pool(
            string_index, 
            frame->class_file->constant_pool, 
            frame->class_file->constant_pool_count
        );
        operand_stack_push_reference(&frame->operand_stack, str_val);
    }
    else if (cp->tag == CONSTANT_Integer) {
        operand_stack_push_int(&frame->operand_stack, cp->info.integer_info.bytes);
    }
    else if (cp->tag == CONSTANT_Float) {
        u4 bytes = cp->info.float_info.bytes;
        float val;
        memcpy(&val, &bytes, sizeof(u4));
        operand_stack_push_float(&frame->operand_stack, val);
    }
}

// ============================================================================
// INVOKES E CAMPOS
// ============================================================================

void getstatic_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    read_s2(frame); // Lê índice (ignoramos pois simulamos System.out)
    // Empilha uma referência "fictícia" para System.out.
    // Usamos NULL ou um ponteiro marcador, pois o invokevirtual vai apenas descartá-lo.
    operand_stack_push_reference(&frame->operand_stack, (void*)0xDEADBEEF); 
}

void invokevirtual_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    read_s2(frame); // Lê índice do método

    if (frame->operand_stack.top < 0) return;

    // Verifica o tipo do elemento no topo da pilha para decidir como imprimir e desempilhar
    StackElement top_el = frame->operand_stack.elements[frame->operand_stack.top];
    
    // NOTA: println(tipo) consome o argumento (Valor) + o objeto (Referência)
    
    if (top_el.type == TYPE_DOUBLE) {
        double val = operand_stack_pop_double(&frame->operand_stack); // Desempilha valor (2 slots)
        operand_stack_pop_reference(&frame->operand_stack);           // Desempilha objeto (1 slot)
        printf("%.6f\n", val);
    }
    else if (top_el.type == TYPE_LONG) {
        int64_t val = operand_stack_pop_long(&frame->operand_stack);
        operand_stack_pop_reference(&frame->operand_stack);
        printf("%ld\n", val);
    }
    else if (top_el.type == TYPE_REFERENCE) {
        char* str = (char*)operand_stack_pop_reference(&frame->operand_stack);
        operand_stack_pop_reference(&frame->operand_stack);
        printf("%s\n", str ? str : "null");
    } 
    else if (top_el.type == TYPE_INT) {
        int32_t val = operand_stack_pop_int(&frame->operand_stack);
        operand_stack_pop_reference(&frame->operand_stack);
        printf("%d\n", val);
    }
    else if (top_el.type == TYPE_FLOAT) {
        float val = operand_stack_pop_float(&frame->operand_stack);
        operand_stack_pop_reference(&frame->operand_stack);
        printf("%f\n", val);
    }
    else {
        // Fallback genérico para evitar loop infinito na pilha
        if (frame->operand_stack.top >= 0) frame->operand_stack.top--;
        operand_stack_pop_reference(&frame->operand_stack); 
    }
}

// --- IMPLEMENTAÇÃO DE INVOKESTATIC ---
void invokestatic_op(JVM* jvm, Frame* frame) {
    u2 index = read_s2(frame); 

    cp_info* cp = frame->class_file->constant_pool[index];
    if (cp->tag != CONSTANT_Methodref) {
        return; 
    }

    // 1. Resolver Nome da Classe
    u2 class_idx = cp->info.methodref_info.class_index;
    u2 name_and_type_idx = cp->info.methodref_info.name_and_type_index;

    cp_info* class_info = frame->class_file->constant_pool[class_idx];
    u2 name_idx = class_info->info.class_info.name_index;
    char* class_name = (char*)get_utf8_from_pool(name_idx, frame->class_file->constant_pool, frame->class_file->constant_pool_count);

    // 2. Resolver Nome do Método e Descritor
    cp_info* name_type = frame->class_file->constant_pool[name_and_type_idx];
    u2 m_name_idx = name_type->info.name_and_type_info.name_index;
    u2 m_desc_idx = name_type->info.name_and_type_info.descriptor_index;
    
    char* method_name = (char*)get_utf8_from_pool(m_name_idx, frame->class_file->constant_pool, frame->class_file->constant_pool_count);
    char* method_desc = (char*)get_utf8_from_pool(m_desc_idx, frame->class_file->constant_pool, frame->class_file->constant_pool_count);

    // 3. Carregar a Classe e encontrar o método
    if (!jvm_load_class(jvm, class_name)) {
        fprintf(stderr, "Erro: Falha ao carregar classe %s\n", class_name);
        exit(1);
    }
    ClassFile* method_class = jvm_find_class(jvm, class_name);
    method_info* method = jvm_find_method(method_class, method_name, method_desc);

    if (!method) {
        fprintf(stderr, "Erro: Metodo estatico nao encontrado: %s.%s\n", class_name, method_name);
        exit(1);
    }

    // 4. Preparar novo Frame
    Frame* new_frame = frame_create(method_class, method);
    
    // 5. Transferir Argumentos
    int num_args_slots = count_parameters(method_desc);
    
    for (int i = num_args_slots - 1; i >= 0; i--) {
        StackElement val = frame->operand_stack.elements[frame->operand_stack.top];
        frame->operand_stack.top--; 
        new_frame->local_vars.variables[i] = val;
    }

    // 6. Empilhar novo frame
    frame_stack_push(&jvm->frame_stack, new_frame);
}

void invokespecial_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    read_s2(frame);
    // Em uma implementação completa, invokespecial (para construtores ou super)
    // também consome parâmetros e a referência do objeto ('this').
    // Para este projeto simples, assumimos que init consome apenas 'this'.
    if (frame->operand_stack.top >= 0) {
        operand_stack_pop_reference(&frame->operand_stack); // Pop 'this'
    }
}

// ============================================================================
// INSTRUÇÕES DE RETORNO
// ============================================================================

void return_op(JVM* jvm, Frame* frame) {
    (void)frame;
    Frame* popped = frame_stack_pop(&jvm->frame_stack);
    frame_destroy(popped);
}

void ireturn_op(JVM* jvm, Frame* frame) {
    int32_t ret_value = operand_stack_pop_int(&frame->operand_stack);
    
    Frame* popped = frame_stack_pop(&jvm->frame_stack);
    frame_destroy(popped);
    
    Frame* caller = jvm->frame_stack.top;
    if (caller) {
        operand_stack_push_int(&caller->operand_stack, ret_value);
    } else {
        printf("Resultado final (ireturn): %d\n", ret_value);
        jvm->exit_code = ret_value;
    }
}
