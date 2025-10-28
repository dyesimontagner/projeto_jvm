# Projeto JVM - Leitor/Exibidor .class

O objetivo desta primeira fase é implementar um **Leitor/Exibidor** para arquivos `.class` Java. O programa é capaz de ler a estrutura binária de um arquivo `.class` e apresentar suas estruturas de forma legível, similar às ferramentas `javap` e `jclasslib`.

## Funcionalidades (Fase 1: Leitor/Exibidor)

O programa, escrito em C (padrão C99) e dividido em módulos de leitura (`leitor_class.c`, `constant_pool.c`) e exibição (`exibidor.c`), implementa as seguintes funcionalidades:

* **Leitura de Informações Gerais:** Analisa e exibe o `magic number`, `minor_version` e `major_version`.
* **Análise do Constant Pool:** Lê e exibe todas as entradas do *Constant Pool*, resolvendo os índices para seus valores em texto e exibindo corretamente os tipos numéricos (`Integer`, `Float`, `Long`, `Double`).
* **Análise de Estruturas:**
    * Lê e exibe as seções `Interfaces`, `Fields` e `Methods` da classe.
    * Decodifica e exibe as `access_flags` (como "PUBLIC", "STATIC", etc.) para a classe, campos e métodos.
    * Exibe nomes e descritores resolvidos para campos e métodos.
* **Análise de Atributos:**
    * Identifica e exibe atributos conhecidos como `Code`, `SourceFile`, `LineNumberTable`.
    * Para o atributo `Code`, exibe `max_stack`, `max_locals`, a tabela de exceções e atributos aninhados (como `LineNumberTable`).
* **Análise de Bytecode:**
    * Decodifica a sequência de bytes do atributo `Code` em instruções Java Bytecode legíveis (mnemónicos).
    * Exibe os operandos de cada instrução, resolvendo índices do *Constant Pool* ou calculando *offsets* de *branch* quando aplicável.

## Ambiente e Requisitos

Este projeto foi desenvolvido e testado em um ambiente **WSL (Ubuntu)** para garantir a compatibilidade com as ferramentas de análise. Os seguintes pacotes são necessários para compilar e analisar o projeto:

* **`build-essential`**: Contém o `gcc` (para compilação em C99) e o `make`.
* **`default-jdk`**: Contém o `javac` para compilar os arquivos `.class` de teste.
* **`cppcheck`**: Utilizado para a análise estática do código.

## Como Compilar e Usar

O projeto utiliza um `Makefile` para automatizar a compilação, análise e limpeza.

### 1. Compilação Padrão

Para compilar a versão principal do leitor-exibidor, execute na pasta raiz do projeto:

```bash
make
