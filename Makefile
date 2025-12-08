# Compilador a ser usado
CC = gcc

# Flags de compilação padrão (C99, warnings)
CFLAGS = -std=c99 -Wall -Wextra

# Bibliotecas (Math library deve ficar aqui para ser linkada no final)
LIBS = -lm

# Flags do AddressSanitizer
ASAN_FLAGS = -fsanitize=address -g

# O nome do nosso executável normal
TARGET = bin/leitor

# Nome do executável com ASan
TARGET_ASAN = bin/leitor_asan

# Lista de todos os arquivos .c que compõem o programa
# ADICIONADO: src/heap.c
SOURCES = src/main.c src/leitor_class.c src/constant_pool.c src/opcodes.c \
          src/exibidor.c src/jvm.c src/frame.c src/engine.c \
          src/opcode_table.c src/opcodes_aritmetica.c \
          src/opcodes_const_load_store.c src/opcodes_controle.c \
          src/opcodes_metodos.c src/heap.c

# A regra principal: 'all' é executada quando você digita 'make'
all: $(TARGET)

# Como criar o executável a partir dos arquivos fonte
$(TARGET): $(SOURCES)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

# Regra para criar o executável com ASan
asan: $(SOURCES)
	@mkdir -p bin
	$(CC) $(ASAN_FLAGS) $(CFLAGS) -o $(TARGET_ASAN) $(SOURCES) $(LIBS)

# Regra para análise estática com Cppcheck
check:
	@echo "Rodando Cppcheck..."
	cppcheck --enable=all --inconclusive --std=c99 --suppress=missingIncludeSystem src/

# Regra 'clean': remove o executável compilado para uma recompilação limpa
clean:
	rm -rf bin
