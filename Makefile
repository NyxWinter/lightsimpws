CC = clang
CFLAGS = -lSDL2 -lm -O3

SRCS = src/main.c src/getPlane.c src/intersections.c 
OBJS = $(SRCS:.c=.o)
TARGET = project

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
