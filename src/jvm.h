#ifndef JVM_H
#define JVM_H

#include "common.h"
#include "leitor_class.h"
#include <stdbool.h>

// ============================================================================
// DEFINIÇÕES DE TIPOS DE DADOS DA JVM
// ============================================================================

// Tipos de valores que podem estar na pilha de operandos
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_LONG,
    TYPE_DOUBLE,
    TYPE_REFERENCE,
    TYPE_RETURN_ADDRESS
} ValueType;

// União para representar diferentes tipos de valores
typedef union {
    int32_t int_val;
    float float_val;
    int64_t long_val;
    double double_val;
    void* reference;
    uint32_t return_address;
} Value;

// Estrutura para um elemento da pilha de operandos
typedef struct {
    ValueType type;
    Value value;
} StackElement;

// ============================================================================
// PILHA DE OPERANDOS
// ============================================================================

typedef struct {
    StackElement* elements;  // Array dinâmico de elementos
    int top;                 // Índice do topo da pilha (-1 se vazia)
    int max_size;           // Tamanho máximo da pilha
} OperandStack;

// ============================================================================
// ARRAY DE VARIÁVEIS LOCAIS
// ============================================================================

typedef struct {
    StackElement* variables; // Array de variáveis locais
    int size;                // Tamanho do array
} LocalVariables;

// ============================================================================
// FRAME (Contexto de execução de um método)
// ============================================================================

typedef struct Frame {
    LocalVariables local_vars;     // Variáveis locais do método
    OperandStack operand_stack;    // Pilha de operandos
    u1* code;                      // Ponteiro para o código do método
    u4 code_length;                // Tamanho do código
    u4 pc;                         // Program Counter (próxima instrução)
    ClassFile* class_file;         // Referência ao arquivo de classe
    method_info* method;           // Referência ao método sendo executado
    struct Frame* previous;        // Frame anterior (para retorno)
} Frame;

// ============================================================================
// PILHA DE FRAMES (Java Stack)
// ============================================================================

typedef struct {
    Frame* top;                    // Frame no topo da pilha
    int frame_count;               // Número de frames na pilha
    int max_frames;                // Máximo de frames permitidos
} FrameStack;

// ============================================================================
// METHOD AREA (Área de Métodos)
// ============================================================================

typedef struct MethodAreaEntry {
    char* class_name;                    // Nome da classe
    ClassFile* class_file;               // Arquivo de classe carregado
    struct MethodAreaEntry* next;        // Próxima entrada (lista encadeada)
} MethodAreaEntry;

typedef struct {
    MethodAreaEntry* classes;            // Lista de classes carregadas
    int class_count;                     // Número de classes carregadas
} MethodArea;

// ============================================================================
// JVM (Máquina Virtual Java)
// ============================================================================

typedef struct {
    MethodArea method_area;              // Área de métodos
    FrameStack frame_stack;              // Pilha de frames
    bool running;                        // Estado de execução
    int exit_code;                       // Código de saída
} JVM;

// ============================================================================
// FUNÇÕES PÚBLICAS DA JVM
// ============================================================================

// Inicialização e finalização
JVM* jvm_create(void);
void jvm_destroy(JVM* jvm);

// Gerenciamento de classes
bool jvm_load_class(JVM* jvm, const char* filename);
ClassFile* jvm_find_class(JVM* jvm, const char* class_name);

// Execução
bool jvm_execute(JVM* jvm, const char* class_name, const char* method_name);
void jvm_run(JVM* jvm);

// Gerenciamento de frames
Frame* frame_create(ClassFile* class_file, method_info* method);
void frame_destroy(Frame* frame);
void frame_stack_push(FrameStack* stack, Frame* frame);
Frame* frame_stack_pop(FrameStack* stack);

// Operações da pilha de operandos
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

// Operações de variáveis locais
void local_var_set_int(LocalVariables* locals, int index, int32_t value);
void local_var_set_float(LocalVariables* locals, int index, float value);
void local_var_set_long(LocalVariables* locals, int index, int64_t value);
void local_var_set_double(LocalVariables* locals, int index, double value);
void local_var_set_reference(LocalVariables* locals, int index, void* ref);

int32_t local_var_get_int(LocalVariables* locals, int index);
float local_var_get_float(LocalVariables* locals, int index);
int64_t local_var_get_long(LocalVariables* locals, int index);
double local_var_get_double(LocalVariables* locals, int index);
void* local_var_get_reference(LocalVariables* locals, int index);

// Utilitários
void jvm_print_status(JVM* jvm);

#endif // JVM_H
