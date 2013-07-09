CFLAGS = -Wall -I.
LIBS = -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm 
OBJS = simple.o graphic.o

# "Compress" textures by default
COMPRESS ?= true

all: simple-world


textures.h: data/test1.bmp data/test2.bmp data/test3.bmp
ifeq ($(COMPRESS), true)
	python generate_textures.py --compress textures.h $?
else
	python generate_textures.py textures.h $?
endif

graphic.o: graphic.c textures.h
	$(CC) -c $(CFLAGS) graphic.c -o graphic.o

simple.o: simple.c
	$(CC) -c $(CFLAGS) simple.c -o simple.o

simple-world: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o simple-world $(LIBS)
clean:
	-rm simple-world
	-rm textures.h
	-rm *.o
