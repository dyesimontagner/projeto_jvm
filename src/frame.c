#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jvm.h" // Traz todas as definições de structs

// ============================================================================
// CRIAÇÃO E DESTRUIÇÃO DE FRAMES
// ============================================================================

Frame* frame_create(ClassFile* class_file, method_info* method) {
    Frame* frame = (Frame*) calloc(1, sizeof(Frame));
    if (!frame) return NULL;

    // Busca atributo Code
    Code_attribute* code = NULL;
    for (int i = 0; i < method->attributes_count; i++) {
        if (method->attributes[i].attr_info.code_info) { // Simplificacao: assume que ponteiro está lá se nome for "Code"
            code = method->attributes[i].attr_info.code_info;
            break;
        }
    }

    if (code) {
        frame->code = code->code;
        frame->code_length = code->code_length;
        
        // Aloca Variáveis Locais
        frame->local_vars.size = code->max_locals;
        if (code->max_locals > 0) {
            frame->local_vars.variables = (StackElement*) calloc(code->max_locals, sizeof(StackElement));
        }

        // Aloca Pilha de Operandos
        frame->operand_stack.max_size = code->max_stack;
        frame->operand_stack.top = -1; // Pilha vazia começa em -1
        if (code->max_stack > 0) {
            frame->operand_stack.elements = (StackElement*) calloc(code->max_stack, sizeof(StackElement));
        }
    }

    frame->pc = 0;
    frame->class_file = class_file;
    frame->method = method;
    frame->previous = NULL;

    return frame;
}

void frame_destroy(Frame* frame) {
    if (!frame) return;
    if (frame->local_vars.variables) free(frame->local_vars.variables);
    if (frame->operand_stack.elements) free(frame->operand_stack.elements);
    free(frame);
}

// ============================================================================
// PILHA DE FRAMES (JVM Stack)
// ============================================================================

void frame_stack_push(FrameStack* stack, Frame* frame) {
    if (stack->frame_count >= stack->max_frames) {
        printf("Erro: StackOverflowError (Frames)\n");
        exit(1);
    }
    frame->previous = stack->top;
    stack->top = frame;
    stack->frame_count++;
}

Frame* frame_stack_pop(FrameStack* stack) {
    if (stack->top == NULL) return NULL;
    Frame* top = stack->top;
    stack->top = top->previous;
    stack->frame_count--;
    return top;
}

// ============================================================================
// OPERAÇÕES DA PILHA DE OPERANDOS (OperandStack)
// ============================================================================

void operand_stack_push_int(OperandStack* stack, int32_t value) {
    if (stack->top >= stack->max_size - 1) return;
    stack->top++;
    stack->elements[stack->top].type = TYPE_INT;
    stack->elements[stack->top].value.int_val = value;
}

void operand_stack_push_float(OperandStack* stack, float value) {
    if (stack->top >= stack->max_size - 1) return;
    stack->top++;
    stack->elements[stack->top].type = TYPE_FLOAT;
    stack->elements[stack->top].value.float_val = value;
}

void operand_stack_push_long(OperandStack* stack, int64_t value) {
    if (stack->top >= stack->max_size - 2) { 
        fprintf(stderr, "Erro: Stack Overflow (LONG)\n"); return; 
    } 
    // High bytes
    stack->elements[stack->top + 1].type = TYPE_LONG;
    stack->elements[stack->top + 1].value.int_val = (int32_t)(value >> 32); 
    // Low bytes
    stack->elements[stack->top + 2].type = TYPE_LONG;
    stack->elements[stack->top + 2].value.int_val = (int32_t)value;      
    stack->top += 2;
}

void operand_stack_push_double(OperandStack* stack, double value) {
    int64_t bit_value;
    memcpy(&bit_value, &value, sizeof(double)); 

    if (stack->top >= stack->max_size - 2) {
        fprintf(stderr, "Erro: Stack Overflow (DOUBLE)\n"); return; 
    }
    
    stack->elements[stack->top + 1].type = TYPE_DOUBLE;
    stack->elements[stack->top + 1].value.int_val = (int32_t)(bit_value >> 32); 
    stack->elements[stack->top + 2].type = TYPE_DOUBLE;
    stack->elements[stack->top + 2].value.int_val = (int32_t)bit_value;      
    stack->top += 2;
}

void operand_stack_push_reference(OperandStack* stack, void* ref) {
    if (stack->top >= stack->max_size - 1) return;
    stack->top++;
    stack->elements[stack->top].type = TYPE_REFERENCE;
    stack->elements[stack->top].value.reference = ref;
}

// --- POPS ---

int32_t operand_stack_pop_int(OperandStack* stack) {
    if (stack->top < 0) return 0;
    return stack->elements[stack->top--].value.int_val;
}

float operand_stack_pop_float(OperandStack* stack) {
    if (stack->top < 0) return 0.0f;
    return stack->elements[stack->top--].value.float_val;
}

int64_t operand_stack_pop_long(OperandStack* stack) {
    if (stack->top < 1) return 0;
    int64_t low = (uint64_t)stack->elements[stack->top].value.int_val & 0xFFFFFFFF;
    int64_t high = (uint64_t)stack->elements[stack->top - 1].value.int_val << 32;
    stack->top -= 2;
    return high | low;
}

double operand_stack_pop_double(OperandStack* stack) {
    if (stack->top < 1) return 0.0;
    int64_t low = (uint64_t)stack->elements[stack->top].value.int_val & 0xFFFFFFFF;
    int64_t high = (uint64_t)stack->elements[stack->top - 1].value.int_val << 32;
    int64_t bit_value = high | low;
    stack->top -= 2;
    
    double result;
    memcpy(&result, &bit_value, sizeof(double));
    return result;
}

void* operand_stack_pop_reference(OperandStack* stack) {
    if (stack->top < 0) return NULL;
    return stack->elements[stack->top--].value.reference;
}

// ============================================================================
// VARIÁVEIS LOCAIS
// ============================================================================

void local_var_set_int(LocalVariables* locals, int index, int32_t value) {
    if (index < locals->size) {
        locals->variables[index].type = TYPE_INT;
        locals->variables[index].value.int_val = value;
    }
}

int32_t local_var_get_int(LocalVariables* locals, int index) {
    if (index < locals->size) return locals->variables[index].value.int_val;
    return 0;
}

void local_var_set_float(LocalVariables* locals, int index, float value) {
    if (index < locals->size) {
        locals->variables[index].type = TYPE_FLOAT;
        locals->variables[index].value.float_val = value;
    }
}

float local_var_get_float(LocalVariables* locals, int index) {
    if (index < locals->size) return locals->variables[index].value.float_val;
    return 0.0f;
}

void local_var_set_long(LocalVariables* locals, int index, int64_t value) {
    if (index >= 0 && index < locals->size - 1) {
        locals->variables[index].type = TYPE_LONG;
        locals->variables[index].value.int_val = (int32_t)(value >> 32); 
        locals->variables[index + 1].type = TYPE_LONG;
        locals->variables[index + 1].value.int_val = (int32_t)value;      
    }
}

int64_t local_var_get_long(LocalVariables* locals, int index) {
    if (index >= 0 && index < locals->size - 1) {
        int64_t low = (uint64_t)locals->variables[index + 1].value.int_val & 0xFFFFFFFF;
        int64_t high = (uint64_t)locals->variables[index].value.int_val << 32;
        return high | low;
    }
    return 0;
}

void local_var_set_double(LocalVariables* locals, int index, double value) {
    int64_t bit_value;
    memcpy(&bit_value, &value, sizeof(double));
    local_var_set_long(locals, index, bit_value);
}

double local_var_get_double(LocalVariables* locals, int index) {
    int64_t bit_value = local_var_get_long(locals, index);
    double result;
    memcpy(&result, &bit_value, sizeof(double));
    return result;
}

// --- IMPLEMENTAÇÃO CORRETA DE REFERÊNCIAS ---

void local_var_set_reference(LocalVariables* locals, int index, void* ref) {
    if (index < locals->size) {
        locals->variables[index].type = TYPE_REFERENCE;
        locals->variables[index].value.reference = ref;
    }
}

void* local_var_get_reference(LocalVariables* locals, int index) {
    if (index < locals->size) {
        return locals->variables[index].value.reference;
    }
    return NULL;
}
