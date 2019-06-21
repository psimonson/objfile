#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unused.h"
#include "object.h"
#include "vector.h"
#include "GL/glew.h"
#include "GL/glut.h"

static struct objfile *obj;

void change_size(int w, int h)
{
	float ratio;

	if(h == 0)
		h = 1;

	ratio = 1.0*w/h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void render_scene(void)
{
	extern struct objfile *obj;

	glClearColor(0.0f, 0.0f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	/* draw object */
	draw_object(obj);

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
	return 0;
}

int main(int argc, char **argv)
{
	extern struct objfile *obj;

	if(init_glut(argc, argv))
		return 1;

	obj = load_object("test.obj");
	if(!obj) return 1;
	print_object(obj);
	glutMainLoop();
	destroy_object(obj);
	return 0;
}
