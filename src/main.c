#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leitor_class.h"
#include "exibidor.h"
#include "jvm.h"

void print_usage(const char* program_name) {
    printf("Uso:\n");
    printf("  %s -exibidor <arquivo.class>     - Modo Leitor/Exibidor\n", program_name);
    printf("  %s -jvm <arquivo.class> [metodo] - Modo JVM (execução)\n", program_name);
    printf("\nExemplos:\n");
    printf("  %s -exibidor Teste.class\n", program_name);
    printf("  %s -jvm Teste.class main\n", program_name);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char* mode = argv[1];
    const char* filename = argv[2];

    // ========================================================================
    // MODO LEITOR/EXIBIDOR
    // ========================================================================
    if (strcmp(mode, "-exibidor") == 0) {
        printf("=== MODO: Leitor/Exibidor ===\n");
        printf("Lendo o arquivo: %s\n\n", filename);

        // Chama a função de leitura
        ClassFile* class_file = read_class_file(filename);

        // Se a leitura falhou, class_file será NULL
        if (class_file == NULL) {
            fprintf(stderr, "Erro: Não foi possível ler o arquivo .class.\n");
            return 1;
        }

        // Chama a função para exibir as informações
        print_class_file_info(class_file);

        // Libera a memória alocada
        free_class_file(class_file);

        return 0;
    }
    
    // ========================================================================
    // MODO JVM (EXECUÇÃO)
    // ========================================================================
    else if (strcmp(mode, "-jvm") == 0) {
        printf("=== MODO: JVM (Execuao) ===\n");
        
        const char* method_name = (argc >= 4) ? argv[3] : "main";
        
        // Criar a JVM
        JVM* jvm = jvm_create();
        if (!jvm) {
            fprintf(stderr, "Erro: Falha ao criar JVM!\n");
            return 1;
        }

        // Carregar a classe
        if (!jvm_load_class(jvm, filename)) {
            fprintf(stderr, "Erro: Falha ao carregar classe!\n");
            jvm_destroy(jvm);
            return 1;
        }

        // Obter o nome da classe do arquivo
        // Simplificação: usar o nome do arquivo sem .class
        char class_name[256];
        strncpy(class_name, filename, 255);
        class_name[255] = '\0';
        
        // Remover extensão .class se existir
        char* dot = strrchr(class_name, '.');
        if (dot && strcmp(dot, ".class") == 0) {
            *dot = '\0';
        }
        
        // Remover caminho se existir
        // Remove caminho (suporta '/' e '\\') para extrair apenas o nome da classe
        char* slash = strrchr(class_name, '/');
        char* bslash = strrchr(class_name, '\\');
        char* sep = (slash && bslash) ? (slash > bslash ? slash : bslash)
                                       : (slash ? slash : bslash);
        if (sep) {
            memmove(class_name, sep + 1, strlen(sep));
        }

        printf("Classe: %s\n", class_name);
        printf("Método: %s\n\n", method_name);

        // Preparar execução do método
        if (!jvm_execute(jvm, class_name, method_name)) {
            fprintf(stderr, "Erro: Falha ao preparar execucao!\n");
            jvm_destroy(jvm);
            return 1;
        }

        // Executar
        jvm_run(jvm);

        // Obter código de saída
        int exit_code = jvm->exit_code;

        // Limpar
        jvm_destroy(jvm);

        return exit_code;
    }
    
    // ========================================================================
    // MODO INVÁLIDO
    // ========================================================================
    else {
        fprintf(stderr, "Erro: Modo invalido '%s'\n\n", mode);
        print_usage(argv[0]);
        return 1;
    }
}