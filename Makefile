# Compilador a ser usado
CC = gcc

# Flags de compilação padrão (C99, warnings, link com math library)
CFLAGS = -std=c99 -Wall -Wextra -lm

# Flags do AddressSanitizer
ASAN_FLAGS = -fsanitize=address -g

# O nome do nosso executável normal
TARGET = bin/leitor

# NOVO: Nome do executável com ASan
TARGET_ASAN = bin/leitor_asan

# Lista de todos os arquivos .c que compõem o programa
#SOURCES = src/main.c src/leitor_class.c src/constant_pool.c src/opcodes.c src/exibidor.c src/jvm.c# Adicionado exibidor.c
SOURCES = src/main.c src/leitor_class.c src/constant_pool.c src/opcodes.c src/exibidor.c src/jvm.c src/frame.c src/engine.c src/opcode_table.c src/opcodes_const_load_store.c src/opcodes_aritmetica.c src/opcodes_controle.c src/opcodes_metodos.c src/opcodes_referencias.c src/opcodes_objetos.c

# A regra principal: 'all' é executada quando você digita 'make'
all: $(TARGET)

# Como criar o executável a partir dos arquivos fonte
$(TARGET): $(SOURCES)
	@if not exist bin mkdir bin
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Regra para criar o executável com ASan
asan: $(SOURCES)
	@if not exist bin mkdir bin
	$(CC) $(ASAN_FLAGS) $(CFLAGS) -o $(TARGET_ASAN) $(SOURCES)

# Regra para análise estática com Cppcheck
check:
	@echo "Rodando Cppcheck..."
	cppcheck --enable=all --inconclusive --std=c99 --suppress=missingIncludeSystem src/

# Regra para executar todos os testes
test: $(TARGET)
	@echo ============================================
	@echo Executando testes da JVM
	@echo ============================================
	@echo.
	@echo [TEST 1] Mini.class - Aritmetica basica
	@$(TARGET) -jvm test_files\Mini.class main
	@echo.
	@echo [TEST 2] TestDiv.class - Divisao inteira
	@$(TARGET) -jvm test_files\TestDiv.class main
	@echo.
	@echo [TEST 3] TestIfEq.class - Comparacao e branches
	@$(TARGET) -jvm test_files\TestIfEq.class main
	@echo.
	@echo [TEST 4] TestLdc.class - Carregamento de constantes
	@$(TARGET) -jvm test_files\TestLdc.class main
	@echo.
	@echo [TEST 5] TestNew.class - Criacao de objetos
	@$(TARGET) -jvm test_files\TestNew.class main
	@echo.
	@echo [TEST 6] SimpleLoop.class - Loop com invokestatic
	@$(TARGET) -jvm test_files\SimpleLoop.class main
	@echo.
	@echo [TEST 7] Fibonacci.class - Recursao
	@$(TARGET) -jvm test_files\Fibonacci.class main
	@echo.
	@echo [TEST 8] TestDivZero.class - Erro: divisao por zero ^(esperado^)
	-@$(TARGET) -jvm test_files\TestDivZero.class main
	@echo.
	@echo ============================================
	@echo Testes concluidos
	@echo ============================================

# Regra 'clean': remove o executável compilado para uma recompilação limpa
clean:
	rm -rf bin