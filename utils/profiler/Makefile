CC = gcc
CFLAGS = -O3 -Wall

all: profile

profile: main.c components/*.c components/*.h
	$(CC) $(CFLAGS) -o profile main.c components/*.c -I$(EMA_INSTALL_DIR)/include -L$(EMA_INSTALL_DIR)/lib -lEMA -lm

clean:
	rm -f profile
