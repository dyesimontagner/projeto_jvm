#include "jvm.h"
#include "engine.h" //tem que criar ainda.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// FUNÇÕES AUXILIARES INTERNAS
// ============================================================================

static OperandStack* operand_stack_create(int max_size) {
    OperandStack* stack = (OperandStack*)malloc(sizeof(OperandStack));
    if (!stack) return NULL;
    
    stack->elements = (StackElement*)calloc(max_size, sizeof(StackElement));
    if (!stack->elements) {
        free(stack);
        return NULL;
    }
    
    stack->top = -1;
    stack->max_size = max_size;
    return stack;
}

static void operand_stack_destroy(OperandStack* stack) {
    if (stack) {
        if (stack->elements) free(stack->elements);
        free(stack);
    }
}

static LocalVariables* local_variables_create(int size) {
    LocalVariables* locals = (LocalVariables*)malloc(sizeof(LocalVariables));
    if (!locals) return NULL;
    
    locals->variables = (StackElement*)calloc(size, sizeof(StackElement));
    if (!locals->variables) {
        free(locals);
        return NULL;
    }
    
    locals->size = size;
    return locals;
}

static void local_variables_destroy(LocalVariables* locals) {
    if (locals) {
        if (locals->variables) free(locals->variables);
        free(locals);
    }
}

// ============================================================================
// OPERAÇÕES DA PILHA DE OPERANDOS
// ============================================================================

void operand_stack_push_int(OperandStack* stack, int32_t value) {
    if (stack->top >= stack->max_size - 1) {
        fprintf(stderr, "Erro: Stack overflow!\n");
        return;
    }
    stack->top++;
    stack->elements[stack->top].type = TYPE_INT;
    stack->elements[stack->top].value.int_val = value;
}

void operand_stack_push_float(OperandStack* stack, float value) {
    if (stack->top >= stack->max_size - 1) {
        fprintf(stderr, "Erro: Stack overflow!\n");
        return;
    }
    stack->top++;
    stack->elements[stack->top].type = TYPE_FLOAT;
    stack->elements[stack->top].value.float_val = value;
}

void operand_stack_push_long(OperandStack* stack, int64_t value) {
    if (stack->top >= stack->max_size - 2) {
        fprintf(stderr, "Erro: Stack overflow!\n");
        return;
    }
    // Long ocupa 2 slots
    stack->top += 2;
    stack->elements[stack->top - 1].type = TYPE_LONG;
    stack->elements[stack->top - 1].value.long_val = value;
}

void operand_stack_push_double(OperandStack* stack, double value) {
    if (stack->top >= stack->max_size - 2) {
        fprintf(stderr, "Erro: Stack overflow!\n");
        return;
    }
    // Double ocupa 2 slots
    stack->top += 2;
    stack->elements[stack->top - 1].type = TYPE_DOUBLE;
    stack->elements[stack->top - 1].value.double_val = value;
}

void operand_stack_push_reference(OperandStack* stack, void* ref) {
    if (stack->top >= stack->max_size - 1) {
        fprintf(stderr, "Erro: Stack overflow!\n");
        return;
    }
    stack->top++;
    stack->elements[stack->top].type = TYPE_REFERENCE;
    stack->elements[stack->top].value.reference = ref;
}

int32_t operand_stack_pop_int(OperandStack* stack) {
    if (stack->top < 0) {
        fprintf(stderr, "Erro: Stack underflow!\n");
        return 0;
    }
    int32_t value = stack->elements[stack->top].value.int_val;
    stack->top--;
    return value;
}

float operand_stack_pop_float(OperandStack* stack) {
    if (stack->top < 0) {
        fprintf(stderr, "Erro: Stack underflow!\n");
        return 0.0f;
    }
    float value = stack->elements[stack->top].value.float_val;
    stack->top--;
    return value;
}

int64_t operand_stack_pop_long(OperandStack* stack) {
    if (stack->top < 1) {
        fprintf(stderr, "Erro: Stack underflow!\n");
        return 0;
    }
    int64_t value = stack->elements[stack->top - 1].value.long_val;
    stack->top -= 2;
    return value;
}

double operand_stack_pop_double(OperandStack* stack) {
    if (stack->top < 1) {
        fprintf(stderr, "Erro: Stack underflow!\n");
        return 0.0;
    }
    double value = stack->elements[stack->top - 1].value.double_val;
    stack->top -= 2;
    return value;
}

void* operand_stack_pop_reference(OperandStack* stack) {
    if (stack->top < 0) {
        fprintf(stderr, "Erro: Stack underflow!\n");
        return NULL;
    }
    void* ref = stack->elements[stack->top].value.reference;
    stack->top--;
    return ref;
}

// ============================================================================
// OPERAÇÕES DE VARIÁVEIS LOCAIS
// ============================================================================

void local_var_set_int(LocalVariables* locals, int index, int32_t value) {
    if (index >= locals->size) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return;
    }
    locals->variables[index].type = TYPE_INT;
    locals->variables[index].value.int_val = value;
}

void local_var_set_float(LocalVariables* locals, int index, float value) {
    if (index >= locals->size) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return;
    }
    locals->variables[index].type = TYPE_FLOAT;
    locals->variables[index].value.float_val = value;
}

void local_var_set_long(LocalVariables* locals, int index, int64_t value) {
    if (index >= locals->size - 1) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return;
    }
    locals->variables[index].type = TYPE_LONG;
    locals->variables[index].value.long_val = value;
}

void local_var_set_double(LocalVariables* locals, int index, double value) {
    if (index >= locals->size - 1) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return;
    }
    locals->variables[index].type = TYPE_DOUBLE;
    locals->variables[index].value.double_val = value;
}

void local_var_set_reference(LocalVariables* locals, int index, void* ref) {
    if (index >= locals->size) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return;
    }
    locals->variables[index].type = TYPE_REFERENCE;
    locals->variables[index].value.reference = ref;
}

int32_t local_var_get_int(LocalVariables* locals, int index) {
    if (index >= locals->size) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return 0;
    }
    return locals->variables[index].value.int_val;
}

float local_var_get_float(LocalVariables* locals, int index) {
    if (index >= locals->size) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return 0.0f;
    }
    return locals->variables[index].value.float_val;
}

int64_t local_var_get_long(LocalVariables* locals, int index) {
    if (index >= locals->size - 1) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return 0;
    }
    return locals->variables[index].value.long_val;
}

double local_var_get_double(LocalVariables* locals, int index) {
    if (index >= locals->size - 1) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return 0.0;
    }
    return locals->variables[index].value.double_val;
}

void* local_var_get_reference(LocalVariables* locals, int index) {
    if (index >= locals->size) {
        fprintf(stderr, "Erro: Índice de variável local inválido!\n");
        return NULL;
    }
    return locals->variables[index].value.reference;
}

// ============================================================================
// GERENCIAMENTO DE FRAMES
// ============================================================================

Frame* frame_create(ClassFile* class_file, method_info* method) {
    Frame* frame = (Frame*)malloc(sizeof(Frame));
    if (!frame) return NULL;
    
    // Encontrar o atributo Code do método
    Code_attribute* code_attr = NULL;
    for (int i = 0; i < method->attributes_count; i++) {
        attribute_info* attr = &method->attributes[i];
        const char* attr_name = get_utf8_from_pool(attr->attribute_name_index, class_file->constant_pool, class_file->constant_pool_count);
        if (attr_name && strcmp(attr_name, "Code") == 0) {
            code_attr = attr->attr_info.code_info;
            break;
        }
    }
    
    if (!code_attr) {
        free(frame);
        return NULL;
    }
    
    // Inicializar pilha de operandos
    OperandStack* stack = operand_stack_create(code_attr->max_stack);
    if (!stack) {
        free(frame);
        return NULL;
    }
    frame->operand_stack = *stack;
    free(stack);
    
    // Inicializar variáveis locais
    LocalVariables* locals = local_variables_create(code_attr->max_locals);
    if (!locals) {
        operand_stack_destroy(&frame->operand_stack);
        free(frame);
        return NULL;
    }
    frame->local_vars = *locals;
    free(locals);
    
    // Configurar código e PC
    frame->code = code_attr->code;
    frame->code_length = code_attr->code_length;
    frame->pc = 0;
    
    // Referências
    frame->class_file = class_file;
    frame->method = method;
    frame->previous = NULL;
    
    return frame;
}

void frame_destroy(Frame* frame) {
    if (frame) {
        if (frame->operand_stack.elements) free(frame->operand_stack.elements);
        if (frame->local_vars.variables) free(frame->local_vars.variables);
        free(frame);
    }
}

void frame_stack_push(FrameStack* stack, Frame* frame) {
    if (stack->frame_count >= stack->max_frames) {
        fprintf(stderr, "Erro: StackOverflowError - muitos frames!\n");
        return;
    }
    
    frame->previous = stack->top;
    stack->top = frame;
    stack->frame_count++;
}

Frame* frame_stack_pop(FrameStack* stack) {
    if (stack->top == NULL) {
        return NULL;
    }
    
    Frame* frame = stack->top;
    stack->top = frame->previous;
    stack->frame_count--;
    
    return frame;
}

// ============================================================================
// GERENCIAMENTO DA METHOD AREA
// ============================================================================

static MethodAreaEntry* method_area_entry_create(const char* class_name, ClassFile* class_file) {
    MethodAreaEntry* entry = (MethodAreaEntry*)malloc(sizeof(MethodAreaEntry));
    if (!entry) return NULL;
    
    entry->class_name = NULL;
    if (class_name) {
        size_t len = strlen(class_name);
        entry->class_name = (char*)malloc(len + 1);
        if (!entry->class_name) { free(entry); return NULL; }
        memcpy(entry->class_name, class_name, len + 1);
    }
    entry->class_file = class_file;
    entry->next = NULL;
    
    return entry;
}

static void method_area_entry_destroy(MethodAreaEntry* entry) {
    if (entry) {
        if (entry->class_name) free(entry->class_name);
        if (entry->class_file) free_class_file(entry->class_file);
        free(entry);
    }
}

// ============================================================================
// INICIALIZAÇÃO E FINALIZAÇÃO DA JVM
// ============================================================================

JVM* jvm_create(void) {
    JVM* jvm = (JVM*)malloc(sizeof(JVM));
    if (!jvm) {
        fprintf(stderr, "Erro: Falha ao alocar memória para JVM!\n");
        return NULL;
    }
    
    // Inicializar Method Area
    jvm->method_area.classes = NULL;
    jvm->method_area.class_count = 0;
    
    // Inicializar Frame Stack
    jvm->frame_stack.top = NULL;
    jvm->frame_stack.frame_count = 0;
    jvm->frame_stack.max_frames = 1000;  // Limite de frames
    
    // Estado inicial
    jvm->running = false;
    jvm->exit_code = 0;
    
    printf("JVM inicializada com sucesso.\n");
    return jvm;
}

void jvm_destroy(JVM* jvm) {
    if (!jvm) return;
    
    // Liberar todos os frames da pilha
    while (jvm->frame_stack.top != NULL) {
        Frame* frame = frame_stack_pop(&jvm->frame_stack);
        frame_destroy(frame);
    }
    
    // Liberar todas as classes da Method Area
    MethodAreaEntry* current = jvm->method_area.classes;
    while (current != NULL) {
        MethodAreaEntry* next = current->next;
        method_area_entry_destroy(current);
        current = next;
    }
    
    free(jvm);
    printf("JVM finalizada.\n");
}

// ============================================================================
// GERENCIAMENTO DE CLASSES
// ============================================================================

bool jvm_load_class(JVM* jvm, const char* filename) {
    if (!jvm || !filename) return false;
    
    printf("Carregando classe: %s\n", filename);
    
    // Ler o arquivo .class
    ClassFile* class_file = read_class_file(filename);
    if (!class_file) {
        fprintf(stderr, "Erro: Falha ao ler arquivo .class: %s\n", filename);
        return false;
    }
    
    // Obter nome da classe do constant pool
    char class_name[256];
    u2 this_class_idx = class_file->this_class;
    if (this_class_idx > 0 && this_class_idx < class_file->constant_pool_count) {
        cp_info* this_class_entry = class_file->constant_pool[this_class_idx];
        if (this_class_entry && this_class_entry->tag == CONSTANT_Class) {
            u2 name_idx = this_class_entry->info.class_info.name_index;
            const char* cname = get_utf8_from_pool(name_idx, class_file->constant_pool, class_file->constant_pool_count);
            if (cname) {
                strncpy(class_name, cname, 255);
                class_name[255] = '\0';
            }
        }
    } else {
        strncpy(class_name, filename, 255);
        class_name[255] = '\0';
    }
    
    // Adicionar à Method Area
    MethodAreaEntry* entry = method_area_entry_create(class_name, class_file);
    if (!entry) {
        free_class_file(class_file);
        return false;
    }
    
    entry->next = jvm->method_area.classes;
    jvm->method_area.classes = entry;
    jvm->method_area.class_count++;
    
    printf("Classe '%s' carregada na Method Area.\n", class_name);
    return true;
}

ClassFile* jvm_find_class(JVM* jvm, const char* class_name) {
    if (!jvm || !class_name) return NULL;
    
    MethodAreaEntry* current = jvm->method_area.classes;
    while (current != NULL) {
        if (strcmp(current->class_name, class_name) == 0) {
            return current->class_file;
        }
        current = current->next;
    }
    
    return NULL;
}

// ============================================================================
// EXECUÇÃO
// ============================================================================

bool jvm_execute(JVM* jvm, const char* class_name, const char* method_name) {
    if (!jvm || !class_name || !method_name) return false;
    
    // Encontrar a classe
    ClassFile* class_file = jvm_find_class(jvm, class_name);
    if (!class_file) {
        fprintf(stderr, "Erro: Classe '%s' não encontrada!\n", class_name);
        return false;
    }
    
    // Encontrar o método
    method_info* method = NULL;
    for (int i = 0; i < class_file->methods_count; i++) {
        u2 name_idx = class_file->methods[i].name_index;
        if (name_idx > 0 && name_idx < class_file->constant_pool_count) {
            const char* mname = get_utf8_from_pool(name_idx, class_file->constant_pool, class_file->constant_pool_count);
            if (mname && strcmp(mname, method_name) == 0) {
                method = &class_file->methods[i];
                break;
            }
        }
    }
    
    if (!method) {
        fprintf(stderr, "Erro: Método '%s' não encontrado na classe '%s'!\n", 
                method_name, class_name);
        return false;
    }
    
    // Criar frame para o método
    Frame* frame = frame_create(class_file, method);
    if (!frame) {
        fprintf(stderr, "Erro: Falha ao criar frame para método '%s'!\n", method_name);
        return false;
    }
    
    // Adicionar frame à pilha
    frame_stack_push(&jvm->frame_stack, frame);
    
    printf("Executando método '%s' da classe '%s'\n", method_name, class_name);
    jvm->running = true;
    
    return true;
}

void jvm_run(JVM* jvm) {
    if (!jvm || !jvm->running) return;
    
    printf("\n========================================\n");
    printf("Iniciando execução da JVM...\n");
    printf("========================================\n\n");
    
    // Loop principal de execução
    while (jvm->running && jvm->frame_stack.top != NULL) {
        Frame* current_frame = jvm->frame_stack.top;
        
        // Verificar se chegou ao fim do código
        if (current_frame->pc >= current_frame->code_length) {
            printf("Método finalizado.\n");
            Frame* finished_frame = frame_stack_pop(&jvm->frame_stack);
            frame_destroy(finished_frame);
            continue;
        }
        
        // Executar próxima instrução usando o engine
        execute_bytecode(jvm, current_frame);
    }
    
    printf("\n========================================\n");
    printf("Execução finalizada.\n");
    printf("Código de saída: %d\n", jvm->exit_code);
    printf("========================================\n");
}

/*
 * Stub de engine: implementação mínima de `execute_bytecode`
 * para permitir compilação e teste do módulo JVM base enquanto
 * o engine real não estiver pronto.
 *
 * Comportamento: avança o PC do frame até o fim do código,
 * fazendo com que o frame seja finalizado no próximo loop.
 */
void execute_bytecode(JVM* jvm, Frame* frame) {
    if (!jvm || !frame) return;

    /* Avança o program counter ao final do código */
    frame->pc = frame->code_length;
}

// ============================================================================
// UTILITÁRIOS
// ============================================================================

void jvm_print_status(JVM* jvm) {
    if (!jvm) return;
    
    printf("\n=== Status da JVM ===\n");
    printf("Estado: %s\n", jvm->running ? "Em execução" : "Parada");
    printf("Classes carregadas: %d\n", jvm->method_area.class_count);
    printf("Frames na pilha: %d\n", jvm->frame_stack.frame_count);
    
    if (jvm->frame_stack.top != NULL) {
        Frame* frame = jvm->frame_stack.top;
        printf("Frame atual - PC: %d, Code Length: %d\n", 
               frame->pc, frame->code_length);
        printf("Stack top: %d, Locals size: %d\n",
               frame->operand_stack.top, frame->local_vars.size);
    }
    
    printf("=====================\n\n");
}
