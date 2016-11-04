CC=g++

TARGET=main

CFLAGS=-I/usr/local/include/ -g -O2

LIBS=-lm -ldl

DEPS = tlib.h

OBJ = main.o h264dec.o 


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o $(TARGET)
