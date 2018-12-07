CFLAGS = -Wall -Wextra -lpthread -D_GNU_SOURCE
OUT = lionel
OBJ = main.o Configuration.o Connection.o Reader.o Exit.o Files.o Time.o Date.o
all: $(OUT)
main.o: main.c Configuration.h Connection.h Exit.h Files.h
	gcc -c main.c $(CFLAGS)
Reader.o: Reader.c Reader.h
	gcc -c Reader.c $(CFLAGS)
Configuration.o: Configuration.c Configuration.h Reader.h
	gcc -c Configuration.c $(CFLAGS)
Connection.o: Connection.c Connection.h Configuration.h Reader.h Files.h
	gcc -c Connection.c $(CFLAGS)
Exit.o: Exit.c Exit.h Connection.h
	gcc -c Exit.c $(CFLAGS)
Files.o: Files.c Files.h Date.h Time.h
	gcc -c Files.c $(CFLAGS)
Time.o: Time.c Time.h
	gcc -c Time.c $(CFLAGS)
Date.o: Date.c Date.h
	gcc -c Date.c $(CFLAGS)
$(OUT): $(OBJ)
	gcc $(OBJ) -o $(OUT) $(CFLAGS)
clean:
	rm *.o
