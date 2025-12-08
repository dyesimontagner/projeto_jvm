#include "opcodes.h"
#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

void new_op(JVM* jvm, Frame* frame) {
    if (!jvm || !frame || !frame->class_file) return;
    
    // Ler índice do constant pool (class reference)
    u1 byte1 = frame->code[frame->pc++];
    u1 byte2 = frame->code[frame->pc++];
    u2 class_idx = (u2)((byte1 << 8) | byte2);
    
    printf("[new] Creating object from class reference #%d\n", class_idx);
    
    // Simplificação: aloca uma estrutura mínima na heap
    // Uma implementação completa resolveria o classe no constant pool
    // e criaria uma instância com campos apropriados
    
    void* obj = malloc(sizeof(void*));  // Aloca espaço mínimo
    if (!obj) {
        fprintf(stderr, "[new] Failed to allocate memory\n");
        jvm->exit_code = 1;
        jvm->running = false;
        return;
    }
    
    operand_stack_push_reference(&frame->operand_stack, obj);
    printf("[new] Object allocated at %p\n", obj);
}

void getfield_op(JVM* jvm, Frame* frame) {
    if (!jvm || !frame || !frame->class_file) return;
    
    // Ler índice do field reference
    u1 byte1 = frame->code[frame->pc++];
    u1 byte2 = frame->code[frame->pc++];
    u2 field_idx = (u2)((byte1 << 8) | byte2);
    
    // Pop object reference da pilha
    void* obj = operand_stack_pop_reference(&frame->operand_stack);
    
    printf("[getfield] Getting field #%d from object %p\n", field_idx, obj);
    
    // Simplificação: por enquanto apenas retorna 0
    // Uma implementação completa buscaria o campo na classe e acessaria o valor
    operand_stack_push_int(&frame->operand_stack, 0);
}

void putfield_op(JVM* jvm, Frame* frame) {
    if (!jvm || !frame || !frame->class_file) return;
    
    // Ler índice do field reference
    u1 byte1 = frame->code[frame->pc++];
    u1 byte2 = frame->code[frame->pc++];
    u2 field_idx = (u2)((byte1 << 8) | byte2);
    
    // Pop valor da pilha
    int32_t value = operand_stack_pop_int(&frame->operand_stack);
    
    // Pop object reference
    void* obj = operand_stack_pop_reference(&frame->operand_stack);
    
    printf("[putfield] Setting field #%d on object %p to %d\n", field_idx, obj, value);
    
    // Simplificação: por enquanto apenas lê e descarta
    // Uma implementação completa armazenaria o valor no objeto
}

void invokespecial_op(JVM* jvm, Frame* frame) {
    if (!jvm || !frame) return;
    
    // Ler índice do method reference
    u1 byte1 = frame->code[frame->pc++];
    u1 byte2 = frame->code[frame->pc++];
    u2 method_idx = (u2)((byte1 << 8) | byte2);
    
    printf("[invokespecial] Calling special method #%d\n", method_idx);
    
    // Simplificação: descarta argumentos
    // Uma implementação completa resolveria o construtor e criaria novo frame
    // Por enquanto, apenas consome do stack (se houver argumentos)
}
