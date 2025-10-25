#  Projeto JVM - Leitor/Exibidor .class

Este é o projeto da disciplina de Software Básico do Departamento de Ciência da Computação da Universidade de Brasília, ministrada pelo Prof. Marcelo Ladeira.

O objetivo desta primeira fase é implementar um **Leitor/Exibidor** para arquivos `.class` Java. O programa é capaz de ler a estrutura binária de um arquivo `.class` e apresentar suas estruturas de forma legível, similar à ferramenta `jclasslib`.

##  Funcionalidades (Fase 1: Leitor/Exibidor)

O programa, escrito em C (padrão C99), atualmente implementa as seguintes funcionalidades:

* **Leitura de Informações Gerais:** Analisa o `magic number`, `minor_version` e `major_version`.
* **Análise do Constant Pool:** Lê e exibe todas as entradas do *Constant Pool*, resolvendo os índices para seus valores em texto.
* **Análise de Estruturas:** Lê e exibe as seções `Fields` e `Methods` da classe.
* **Análise de Bytecode:** Analisa o atributo `Code` dentro de cada método, exibindo `max_stack`, `max_locals` e a lista completa de **bytecodes** em hexadecimal.

##  Ambiente e Requisitos

Este projeto foi desenvolvido e testado em um ambiente **WSL (Ubuntu)** para garantir a compatibilidade com as ferramentas de análise. Os seguintes pacotes são necessários para compilar e analisar o projeto:

* **`build-essential`**: Contém o `gcc` (para compilação em C99)  e o `make`.
* **`default-jdk`**: Contém o `javac` para compilar os arquivos `.class` de teste.
* **`cppcheck`**: Utilizado para a análise estática do código.

##  Como Compilar e Usar

O projeto utiliza um `Makefile` para automatizar a compilação, análise e limpeza.

### 1. Compilação Padrão

Para compilar a versão principal do leitor-exibidor, execute:

```bash
make
```
Isso criará o executável em `bin/leitor`.

### 2. Como Usar o Leitor

Primeiro, compile um arquivo Java de teste (ex: `Teste.java`):

```bash
javac test_files/Teste.java
```
Em seguida, execute o leitor passando o arquivo .class como argumento:

```bash
./bin/leitor test_files/Teste.class
```

### 3. Análise de Código (Cppcheck e ASan)

O Makefile está configurado para usar as ferramentas de análise exigidas.

Análise Estática (Cppcheck): Para verificar o código em busca de erros estáticos e problemas de estilo, rode:
```bash
make check
```
Análise Dinâmica (AddressSanitizer): Para compilar uma versão especial do programa que detecta erros de memória em tempo de execução (como memory leaks ou use-after-free), rode:
```bash
make asan
```
Isso criará o executável bin/leitor_asan. Execute-o da mesma forma que o leitor normal:
```bash
./bin/leitor_asan test_files/Teste.class
```
