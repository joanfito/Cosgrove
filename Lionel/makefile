CFLAGS = -Wall -Wextra -lpthread
OUT = lionel
OBJ = main.o Configuration.o Connection.o Reader.o Exit.o
all: $(OUT)
main.o: main.c Configuration.h Connection.h Exit.h
	gcc -c main.c $(CFLAGS)
Reader.o: Reader.c Reader.h
	gcc -c Reader.c $(CFLAGS)
Configuration.o: Configuration.c Configuration.h Reader.h
	gcc -c Configuration.c $(CFLAGS)
Connection.o: Connection.c Connection.h Configuration.h Reader.h
	gcc -c Connection.c $(CFLAGS)
Exit.o: Exit.c Exit.h Connection.h
	gcc -c Exit.c $(CFLAGS)
$(OUT): $(OBJ)
	gcc $(OBJ) -o $(OUT) $(CFLAGS)
clean:
	rm *.o
