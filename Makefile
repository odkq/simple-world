CFLAGS = -Wall -I.
LIBS = -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm 
OBJS = simple.o texture.o

all: simple-world

textures.h: data/test1.bmp data/test2.bmp data/test3.bmp
	python generate_textures.py textures.h $?

simple.o: simple.c textures.h
	$(CC) -c $(CFLAGS) simple.c -o simple.o

simple-world: simple.o
	$(CC) $(CFLAGS) $< -o simple-world $(LIBS)
clean:
	-rm simple-world
	-rm textures.h
	-rm *.o
