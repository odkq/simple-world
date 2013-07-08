#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#include <string.h>

#include <textures.h>
#include <graphic.h>

#define WINDOW_TITLE "Simple World"

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

struct camera {
	float x, y, z;
};

struct graphic {
	int width, height;
	int window;
	struct camera c;
};

static struct graphic gl;

static void gl_resize(int, int);
static void gl_redraw(void);
static void gl_keypress(unsigned char, int, int);

static void gl_process(int id);

unsigned char keys[256];

float pos[3] = { -1.0f, 0.0f, -6.0f };

int tex = TEX_TEST1;

/* opengl initialization boilerplate */
int gl_start(int argc, char **argv)
{
	gl.width = 1920;
	gl.height = 1080;
	gl.c.x = -1.0f;
	gl.c.y = 0.0f;
	gl.c.z = -6.0f;

	glutInit(&argc, argv);  /* parameters for X11 are processed by this */
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(gl.width, gl.height);
	glutInitWindowPosition(0, 0);
	gl.window = glutCreateWindow(WINDOW_TITLE);
	glutDisplayFunc(&gl_redraw);
	glutFullScreen(); 
	glutIdleFunc(&gl_redraw);
	glutReshapeFunc(&gl_resize);
	glutKeyboardFunc(&gl_keypress);
	textures_init();
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	/* reset the projection matrix (WTF?) */
	/* Adjust the aspect ratio */
	gluPerspective(45.0f,(GLfloat)gl.width/(GLfloat)gl.height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glutTimerFunc(1, gl_process, 0);
	memset((void *)keys, 0, 256);
	return argc;	/* Return argc with first non-x11 argument pointer */
}

static void gl_print_quad(float *coords, int tex)
{
	glBindTexture(GL_TEXTURE_2D, texture[tex]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(coords[0], coords[1],  coords[2]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(coords[3], coords[4],  coords[5]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(coords[6], coords[7],  coords[8]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(coords[9], coords[10],  coords[11]);
	glEnd();
}
static void gl_redraw(void)
{
	float quad[12] = {
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f,-1.0f, 0.0f,
		-1.0f,-1.0f, 0.0f };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// glTranslatef(-1.5f,0.0f,-6.0f);
	glTranslatef(pos[0], pos[1], pos[2]);
	gl_print_quad(quad, tex);
	glutSwapBuffers();
}
// int move(int axis,
/* Called each 10 mils */
static void gl_process(int id)
{
	float delta = 0.1f;

	if (keys['0'] == 1) {
		gl_end();
		exit(0);
	}
	if (keys['1'] == 1) {
		tex = TEX_TEST1;
		keys['1'] = 0;
	}
	if (keys['2'] == 1) {
		tex = TEX_TEST2;
		keys['2'] = 0;
	}
	if (keys['3'] == 1) {
		tex = TEX_TEST3;
		keys['3'] = 0;
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
	if (keys['q'] == 1) {
		pos[2] -= delta;
		keys['q'] = 0;
	}
	if (keys['e'] == 1) {
		pos[2] += delta;
		keys['e'] = 0;
	}
	glutTimerFunc(1, gl_process, 0);
}

/* symetrically finalization boilerplate */
void gl_end(void)
{
	glutDestroyWindow(gl.window);
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

void gl_loop(void)
{
	glutMainLoop();
}

