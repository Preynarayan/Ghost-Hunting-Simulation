CC = gcc
CFLAGS = -Wall -Wextra -pthread -g 
OBJDIR = obj

main: main.o house.o utils.o logger.o ghost.o room.o hunters.o evidence.o
	$(CC) $(CFLAGS) -o main ./$(OBJDIR)/evidence.o ./$(OBJDIR)/main.o ./$(OBJDIR)/house.o ./$(OBJDIR)/utils.o ./$(OBJDIR)/logger.o ./$(OBJDIR)/ghost.o ./$(OBJDIR)/room.o ./$(OBJDIR)/hunters.o

main.o: main.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c main.c -o ./$(OBJDIR)/main.o

house.o: house.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c house.c -o ./$(OBJDIR)/house.o

utils.o: utils.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c utils.c -o ./$(OBJDIR)/utils.o	

logger.o: logger.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c logger.c -o ./$(OBJDIR)/logger.o	

ghost.o: ghost.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c ghost.c -o ./$(OBJDIR)/ghost.o	

room.o: room.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c room.c -o ./$(OBJDIR)/room.o

hunters.o: hunters.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c hunters.c -o ./$(OBJDIR)/hunters.o

evidence.o: evidence.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c evidence.c -o ./$(OBJDIR)/evidence.o

clean:
	rm -rf obj
	rm -f *.o main

temp: main
	./main
	$(MAKE) clean