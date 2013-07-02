CFLAGS = -Wall
LIBS = -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm 
simple-world: simple.o
	$(CC) $(CFLAGS) $< -o simple-world $(LIBS)
clean:
	-rm simple-world
	-rm *.o
