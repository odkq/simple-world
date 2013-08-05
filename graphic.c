#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#include <string.h>

#include <textures.h>
#include <graphic.h>
#include <input.h>

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
static void gl_redraw2(void);
static void gl_keypress(unsigned char, int, int);
static void gl_special_keypress(int k, int xx, int yy);
static void gl_process(int id);
static void change_texture(int t);
static void move(int t);
static void quit(int t);

static void move_quad(float *quad, int c, float delta);

float pos[3] = { -1.0f, 0.0f, -6.0f };

int tex = TEX_TEST1;

// angle of rotation for the camera direction
float angle=0.0;
// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f;
// XZ position of the camera
float x=0.0f,z=5.0f;

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
	glutDisplayFunc(&gl_redraw2);
	glutFullScreen(); 
	glutIdleFunc(&gl_redraw2);
	glutReshapeFunc(&gl_resize);
	glutKeyboardFunc(&gl_keypress);
	glutSpecialFunc(&gl_special_keypress);
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

	input_register('1', change_texture, TEX_TEST1);
	input_register('2', change_texture, TEX_TEST2);
	input_register('3', change_texture, TEX_TEST3);
	input_register('w', move, 'w');
	input_register('s', move, 's');
	input_register('a', move, 'a');
	input_register('d', move, 'd');
	input_register('q', move, 'q');
	input_register('e', move, 'e');
	input_register('0', quit, 0);
    input_register(GLUT_KEY_LEFT + 0xff, move, GLUT_KEY_LEFT + 0xff);
    input_register(GLUT_KEY_RIGHT + 0xff, move, GLUT_KEY_RIGHT + 0xff);
    input_register(GLUT_KEY_UP + 0xff, move, GLUT_KEY_UP + 0xff);
    input_register(GLUT_KEY_DOWN + 0xff, move, GLUT_KEY_DOWN + 0xff);
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

static void drawSnowMan() {

    glColor3f(1.0f, 1.0f, 1.0f);

// Draw Body
    glTranslatef(0.0f ,0.75f, 0.0f);
    glutSolidSphere(0.75f,20,20);

// Draw Head
    glTranslatef(0.0f, 1.0f, 0.0f);
    glutSolidSphere(0.25f,20,20);

// Draw Eyes
    glPushMatrix();
    glColor3f(0.0f,0.0f,0.0f);
    glTranslatef(0.05f, 0.10f, 0.18f);
    glutSolidSphere(0.05f,10,10);
    glTranslatef(-0.1f, 0.0f, 0.0f);
    glutSolidSphere(0.05f,10,10);
    glPopMatrix();

// Draw Nose
    glColor3f(1.0f, 0.5f , 0.5f);
    glRotatef(0.0f,1.0f, 0.0f, 0.0f);
    glutSolidCone(0.08f,0.5f,10,2);
}

static void gl_redraw(void)
{
	int i;
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
	/* Some turtle graphics */
	for(i = 0; i < 3; i++) {
		move_quad(quad, 0, 2.0);
		gl_print_quad(quad, tex);
	}
	move_quad(quad, 1, 2.0);
	gl_print_quad(quad, tex);
	for(i = 0; i < 3; i++) {
		move_quad(quad, 0, -2.0);
		gl_print_quad(quad, tex);
	}
	move_quad(quad, 1, 2.0);
	gl_print_quad(quad, tex);
	for(i = 0; i < 3; i++) {
		move_quad(quad, 0, 2.0);
		gl_print_quad(quad, tex);
	}
    drawSnowMan();
	glutSwapBuffers();
}

void gl_redraw2(void)
{
    int i, j;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(  x, 1.0f, z,
            x+lx, 1.0f,  z+lz,
            0.0f, 1.0f,  0.0f);

        // Draw ground
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
        glVertex3f(-100.0f, 0.0f, -100.0f);
        glVertex3f(-100.0f, 0.0f,  100.0f);
        glVertex3f( 100.0f, 0.0f,  100.0f);
        glVertex3f( 100.0f, 0.0f, -100.0f);
    glEnd();

        // Draw 36 SnowMen
    for(i = -3; i < 3; i++)
        for(j=-3; j < 3; j++) {
            glPushMatrix();
            glTranslatef(i*10.0,0,j * 10.0);
            drawSnowMan();
            glPopMatrix();
        }

    glutSwapBuffers();
}


static void change_texture(int t)
{
	tex = t;
}

static void move_quad(float *quad, int c, float d)
{
	int i;

	int delta[3][4] =
		{{ 0, 3, 6, 9 },
		{ 1, 4, 7, 10 },
		{ 2, 5, 8, 11 }};

	for (i = 0; i < 4; i++) {
		quad[delta[c][i]] += d;
	}
}

static void move(int t)
{
	float delta = 0.1f;

	switch(t) {
		case 'w': pos[1] += delta; break;
		case 's': pos[1] -= delta; break;
		case 'a': pos[0] -= delta; break;
		case 'd': pos[0] += delta; break;
		case 'q': pos[2] -= delta; break;
		case 'e': pos[2] += delta; break;
        case (GLUT_KEY_LEFT + 0xff):
            angle -= 0.01f;
            lx = sin(angle);
            lz = -cos(angle);
            break;
        case (GLUT_KEY_RIGHT + 0xff):
            angle += 0.01f;
            lx = sin(angle);
            lz = -cos(angle);
            break;
        case (GLUT_KEY_UP + 0xff):
            x += lx * delta;
            z += lz * delta;
            break;
        case (GLUT_KEY_DOWN + 0xff):
            x -= lx * delta;
            z -= lz * delta;
            break;
        case '0': exit(0);
	}
}

static void quit(int t)
{
	gl_end();
	exit(0);
}

// int move(int axis,
/* Called each 10 mils */
static void gl_process(int id)
{
	input_poll();
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
	input_set(k);
}

static void gl_special_keypress(int k, int xx, int yy)
{
    input_set(k + 0xff);
}

void gl_loop(void)
{
	glutMainLoop();
}

