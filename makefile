CC = g++ -std=c++0x
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

cliente: mkdir_bin $(INCLUDE)/raw_socket.h $(INCLUDE)/mensagem.h $(SRC)/cliente.cpp $(SRC)/raw_socket.cpp $(SRC)/mensagem.cpp $(SRC)/operations.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $(BIN)/cliente $(SRC)/cliente.cpp $(SRC)/raw_socket.cpp $(SRC)/mensagem.cpp $(SRC)/operations.cpp

servidor: mkdir_bin $(INCLUDE)/raw_socket.h $(INCLUDE)/mensagem.h $(SRC)/servidor.cpp $(SRC)/raw_socket.cpp $(SRC)/mensagem.cpp $(SRC)/operations.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $(BIN)/servidor $(SRC)/servidor.cpp $(SRC)/raw_socket.cpp $(SRC)/mensagem.cpp $(SRC)/operations.cpp