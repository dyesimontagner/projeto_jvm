#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include "./leitor_class.h" // Importa a definição de ClassFile

// Definição de tipos básicos
typedef uint8_t u1;
typedef uint32_t u4;

// Enum para identificar o que é o objeto em tempo de execução
typedef enum {
    TIPO_OBJETO_INSTANCIA,
    TIPO_OBJETO_ARRAY
} TipoObjeto;

// --- ESTRUTURAS DE DADOS ---

/**
 * Cabeçalho comum a todos os objetos no Heap.
 * Permite identificar se um ponteiro é uma Instância ou um Array.
 */
typedef struct Objeto {
    TipoObjeto tipo;
} Objeto;

/**
 * Representa uma instância de classe (criada via 'new').
 */
typedef struct ObjetoInstancia {
    Objeto cabecalho;
    
    // Ponteiro para a classe que originou este objeto (para acessar métodos, constant pool, etc)
    ClassFile* classe_ref;
    
    // Array dinâmico de dados.
    // Armazena os valores dos campos (fields) da instância.
    // Usamos 'int32_t' (ou u4) como um container genérico (Slot) de 32 bits.
    // Longs e Doubles ocuparão 2 slots consecutivos.
    int32_t* dados_fields; 
    
} ObjetoInstancia;

/**
 *  Representa um array (criado via 'newarray' ou 'anewarray').
 */
typedef struct ObjetoArray {
    Objeto cabecalho; // Primeiro campo
    
    u4 tamanho;       // O atributo 'arraylength' 
    u1 tipo_elementos; // Tipo dos dados (ex: T_INT, T_FLOAT, ou T_REFERENCE)
    
    // Ponteiro para os dados do array.
    // Dependendo do tipo, faremos cast para (u1*), (u2*), (u4*) ou (Objeto**)
    void* dados;
    
} ObjetoArray;

// --- FUNÇÕES DE GERENCIAMENTO DO HEAP ---

// Inicializa o Heap (simplesmente prepara estruturas globais se necessário)
void inicializar_heap();

// Cria uma nova instância de classe (opcode 'new')
// Recebe a classe resolvida pelo ClassLoader
ObjetoInstancia* heap_criar_objeto(ClassFile* classe);

// Cria um novo array de tipos primitivos (opcode 'newarray')
ObjetoArray* heap_criar_array_primitivo(u4 tamanho, u1 tipo_elemento);

// Cria um novo array de referências (opcode 'anewarray')
ObjetoArray* heap_criar_array_referencia(u4 tamanho, ClassFile* classe_ref);

// Como não temos GC, podemos ter uma função para limpar tudo no final da execução
void liberar_heap_total();

#endif