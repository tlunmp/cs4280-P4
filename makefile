CC = gcc
CFLAGS = 
TARGET = comp
OBJ = main.o semantics.o parser.o scanner.o codegen.o 
SRC = main.c semantics.c parser.c scanner.c codegen.c

all: $(TARGET) 
	
$(TARGET):$(OBJ)
	$(CC) -o $(TARGET) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	/bin/rm -f *.o $(TARGET) 
