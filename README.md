
# Máquina Virtual Java Simplificada (JVM)

Este projeto implementa uma Máquina Virtual Java simplificada, responsável por carregar arquivos `.class`, construir as estruturas necessárias para execução e interpretar um subconjunto de bytecodes Java.

Esta é a **segunda fase** do trabalho, sucedendo o Leitor/Exibidor de `.class`, que é utilizado aqui como módulo de carregamento de classes.

## 1. Objetivos da Fase 2

O objetivo desta etapa é implementar uma JVM funcional, porém reduzida, capaz de:

* Carregar uma classe `.class` utilizando o leitor desenvolvido na Fase 1.
* Construir as áreas de memória internas da JVM (Área de Métodos, Heap, Frames de Execução).
* Interpretar e executar instruções do bytecode Java.
* Criar e destruir frames de execução conforme chamadas de métodos.
* Executar o método `main` da classe fornecida.

A implementação segue o projeto lógico definido pelo professor, com simplificações importantes:

* Não possui Garbage Collector.
* Não implementa o Verifier de bytecode.
* Não suporta múltiplas threads.
* Não possui suporte a JNI.
* Executa apenas um subconjunto essencial de instruções.

## 2. Arquitetura da JVM

A arquitetura foi dividida em módulos independentes, cada um responsável por uma parte do ambiente de execução.

### 2.1 Class Loader

É responsável por utilizar o Leitor/Exibidor da Fase 1 para:

* Ler o arquivo `.class`.
* Produzir a estrutura `ClassFile`.
* Registrar a classe na Área de Métodos.

> **Nota:** O carregamento não realiza verificação de segurança; assume-se que o arquivo é válido.

### 2.2 Área de Métodos (Method Area)

A Área de Métodos funciona como o repositório de classes carregadas. Armazena:

* Metadados de classe.
* Métodos e seus atributos `Code`.
* Tabelas auxiliares necessárias para execução.
* Informações necessárias para resolução de métodos durante instruções como `invokestatic` ou `invokevirtual`.

Cada classe carregada mantém ponteiros para sua estrutura `ClassFile` e para os métodos acessíveis.

### 2.3 Heap

A Heap é responsável pelo armazenamento de:

* Objetos instanciados.
* Arrays de tipos primitivos ou referências.

A implementação utiliza a heap do sistema operacional por meio das funções `malloc()`, `calloc()` e `free()`.

Por não haver Garbage Collector, toda memória é liberada apenas ao final da execução da JVM, por meio de uma desalocação controlada.

Cada objeto contém:

* Uma referência para sua classe.
* Um vetor de campos (instância).
* Informações específicas para arrays, quando aplicável.

### 2.4 Frames de Execução

Cada chamada de método cria um Frame contendo:

* Variáveis locais.
* Pilha de operandos.
* Contador de programa (PC).
* Referência para o método sendo executado.
* Ponteiro para o frame anterior (modelando a pilha de chamadas).

Quando o método retorna, o frame é removido e o valor de retorno, se existir, é copiado para o frame anterior.

### 2.5 Engine de Execução (Interpretador)

A Engine é o laço principal da JVM. Ela executa o bytecode de forma sequencial, instrução por instrução.

**Importante:** A execução de instruções **não** é implementada por meio de um bloco `switch-case`, mas através de  **ponteiros para funções** , utilizando uma  **tabela de opcodes** .

Esse mecanismo funciona assim:

1. Cada instrução da JVM possui um código numérico entre 0 e 255.
2. A JVM inicializa uma tabela de 256 posições.
3. Cada posição contém um ponteiro para a função correspondente ao opcode.

**Exemplo de inicialização:**

```
OpcodeFunc tabela_opcodes[256];

void inicializar_tabela_opcodes() {
    for (int i = 0; i < 256; i++)
        tabela_opcodes[i] = NULL;

    tabela_opcodes[0x60] = op_iadd;    // iadd
    tabela_opcodes[0xAC] = op_ireturn; // ireturn
    tabela_opcodes[0xA7] = op_goto;    // goto
}

```

**Loop da Engine:**

```
while (jvm->frame_atual != NULL) {
    u1 opcode = frame->code[frame->pc];
    OpcodeFunc func = tabela_opcodes[opcode];

    if (func == NULL) {
        printf("Opcode não implementado: 0x%02X\n", opcode);
        exit(1);
    }

    func(jvm, frame); // Chama a função correspondente ao opcode
}

```

Com isso, a Engine torna-se modular e extensível, e evita o uso do bloco `switch`.

## 3. Conjunto de Bytecodes Suportados

A implementação contempla um subconjunto mínimo necessário para executar programas simples:

* **Instruções aritméticas:** `iadd`, `isub`, `imul`, `idiv`.
* **Instruções de carregamento/armazenamento:** `iload`, `istore`, `aload`, `ldc`, `bipush`.
* **Instruções de controle de fluxo:** `goto`, `ifeq`, `ifne`, `return`, `ireturn`.
* **Instruções de criação e manipulação de objetos:** `new`, `getfield`, `putfield`, `invokestatic`, `invokevirtual`.

Outras instruções podem ser incluídas posteriormente, bastando adicionar novas funções à tabela de opcodes.

## 4. Estrutura do Projeto

```
jvm/
│
├── main.c
├── class_loader/
│   └── leitor.class (Fase 1)
│
├── runtime/
│   ├── tipos.h
│   ├── jvm.h / jvm.c
│   ├── frame.h / frame.c
│   ├── heap.h / heap.c
│   └── method_area.h / method_area.c
│
├── engine/
│   ├── engine.h / engine.c
│   └── opcode_table.c
│
├── opcodes/
│   ├── opcodes.h
│   ├── opcodes_aritmetica.c
│   ├── opcodes_load_store.c
│   ├── opcodes_controle.c
│   └── opcodes_objetos.c
│
└── Makefile

```

## 5. Compilação e Execução

### Compilar

```
make

```

### Executar

```
./bin/jvm Teste.class

```

**Pré-requisito:** o arquivo `.class` deve ser compilado previamente com `javac`.

## 6. Testes Sugeridos

Recomenda-se iniciar com programas simples:

### Operações aritméticas

```
public class Teste {
    public static void main(String[] args) {
        int x = 2 + 3;
    }
}

```

### Métodos estáticos

```
class T {
    static int soma(int a, int b) { return a + b; }

    public static void main(String[] args) {
        soma(1, 2);
    }
}

```

### Criação de objetos

```
class A { int x; }

public class Teste {
    public static void main(String[] args) {
        A a = new A();
        a.x = 10;
    }
}

```

## 7. Divisão do Trabalho (5 Integrantes)

| **Integrante** | **Módulo**                          | **Responsabilidades**                                                                                                                                 |
| -------------------- | ------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **1**          | **JVM base**                         | Implementação de `jvm.c`, inicialização da JVM, finalização, integração com engine e area de métodos, ajustes do `main.c`.                     |
| **2**          | **Class Loader e Área de Métodos** | Integração com o leitor da fase 1, armazenamento de classes carregadas, busca de métodos.                                                                |
| **3**          | **Frames e Pilha de Operandos**      | Implementação de `frame.c`, criação/destruição de frames, pilha de operandos, variáveis locais.                                                    |
| **4**          | **Heap e Objetos**                   | Implementação de objetos e arrays, alocação via `malloc/calloc`, funções `new`,`getfield`,`putfield`.                                         |
| **5**          | **Engine e Bytecodes**               | Implementação da Engine usando tabela de ponteiros, criação da tabela de opcodes, implementação dos bytecodes aritméticos, de controle e load/store. |
