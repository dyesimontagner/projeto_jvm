#include "heap.h"
#include <stdlib.h>
#include <stdio.h>

// Lista ligada simples apenas para rastrear alocações e liberar no final
typedef struct NoHeap {
    void* objeto;
    struct NoHeap* proximo;
} NoHeap;

NoHeap* lista_objetos = NULL;

void registrar_alocacao(void* obj) {
    NoHeap* novo = (NoHeap*) malloc(sizeof(NoHeap));
    novo->objeto = obj;
    novo->proximo = lista_objetos;
    lista_objetos = novo;
}

void inicializar_heap() {
    lista_objetos = NULL;
    printf("[HEAP] Inicializado (Modo: Sem Garbage Collector)\n");
}

/**
 * Conta quantos campos de instância (não estáticos) uma classe possui.
 */
int contar_fields_instancia(ClassFile* classe) {
    int contador = 0;
    for (int i = 0; i < classe->fields_count; i++) {
        // Verifica as Access Flags. Se NÃO for STATIC (0x0008), é campo de instância.
        if (!(classe->fields[i].access_flags & 0x0008)) {
            // Nota: Doubles e Longs deveriam contar como 2 slots na contagem de tamanho,
            // mas como 1 campo na iteração. Simplificação: assumindo 1 slot por campo aqui
            // ou você deve verificar o descritor do campo.
            contador++; 
        }
    }
    return contador;
}

ObjetoInstancia* heap_criar_objeto(ClassFile* classe) {
    // 1. Aloca a estrutura de controle do objeto
    ObjetoInstancia* novo_obj = (ObjetoInstancia*) malloc(sizeof(ObjetoInstancia));
    
    // 2. Configura o cabeçalho
    novo_obj->cabecalho.tipo = TIPO_OBJETO_INSTANCIA;
    novo_obj->classe_ref = classe;
    
    // 3. Aloca espaço para os dados (fields)
    int qtd_fields = contar_fields_instancia(classe);
    
    if (qtd_fields > 0) {
        // calloc garante que os campos iniciam com 0/null (requisito JVM)
        novo_obj->dados_fields = (int32_t*) calloc(qtd_fields, sizeof(int32_t));
    } else {
        novo_obj->dados_fields = NULL;
    }

    registrar_alocacao(novo_obj);
    return novo_obj;
}

ObjetoArray* heap_criar_array_primitivo(u4 tamanho, u1 tipo_elemento) {
    // 1. Aloca estrutura do array
    ObjetoArray* novo_arr = (ObjetoArray*) malloc(sizeof(ObjetoArray));
    
    // 2. Configura cabeçalho e metadados
    novo_arr->cabecalho.tipo = TIPO_OBJETO_ARRAY;
    novo_arr->tamanho = tamanho; // Atributo arraylength
    novo_arr->tipo_elementos = tipo_elemento;
    
    // 3. Define tamanho do elemento em bytes
    size_t tamanho_tipo = 0;
    switch (tipo_elemento) {
        case 4: /* T_BOOLEAN */ tamanho_tipo = 1; break;
        case 5: /* T_CHAR    */ tamanho_tipo = 2; break;
        case 6: /* T_FLOAT   */ tamanho_tipo = 4; break;
        case 7: /* T_DOUBLE  */ tamanho_tipo = 8; break;
        case 8: /* T_BYTE    */ tamanho_tipo = 1; break;
        case 9: /* T_SHORT   */ tamanho_tipo = 2; break;
        case 10: /* T_INT    */ tamanho_tipo = 4; break;
        case 11: /* T_LONG   */ tamanho_tipo = 8; break;
        default: tamanho_tipo = 4; break; // Fallback
    }
    
    // 4. Aloca os dados contíguos
    if (tamanho > 0) {
        novo_arr->dados = calloc(tamanho, tamanho_tipo);
    } else {
        novo_arr->dados = NULL;
    }
    
    registrar_alocacao(novo_arr);
    return novo_arr;
}

// Opcional: Função para limpar tudo ao encerrar a JVM
void liberar_heap_total() {
    NoHeap* atual = lista_objetos;
    while (atual != NULL) {
        NoHeap* temp = atual;
        
        // Verifica tipo para liberar sub-estruturas
        Objeto* obj = (Objeto*) atual->objeto;
        if (obj->tipo == TIPO_OBJETO_INSTANCIA) {
             ObjetoInstancia* inst = (ObjetoInstancia*) obj;
             if (inst->dados_fields) free(inst->dados_fields);
        } else if (obj->tipo == TIPO_OBJETO_ARRAY) {
             ObjetoArray* arr = (ObjetoArray*) obj;
             if (arr->dados) free(arr->dados);
        }
        
        free(atual->objeto); // Libera o objeto em si
        atual = atual->proximo;
        free(temp); // Libera o nó da lista
    }
    printf("[HEAP] Memoria liberada.\n");
}