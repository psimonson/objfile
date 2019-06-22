#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unused.h"
#include "object.h"
#include "vector.h"
#include "GL/freeglut.h"

static int mybox;

void cleanup()
{
	glDeleteLists(mybox, 1);
}

void change_size(int w, int h)
{
	float ratio;

	if(h == 0)
		h = 1;

	ratio = 1.0*w/h;

	glClearColor(0.0f, 0.0f, 0.7f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
}

void render_scene()
{
	extern int mybox;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	/* draw object */
	draw_object(mybox);

	glutSwapBuffers();
}

int init_glut(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	if(!glutCreateWindow("OBJFILE v0.01")) return 1;
	glutDisplayFunc(render_scene);
	glutIdleFunc(render_scene);
	glutReshapeFunc(change_size);
	glutCloseFunc(cleanup);
	return 0;
}

int main(int argc, char **argv)
{
	struct objfile *obj;

	if(init_glut(argc, argv))
		return 1;

	obj = load_object("test.obj");
	if(!obj) return 1;
	mybox = make_object(obj);
	print_object(obj);
	destroy_object(obj);
	glutMainLoop();
	return 0;
}
