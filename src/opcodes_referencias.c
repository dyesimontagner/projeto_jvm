#include "opcodes.h"
#include "engine.h"
#include <stdio.h>
#include <string.h>

void ldc_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    if (!frame || !frame->class_file) return;
    
    u1 index = frame->code[frame->pc++];
    
    if (index < 1 || index >= frame->class_file->constant_pool_count) {
        fprintf(stderr, "[ldc] Invalid constant pool index: %d\n", index);
        return;
    }
    
    cp_info* cp_entry = frame->class_file->constant_pool[index];
    if (!cp_entry) return;
    
    // Suporta apenas inteiros simples por enquanto
    if (cp_entry->tag == CONSTANT_Integer) {
        int32_t val = cp_entry->info.integer_info.bytes;
        operand_stack_push_int(&frame->operand_stack, val);
        printf("[ldc] Loaded int constant: %d\n", val);
    } else if (cp_entry->tag == CONSTANT_String) {
        // Strings são referências para UTF8 no constant pool
        u2 string_idx = cp_entry->info.string_info.string_index;
        printf("[ldc] Loaded string reference: index %d\n", string_idx);
        // Simplificado: apenas empurha o índice como referência
        operand_stack_push_reference(&frame->operand_stack, (void*)(uintptr_t)string_idx);
    } else {
        fprintf(stderr, "[ldc] Unsupported constant type: %d\n", cp_entry->tag);
    }
}

void ldc_w_op(JVM* jvm, Frame* frame) {
    (void)jvm;
    if (!frame || !frame->class_file) return;
    
    // ldc_w usa 2 bytes para índice (wide)
    u1 byte1 = frame->code[frame->pc++];
    u1 byte2 = frame->code[frame->pc++];
    u2 index = (u2)((byte1 << 8) | byte2);
    
    if (index < 1 || index >= frame->class_file->constant_pool_count) {
        fprintf(stderr, "[ldc_w] Invalid constant pool index: %d\n", index);
        return;
    }
    
    cp_info* cp_entry = frame->class_file->constant_pool[index];
    if (!cp_entry) return;
    
    // Mesmo comportamento de ldc mas com índice de 2 bytes
    if (cp_entry->tag == CONSTANT_Integer) {
        int32_t val = cp_entry->info.integer_info.bytes;
        operand_stack_push_int(&frame->operand_stack, val);
        printf("[ldc_w] Loaded int constant: %d\n", val);
    } else if (cp_entry->tag == CONSTANT_String) {
        u2 string_idx = cp_entry->info.string_info.string_index;
        printf("[ldc_w] Loaded string reference: index %d\n", string_idx);
        operand_stack_push_reference(&frame->operand_stack, (void*)(uintptr_t)string_idx);
    } else {
        fprintf(stderr, "[ldc_w] Unsupported constant type: %d\n", cp_entry->tag);
    }
}
