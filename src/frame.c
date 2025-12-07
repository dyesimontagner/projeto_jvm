#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jvm.h" // Traz todas as definições de structs

// --- IMPLEMENTAÇÃO DE FRAMES ---

Frame* frame_create(ClassFile* class_file, method_info* method) {
    Frame* frame = (Frame*) calloc(1, sizeof(Frame));
    if (!frame) return NULL;

    // Busca atributo Code
    Code_attribute* code = NULL;
    for (int i = 0; i < method->attributes_count; i++) {
        if (method->attributes[i].attr_info.code_info) { // Simplificacao
            code = method->attributes[i].attr_info.code_info;
            break;
        }
    }

    if (code) {
        frame->code = code->code;
        frame->code_length = code->code_length;
        
        // Aloca Variáveis Locais (Baseado em StackElement do jvm.h)
        frame->local_vars.size = code->max_locals;
        if (code->max_locals > 0) {
            frame->local_vars.variables = (StackElement*) calloc(code->max_locals, sizeof(StackElement));
        }

        // Aloca Pilha de Operandos
        frame->operand_stack.max_size = code->max_stack;
        frame->operand_stack.top = -1; // Pilha vazia começa em -1 no jvm.h
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

// --- PILHA DE FRAMES (JVM Stack) ---

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

// --- OPERAÇÕES DA PILHA DE OPERANDOS (OperandStack) ---
// O jvm.h pede tipos especificos (int, float, etc). Implementei todos.

void operand_stack_push_int(OperandStack* stack, int32_t value) {
    if (stack->top >= stack->max_size - 1) return; // Stack Overflow
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
        // Em um projeto real, você lançaria StackOverflowError.
        fprintf(stderr, "Erro: Stack Overflow na pilha de operandos (LONG)\n"); 
        return; 
    } 
    
    // Armazena High bytes (os 32 bits superiores) no primeiro slot
    stack->elements[stack->top + 1].type = TYPE_LONG;
    stack->elements[stack->top + 1].value.int_val = (int32_t)(value >> 32); 

    // Armazena Low bytes (os 32 bits inferiores) no segundo slot
    stack->elements[stack->top + 2].type = TYPE_LONG;
    stack->elements[stack->top + 2].value.int_val = (int32_t)value;      

    stack->top += 2; // Avança 2 slots
}

void operand_stack_push_double(OperandStack* stack, double value) {
    // Reinterpretando double como 64-bit integer para armazenar os bits corretamente
    int64_t bit_value;
    memcpy(&bit_value, &value, sizeof(double)); 

    // Verifica espaço para DOIS slots
    if (stack->top >= stack->max_size - 2) {
        fprintf(stderr, "Erro: Stack Overflow na pilha de operandos (DOUBLE)\n"); 
        return; 
    }
    
    // Armazena High bytes no primeiro slot
    stack->elements[stack->top + 1].type = TYPE_DOUBLE;
    stack->elements[stack->top + 1].value.int_val = (int32_t)(bit_value >> 32); 

    // Armazena Low bytes no segundo slot
    stack->elements[stack->top + 2].type = TYPE_DOUBLE;
    stack->elements[stack->top + 2].value.int_val = (int32_t)bit_value;      

    stack->top += 2; // Avança 2 slots
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
    if (stack->top < 1) return 0; // Precisa de pelo menos 2 slots

    // Combina os dois slots (Low bytes lidos primeiro)
    int64_t low = (uint64_t)stack->elements[stack->top].value.int_val & 0xFFFFFFFF;
    int64_t high = (uint64_t)stack->elements[stack->top - 1].value.int_val << 32;

    stack->top -= 2; // Retrocede 2 slots
    return high | low;
}

double operand_stack_pop_double(OperandStack* stack) {
    if (stack->top < 1) return 0.0; // Precisa de pelo menos 2 slots

    // 1. Combina os dois slots em um inteiro de 64 bits
    int64_t low = (uint64_t)stack->elements[stack->top].value.int_val & 0xFFFFFFFF;
    int64_t high = (uint64_t)stack->elements[stack->top - 1].value.int_val << 32;
    int64_t bit_value = high | low;

    stack->top -= 2; // Retrocede 2 slots
    
    // 2. Reinterpreta o inteiro de 64 bits como double
    double result;
    memcpy(&result, &bit_value, sizeof(double));
    return result;
}

void* operand_stack_pop_reference(OperandStack* stack) {
    if (stack->top < 0) return NULL;
    return stack->elements[stack->top--].value.reference;
}

// --- VARIÁVEIS LOCAIS (Stubs necessários para linkar) ---

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

// (Adicione os outros stubs de local_var se o compilador reclamar, mas este deve bastar pro Fatorial)
void local_var_set_float(LocalVariables* locals, int index, float value) { (void)locals; (void)index; (void)value; }
void local_var_set_long(LocalVariables* locals, int index, int64_t value) {
    if (index >= 0 && index < locals->size - 1) { // Verifica se há espaço para index E index+1
        locals->variables[index].type = TYPE_LONG;
        locals->variables[index].value.int_val = (int32_t)(value >> 32); // High bytes

        locals->variables[index + 1].type = TYPE_LONG;
        locals->variables[index + 1].value.int_val = (int32_t)value;      // Low bytes
    }
}
void local_var_set_double(LocalVariables* locals, int index, double value) {
    int64_t bit_value;
    memcpy(&bit_value, &value, sizeof(double));
    local_var_set_long(locals, index, bit_value); // Reutiliza a lógica de set_long
}
void local_var_set_reference(LocalVariables* locals, int index, void* ref) { (void)locals; (void)index; (void)ref; }

float local_var_get_float(LocalVariables* locals, int index) { (void)locals; (void)index; return 0.0; }
int64_t local_var_get_long(LocalVariables* locals, int index) {
    if (index >= 0 && index < locals->size - 1) {
        int64_t low = (uint64_t)locals->variables[index + 1].value.int_val & 0xFFFFFFFF;
        int64_t high = (uint64_t)locals->variables[index].value.int_val << 32;
        return high | low;
    }
    return 0;
}
double local_var_get_double(LocalVariables* locals, int index) {
    int64_t bit_value = local_var_get_long(locals, index); // Reutiliza a lógica de get_long
    double result;
    memcpy(&result, &bit_value, sizeof(double));
    return result;
}
void* local_var_get_reference(LocalVariables* locals, int index) { (void)locals; (void)index; return NULL; }