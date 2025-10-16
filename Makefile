# Compilador a ser usado
CC = gcc

# Flags de compilação: -std=c99 (padrão C99), -Wall (mostra todos os warnings), -g (inclui infos de debug)
CFLAGS = -std=c99 -Wall -g

# O nome do nosso executável, que será salvo na pasta bin/
TARGET = bin/leitor

# Lista de todos os arquivos .c que compõem o programa
SOURCES = src/main.c src/leitor_class.c src/constant_pool.c

# A regra principal: 'all' é executada quando você digita 'make'
all: $(TARGET)

# Como criar o executável a partir dos arquivos fonte
$(TARGET): $(SOURCES)
	@mkdir -p bin  # Cria a pasta bin/ se ela não existir
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Regra 'clean': remove o executável compilado para uma recompilação limpa
# Para usar: digite 'make clean'
clean:
	rm -f $(TARGET)