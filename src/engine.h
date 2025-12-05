#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "jvm.h"

/*
 * Este header define apenas as interfaces (protótipos) usadas
 * pela JVM/engine. As structs (JVM, Frame, OperandStack, etc.)
 * devem estar declaradas em jvm.h.
 *
 * Quando você implementar engine.c, basta incluir este header.
 */

/* ========================================================================= */
/*  OPERAÇÕES DA PILHA DE OPERANDOS                                          */
/* ========================================================================= */

void operand_stack_push_int(OperandStack* stack, int32_t value);
void operand_stack_push_float(OperandStack* stack, float value);
void operand_stack_push_long(OperandStack* stack, int64_t value);
void operand_stack_push_double(OperandStack* stack, double value);
void operand_stack_push_reference(OperandStack* stack, void* ref);

int32_t operand_stack_pop_int(OperandStack* stack);
float   operand_stack_pop_float(OperandStack* stack);
int64_t operand_stack_pop_long(OperandStack* stack);
double  operand_stack_pop_double(OperandStack* stack);
void*   operand_stack_pop_reference(OperandStack* stack);

/* ========================================================================= */
/*  OPERAÇÕES DE VARIÁVEIS LOCAIS                                            */
/* ========================================================================= */

void     local_var_set_int(LocalVariables* locals, int index, int32_t value);
void     local_var_set_float(LocalVariables* locals, int index, float value);
void     local_var_set_long(LocalVariables* locals, int index, int64_t value);
void     local_var_set_double(LocalVariables* locals, int index, double value);
void     local_var_set_reference(LocalVariables* locals, int index, void* ref);

int32_t  local_var_get_int(LocalVariables* locals, int index);
float    local_var_get_float(LocalVariables* locals, int index);
int64_t  local_var_get_long(LocalVariables* locals, int index);
double   local_var_get_double(LocalVariables* locals, int index);
void*    local_var_get_reference(LocalVariables* locals, int index);

/* ========================================================================= */
/*  GERENCIAMENTO DE FRAMES                                                  */
/* ========================================================================= */

Frame* frame_create(ClassFile* class_file, method_info* method);
void   frame_destroy(Frame* frame);

void   frame_stack_push(FrameStack* stack, Frame* frame);
Frame* frame_stack_pop(FrameStack* stack);

/* ========================================================================= */
/*  ENGINE / EXECUÇÃO DE BYTECODES                                           */
/* ========================================================================= */

/* Função que você ainda vai implementar em engine.c */
void execute_bytecode(JVM* jvm, Frame* frame);

#endif /* ENGINE_H */
