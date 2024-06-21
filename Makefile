# Makefile para o programa de operações com matrizes usando threads

# Nome do compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall -pthread

# Nome do executável
TARGET = programa

# Arquivo de código-fonte
SRC = projetoSO.c

# Arquivo objeto
OBJ = projetoSO.o

# Regra padrão para compilar o programa
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regra para compilar o arquivo objeto
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Regra para limpar arquivos compilados
clean:
	rm -f $(OBJ) $(TARGET)

# Regra para forçar a recompilação
rebuild: clean all

# Regra para executar o programa com parâmetros fornecidos pelo usuário
run: $(TARGET)
	@echo "Digite o número de threads:"
	@read threads; \
	echo "Digite o tamanho da matriz (n):"; \
	read tamanho; \
	./$(TARGET) $$threads $$tamanho arqA.dat arqB.dat arqC.dat arqD.dat arqE.dat
