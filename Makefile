# Kompilator
CC = gcc

# Flagi kompilatora (-Wall pokazuje ostrzeżenia, -g dodaje informacje do debugowania)
CFLAGS = -Wall -g

# Biblioteki (-lm dla matematyki, -lpthread dla wątków)
LIBS = -lm -lpthread

# Plik wynikowy
TARGET = main.exe

# Lista plików źródłowych (możesz wpisać ręcznie lub użyć wildcard)
SRCS = main.c globals.c save.c signal_handlers.c initializers.c validations.c cash_manager.c
# Alternatywnie automatycznie wszystkie .c: SRCS = $(wildcard *.c)

# Zamiana .c na .o (pliki obiektowe)
OBJS = $(SRCS:.c=.o)

# Domyślny cel (uruchamiany po wpisaniu 'make')
all: $(TARGET)

# Linkowanie (łączenie plików .o w .exe)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# Kompilacja poszczególnych plików .c na .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Sprzątanie (usuwanie plików .o i .exe)
clean:
	rm -f $(OBJS) $(TARGET)