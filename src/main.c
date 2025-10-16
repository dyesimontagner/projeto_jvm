#include <stdio.h>
#include <stdlib.h>
#include "leitor_class.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.class>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    printf("Lendo o arquivo: %s\n\n", filename);

    // Chama a função de leitura
    ClassFile* class_file = read_class_file(filename);

    // Se a leitura falhou, class_file será NULL
    if (class_file == NULL) {
        fprintf(stderr, "Nao foi possivel ler o arquivo .class.\n");
        return 1;
    }

    // Chama a função para exibir as informações
    print_class_file_info(class_file);

    // Libera a memória alocada
    free_class_file(class_file);

    return 0;
}