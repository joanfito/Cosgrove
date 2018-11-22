CFLAGS = -Wall -Wextra -lpthread
OUT = mcgruder
OBJ = main.o Reader.o Configuration.o Connection.o Files.o Exit.o
all: $(OUT)
main.o: main.c Configuration.h Connection.h Files.h Exit.h
	gcc -c main.c $(CFLAGS)
Reader.o: Reader.c Reader.h
	gcc -c Reader.c $(CFLAGS)
Configuration.o: Configuration.c Configuration.h Reader.h
	gcc -c Configuration.c $(CFLAGS)
Connection.o: Connection.c Connection.h Configuration.h Reader.h Exit.h
	gcc -c Connection.c $(CFLAGS)
Files.o: Files.c Files.h Connection.h Reader.h
	gcc -c Files.c $(CFLAGS)
Exit.o: Exit.c Exit.h Connection.h Configuration.h
	gcc -c Exit.c $(CFLAGS)
$(OUT): $(OBJ)
	gcc $(OBJ) -o $(OUT) $(CFLAGS)
clean:
	rm *.o
