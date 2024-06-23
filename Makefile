# Nome do compilador
CC = gcc

# Opções de compilação
CFLAGS = -Wall -pthread

# Nome do arquivo de saída
TARGET = projeto

# Lista de arquivos fonte
SRC = projetoSO.c

# Regra para criar o executável
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Regra padrão
all: $(TARGET)

# Limpeza dos arquivos compilados
clean:
	rm -f $(TARGET)

.PHONY: all clean
