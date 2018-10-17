CFLAGS = -Wall -Wextra
OUT = lionel
OBJ = main.o
all: $(OUT)
main.o: main.c
	gcc -c main.c $(CFLAGS)
$(OUT): $(OBJ)
	gcc $(OBJ) -o $(OUT) $(CFLAGS)
clean:
	rm *.o
