# Compilador a ser usado
CC = gcc

# Flags do AddressSanitizer
ASAN_FLAGS = -fsanitize=address -g

# O nome do nosso executável normal
TARGET = bin/leitor

# NOVO: Nome do executável com ASan
TARGET_ASAN = bin/leitor_asan

# Lista de todos os arquivos .c que compõem o programa
SOURCES = src/main.c src/leitor_class.c src/constant_pool.c

# A regra principal: 'all' é executada quando você digita 'make'
all: $(TARGET)

# Como criar o executável a partir dos arquivos fonte
$(TARGET): $(SOURCES)
	@mkdir -p bin  
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
	

# Regra para criar o executável com ASan
asan: $(SOURCES)
	@mkdir -p bin
	$(CC) $(ASAN_FLAGS) $(CFLAGS) -o $(TARGET_ASAN) $(SOURCES)

# Regra para análise estática com Cppcheck
check:
	@echo "Rodando Cppcheck..."
	cppcheck --enable=all --inconclusive --std=c99 --suppress=missingIncludeSystem src/
# Regra 'clean': remove o executável compilado para uma recompilação limpa
# Para usar: digite 'make clean'
clean:
	rm -f $(TARGET) $(TARGET_ASAN)