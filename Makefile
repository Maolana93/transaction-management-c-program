CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lsqlite3 -lcjson

TARGET = transaction_system
OBJS = main.o database.o account.o transaction.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean