CFLAGS = -Wall -Wextra -lpthread -D_GNU_SOURCE
OUT = mctavish
OBJ = main.o Reader.o Configuration.o Connection.o Menu.o Exit.o
all: $(OUT)
main.o: main.c Configuration.h Connection.h Menu.h Exit.h
	gcc -c main.c $(CFLAGS)
Reader.o: Reader.c Reader.h
	gcc -c Reader.c $(CFLAGS)
Configuration.o: Configuration.c Configuration.h Reader.h
	gcc -c Configuration.c $(CFLAGS)
Connection.o: Connection.c Connection.h Configuration.h Reader.h Exit.h
	gcc -c Connection.c $(CFLAGS)
Menu.o: Menu.c Menu.h Connection.h Reader.h Exit.h
	gcc -c Menu.c $(CFLAGS)
Exit.o: Exit.c Exit.h Connection.h Configuration.h
	gcc -c Exit.c $(CFLAGS)
$(OUT): $(OBJ)
	gcc $(OBJ) -o $(OUT) $(CFLAGS)
clean:
	rm *.o
