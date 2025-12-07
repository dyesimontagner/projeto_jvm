#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "leitor_class.h"
#include "jvm.h"
#include "engine.h"
#include "frame.h"

// --- Protótipos ---
void operand_stack_push_int(OperandStack* stack, int32_t value);
void operand_stack_push_float(OperandStack* stack, float value);
void operand_stack_push_long(OperandStack* stack, int64_t value);
void operand_stack_push_double(OperandStack* stack, double value);
void operand_stack_push_reference(OperandStack* stack, void* ref);

int32_t operand_stack_pop_int(OperandStack* stack);
float operand_stack_pop_float(OperandStack* stack);
int64_t operand_stack_pop_long(OperandStack* stack);
double operand_stack_pop_double(OperandStack* stack);
void* operand_stack_pop_reference(OperandStack* stack);

void local_var_set_int(LocalVariables* locals, int index, int32_t value);
int32_t local_var_get_int(LocalVariables* locals, int index);

// --- EXECUÇÃO ---

void execute_bytecode(JVM* jvm, Frame* frame) {
    if (!frame || !frame->code) return;

    while (jvm->running && frame == jvm->frame_stack.top && frame->pc < frame->code_length) {
        u1 opcode = frame->code[frame->pc];
        frame->pc++; 

        switch (opcode) {
            case 0x00: break; // nop

            // --- Constantes ---
            case 0x02: operand_stack_push_int(&frame->operand_stack, -1); break;
            case 0x03: operand_stack_push_int(&frame->operand_stack, 0); break;
            case 0x04: operand_stack_push_int(&frame->operand_stack, 1); break;
            case 0x05: operand_stack_push_int(&frame->operand_stack, 2); break;
            case 0x06: operand_stack_push_int(&frame->operand_stack, 3); break;
            case 0x07: operand_stack_push_int(&frame->operand_stack, 4); break;
            case 0x08: operand_stack_push_int(&frame->operand_stack, 5); break;
            
            case 0x10: { // bipush
                int8_t val = (int8_t)frame->code[frame->pc++];
                operand_stack_push_int(&frame->operand_stack, val);
            } break;

            // --- Loads ---
            case 0x1A: operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 0)); break;
            case 0x1B: operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 1)); break;
            case 0x1C: operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 2)); break;
            case 0x1D: operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, 3)); break;
            case 0x15: { 
                u1 idx = frame->code[frame->pc++];
                operand_stack_push_int(&frame->operand_stack, local_var_get_int(&frame->local_vars, idx));
            } break;

            // --- Stores ---
            case 0x3B: local_var_set_int(&frame->local_vars, 0, operand_stack_pop_int(&frame->operand_stack)); break;
            case 0x3C: local_var_set_int(&frame->local_vars, 1, operand_stack_pop_int(&frame->operand_stack)); break;
            case 0x3D: local_var_set_int(&frame->local_vars, 2, operand_stack_pop_int(&frame->operand_stack)); break;
            case 0x3E: local_var_set_int(&frame->local_vars, 3, operand_stack_pop_int(&frame->operand_stack)); break;
            case 0x36: { 
                u1 idx = frame->code[frame->pc++];
                local_var_set_int(&frame->local_vars, idx, operand_stack_pop_int(&frame->operand_stack));
            } break;

            // --- Aritmética ---
            case 0x60: { // iadd
                int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                operand_stack_push_int(&frame->operand_stack, v1 + v2);
            } break;
            case 0x64: { // isub
                int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                operand_stack_push_int(&frame->operand_stack, v1 - v2);
            } break;
            case 0x68: { // imul
                int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                int32_t res = v1 * v2; 
                operand_stack_push_int(&frame->operand_stack, res);
            } break;
            case 0x84: { // iinc
                u1 idx = frame->code[frame->pc++];
                int8_t const_val = (int8_t)frame->code[frame->pc++];
                int32_t val = local_var_get_int(&frame->local_vars, idx);
                local_var_set_int(&frame->local_vars, idx, val + const_val);
            } break;

            // --- Branchs ---
            case 0xA7: { // goto
                u1 b1 = frame->code[frame->pc++];
                u1 b2 = frame->code[frame->pc++];
                int16_t offset = (int16_t)((b1 << 8) | b2);
                frame->pc = (frame->pc - 3) + offset;
            } break;

            // if_icmpge (>=)
            case 0xA2: { 
                u1 b1 = frame->code[frame->pc++];
                u1 b2 = frame->code[frame->pc++];
                int16_t offset = (int16_t)((b1 << 8) | b2);
                int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                if (v1 >= v2) frame->pc = (frame->pc - 3) + offset;
            } break;

            // if_icmpgt (>) - ADICIONADO PARA GARANTIR FATORIAL
            case 0xA3: { 
                u1 b1 = frame->code[frame->pc++];
                u1 b2 = frame->code[frame->pc++];
                int16_t offset = (int16_t)((b1 << 8) | b2);
                int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                if (v1 > v2) frame->pc = (frame->pc - 3) + offset;
            } break;

            // if_icmple (<=) - ADICIONADO PARA GARANTIR FATORIAL
            case 0xA4: { 
                u1 b1 = frame->code[frame->pc++];
                u1 b2 = frame->code[frame->pc++];
                int16_t offset = (int16_t)((b1 << 8) | b2);
                int32_t v2 = operand_stack_pop_int(&frame->operand_stack);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                if (v1 <= v2) frame->pc = (frame->pc - 3) + offset;
            } break;

            case 0x9A: { // ifne
                u1 b1 = frame->code[frame->pc++];
                u1 b2 = frame->code[frame->pc++];
                int16_t offset = (int16_t)((b1 << 8) | b2);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                if (v1 != 0) frame->pc = (frame->pc - 3) + offset;
            } break;
            
            case 0x9E: { // ifle
                u1 b1 = frame->code[frame->pc++];
                u1 b2 = frame->code[frame->pc++];
                int16_t offset = (int16_t)((b1 << 8) | b2);
                int32_t v1 = operand_stack_pop_int(&frame->operand_stack);
                if (v1 <= 0) frame->pc = (frame->pc - 3) + offset;
            } break;

            // --- Invocações ---
            case 0xB2: // getstatic
                frame->pc += 2; 
                break;

            case 0xB6: // invokevirtual
                {
                    frame->pc += 2; 
                    int32_t val = operand_stack_pop_int(&frame->operand_stack);
                    printf("%d\n", val);
                }
                break;

            case 0xB8: // invokestatic
                {
                    u1 b1 = frame->code[frame->pc++];
                    u1 b2 = frame->code[frame->pc++];
                    u2 cp_index = (b1 << 8) | b2;
                    
                    ClassFile* cf = frame->class_file;
                    method_info* callee = NULL;
                    
                    // 1. Acessa o MethodRef (Sem -1, pois parece ser array direto)
                    cp_info* method_ref = cf->constant_pool[cp_index];
                    
                    if (method_ref->tag == 10) { // CONSTANT_Methodref
                        u2 name_type_idx = method_ref->info.methodref_info.name_and_type_index;
                        cp_info* name_type = cf->constant_pool[name_type_idx];
                        u2 name_idx = name_type->info.name_and_type_info.name_index;
                        cp_info* utf8_name = cf->constant_pool[name_idx];
                        
                        u2 target_len = utf8_name->info.utf8_info.length;
                        u1* target_bytes = utf8_name->info.utf8_info.bytes;
                        
                        for (int i = 0; i < cf->methods_count; i++) {
                            method_info* m = &cf->methods[i];
                            cp_info* m_name_cp = cf->constant_pool[m->name_index]; 
                            
                            u2 m_len = m_name_cp->info.utf8_info.length;
                            u1* m_bytes = m_name_cp->info.utf8_info.bytes;
                            
                            if (m_len == target_len && strncmp((char*)m_bytes, (char*)target_bytes, m_len) == 0) {
                                callee = m;
                                break;
                            }
                        }
                    } else {
                         printf("[ERRO CRITICO] Indice %d tem Tag %d (Esperado 10)\n", cp_index, method_ref->tag);
                    }

                    if (callee) {
                        Frame* new_frame = frame_create(cf, callee);
                        new_frame->previous = frame;
                        
                        if (new_frame->local_vars.size > 0) {
                            int32_t arg = operand_stack_pop_int(&frame->operand_stack);
                            local_var_set_int(&new_frame->local_vars, 0, arg);
                        }
                        frame_stack_push(&jvm->frame_stack, new_frame);
                        return;
                    } else {
                        printf("[ERRO FATAL] Metodo nao encontrado.\n");
                        jvm->running = 0;
                    }
                }
                break;

            case 0xB1: // return
            case 0xAC: // ireturn
                {
                    if (opcode == 0xAC) {
                        int32_t ret_val = operand_stack_pop_int(&frame->operand_stack);
                        if (frame->previous) {
                            operand_stack_push_int(&frame->previous->operand_stack, ret_val);
                        }
                    }
                    frame_stack_pop(&jvm->frame_stack);
                    return;
                }
                break;

            default:
                break;
        }
    }
}