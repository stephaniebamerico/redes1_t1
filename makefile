CC = gcc
CFLAGS = -Wall -g 
#-DDEBUG

BIN = bin
SRC = src
INCLUDE = include

all: cliente servidor

clean:
	-rm -f $(BIN)/cliente
	-rm -f $(BIN)/servidor
	-rm -rf $(BIN)

#---------------------------------------------------------------

mkdir_bin:
	mkdir -p $(BIN)

#---------------------------------------------------------------

cliente: mkdir_bin $(INCLUDE)/raw_socket.h $(SRC)/cliente.c $(SRC)/raw_socket.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $(BIN)/cliente $(SRC)/cliente.c $(SRC)/raw_socket.c

servidor: mkdir_bin $(INCLUDE)/raw_socket.h $(SRC)/servidor.c $(SRC)/raw_socket.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $(BIN)/servidor $(SRC)/servidor.c $(SRC)/raw_socket.c