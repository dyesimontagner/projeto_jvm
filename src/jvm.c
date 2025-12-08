#include "jvm.h"
//#include "frame.h"     // frame_create, frame_destroy, frame_stack_push/pop
#include "engine.h"    // execute_bytecode
#include "leitor_class.h" // read_class_file, free_class_file, estruturas da Fase 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// GERENCIAMENTO DA METHOD AREA (interno à JVM)
// ============================================================================

static MethodAreaEntry* method_area_entry_create(const char* class_name,
                                                 ClassFile* class_file) {
    MethodAreaEntry* entry = (MethodAreaEntry*)malloc(sizeof(MethodAreaEntry));
    if (!entry) return NULL;

    entry->class_name = NULL;
    if (class_name) {
        size_t len = strlen(class_name);
        entry->class_name = (char*)malloc(len + 1);
        if (!entry->class_name) {
            free(entry);
            return NULL;
        }
        memcpy(entry->class_name, class_name, len + 1);
    }

    entry->class_file = class_file;
    entry->next = NULL;

    return entry;
}

static void method_area_entry_destroy(MethodAreaEntry* entry) {
    if (!entry) return;

    if (entry->class_name) free(entry->class_name);
    if (entry->class_file) free_class_file(entry->class_file);
    free(entry);
}

// ============================================================================
// INICIALIZAÇaO E FINALIZAÇaO DA JVM
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
    jvm->frame_stack.max_frames = 1000; // limite arbitrário de frames

    // Estado inicial
    jvm->running  = false;
    jvm->exit_code = 0;

    // Inicializa tabela de opcodes (engine)
    engine_init();

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

    // Ler o arquivo .class (funçao da Fase 1)
    ClassFile* class_file = read_class_file(filename);
    if (!class_file) {
        fprintf(stderr, "Erro: Falha ao ler arquivo .class: %s\n", filename);
        return false;
    }

    // Obter nome da classe do constant pool
    char class_name[256] = {0};
    u2 this_class_idx = class_file->this_class;

    if (this_class_idx > 0 && this_class_idx < class_file->constant_pool_count) {
        cp_info* this_class_entry = class_file->constant_pool[this_class_idx];
        if (this_class_entry && this_class_entry->tag == CONSTANT_Class) {
            u2 name_idx = this_class_entry->info.class_info.name_index;
            const char* cname = get_utf8_from_pool(
                name_idx,
                class_file->constant_pool,
                class_file->constant_pool_count
            );
            if (cname) {
                strncpy(class_name, cname, 255);
                class_name[255] = '\0';
            }
        }
    }

    // Se nao conseguir pelo constant pool, usa o filename como fallback
    if (class_name[0] == '\0') {
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
        if (current->class_name &&
            strcmp(current->class_name, class_name) == 0) {
            return current->class_file;
        }
        current = current->next;
    }

    return NULL;
}

// ============================================================================
// EXECUÇaO
// ============================================================================

bool jvm_execute(JVM* jvm, const char* class_name, const char* method_name) {
    if (!jvm || !class_name || !method_name) return false;

    // Encontrar a classe
    ClassFile* class_file = jvm_find_class(jvm, class_name);
    if (!class_file) {
        fprintf(stderr, "Erro: Classe '%s' nao encontrada!\n", class_name);
        return false;
    }

    // Encontrar o Metodo
    method_info* method = NULL;
    for (int i = 0; i < class_file->methods_count; i++) {
        u2 name_idx = class_file->methods[i].name_index;
        if (name_idx > 0 && name_idx < class_file->constant_pool_count) {
            const char* mname = get_utf8_from_pool(
                name_idx,
                class_file->constant_pool,
                class_file->constant_pool_count
            );
            if (mname && strcmp(mname, method_name) == 0) {
                method = &class_file->methods[i];
                break;
            }
        }
    }

    if (!method) {
        fprintf(stderr,
                "Erro: Metodo '%s' nao encontrado na classe '%s'!\n",
                method_name, class_name);
        return false;
    }

    // Criar frame para o Metodo (implementaçao em frame.c)
    Frame* frame = frame_create(class_file, method);
    if (!frame) {
        fprintf(stderr, "Erro: Falha ao criar frame para metodo '%s'!\n",
                method_name);
        return false;
    }

    // Adicionar frame à pilha
    frame_stack_push(&jvm->frame_stack, frame);

    printf("Executando metodo '%s' da classe '%s'\n", method_name, class_name);
    jvm->running = true;

    return true;
}

void jvm_run(JVM* jvm) {
    if (!jvm || !jvm->running) return;

    printf("\n========================================\n");
    printf("Iniciando execucao da JVM...\n");
    printf("========================================\n\n");

    // Loop principal de execuçao
    while (jvm->running && jvm->frame_stack.top != NULL) {
        Frame* current_frame = jvm->frame_stack.top;

        // Verificar se chegou ao fim do código
        if (current_frame->pc >= current_frame->code_length) {
            printf("Metodo finalizado.\n");
            Frame* finished_frame = frame_stack_pop(&jvm->frame_stack);
            frame_destroy(finished_frame);
            continue;
        }

        // Executar próxima instruçao usando o engine (engine.c)
        execute_bytecode(jvm, current_frame);
    }

    jvm->running = false;

    printf("\n========================================\n");
    printf("Execucao finalizada.\n");
    printf("Codigo de saida: %d\n", jvm->exit_code);
    printf("========================================\n");
}

// ============================================================================
// UTILITÁRIOS
// ============================================================================

void jvm_print_status(JVM* jvm) {
    if (!jvm) return;

    printf("\n=== Status da JVM ===\n");
    printf("Estado: %s\n", jvm->running ? "Em execucao" : "Parada");
    printf("Classes carregadas: %d\n", jvm->method_area.class_count);
    printf("Frames na pilha: %d\n", jvm->frame_stack.frame_count);

    if (jvm->frame_stack.top != NULL) {
        Frame* frame = jvm->frame_stack.top;
        printf("Frame atual - PC: %d, Code Length: %d\n",
               (int)frame->pc, (int)frame->code_length);
        printf("Stack top: %d, Locals size: %d\n",
               frame->operand_stack.top, frame->local_vars.size);
    }

    printf("=====================\n\n");
}

/**
 * Procura um Metodo em uma ClassFile carregada pelo nome e descritor.
 * Retorna o ponteiro method_info* se encontrado, ou NULL.
 */
method_info* jvm_find_method(ClassFile* class_file, 
                             const char* method_name, 
                             const char* descriptor) {
    
    if (!class_file || !method_name || !descriptor) return NULL;

    // Itera sobre todos os métodos na classe
    for (int i = 0; i < class_file->methods_count; i++) {
        method_info* method = &class_file->methods[i];
        
        // Resolve o nome do Metodo na Constant Pool
        const char* mname = get_utf8_from_pool(
            method->name_index,
            class_file->constant_pool,
            class_file->constant_pool_count
        );

        // Resolve o descritor (assinatura) na Constant Pool
        const char* mdesc = get_utf8_from_pool(
            method->descriptor_index,
            class_file->constant_pool,
            class_file->constant_pool_count
        );
        
        // Compara nome e descritor
        if (mname && mdesc && 
            strcmp(mname, method_name) == 0 && 
            strcmp(mdesc, descriptor) == 0) {
            
            return method; // Metodo encontrado
        }
    }
    
    return NULL; // Metodo nao encontrado
}