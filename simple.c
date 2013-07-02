#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#include <string.h>

#define WINDOW_TITLE "Simple World"
int window;

static int gl_start(int, char **);
static void gl_end(void);
static void gl_resize(int, int);
static void gl_redraw(void);
static void gl_keypress(unsigned char, int, int);

static void gl_process(int id);

unsigned char keys[256];

float pos[3] = { -1.0f, 0.0f, -100.0f };

/* opengl initialization boilerplate */
static int gl_start(int argc, char **argv)
{
	int w = 1920, h = 1080;

        glutInit(&argc, argv);  /* parameters for X11 are processed by this */
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
        glutInitWindowSize(w, h);
        glutInitWindowPosition(0, 0);
        window = glutCreateWindow(WINDOW_TITLE);
        glutDisplayFunc(&gl_redraw);
        glutFullScreen(); 
        glutIdleFunc(&gl_redraw);
        glutReshapeFunc(&gl_resize);
        glutKeyboardFunc(&gl_keypress);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	/* reset the projection matrix (WTF?) */
	/* Adjust the aspect ratio */
	gluPerspective(45.0f,(GLfloat)w/(GLfloat)h, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glutTimerFunc(1, gl_process, 0);
	
	memset((void *)keys, 0, 256);
	return argc;	/* Return argc with first non-x11 argument pointer */
}

static void gl_redraw(void)
{
	int i;
	float x, y, z;

	float quad[12] = {
		-10.0f, 10.0f, 0.0f,
		10.0f, 10.0f, 0.0f,
		10.0f,-10.0f, 0.0f,
		-10.0f,-10.0f, 0.0f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// glTranslatef(-1.5f,0.0f,-6.0f);
	glTranslatef(pos[0], pos[1], pos[2]);

	glBegin(GL_QUADS);
	for (i = 0; i < 12; i++) {
		switch (i % 3) {
		case 0:
			x = quad[i];
			break;
		case 1:
			y = quad[i];
			break;
		case 2:
			z = quad[i];
			glVertex3f(x, y, z);
			break;
		}
	}
	glEnd();

	glutSwapBuffers();
}

/* Called each 10 mils */
static void gl_process(int id)
{
	float delta = 1.0f;

	if (keys['0'] == 1) {
		gl_end();
		exit(0);
	}
	if (keys['w'] == 1) {
		pos[1] += delta;
		keys['w'] = 0;
	}
	if (keys['s'] == 1) {
		pos[1] -= delta;
		keys['s'] = 0;
	}
	if (keys['a'] == 1) {
		pos[0] -= delta;
		keys['a'] = 0;
	}
	if (keys['d'] == 1) {
		pos[0] += delta;
		keys['d'] = 0;
	}
	glutTimerFunc(1, gl_process, 0);
}

/* symetrically finalization boilerplate */
static void gl_end(void)
{
	glutDestroyWindow(window);
}

/* resize ... not yet since we are fullscreen */
static void gl_resize(int w, int h)
{
	return;
}

/* The function called whenever a key is pressed. */
static void gl_keypress(unsigned char k, int x, int y)
{
	keys[k] = 1;
}

int main(int argc, char **argv)
{
	argc = gl_start(argc, argv);
	glutMainLoop();
	return 0;
}

