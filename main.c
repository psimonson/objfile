#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unused.h"
#include "object.h"
#include "GL/freeglut.h"

static struct objfile *obj,*obj2;

void cleanup()
{
	extern struct objfile *obj,*obj2;
	destroy_object(obj);
	destroy_object(obj2);
}

void change_size(int w, int h)
{
	const float col[] = {0.7,0.7,0.7,1.0};
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
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
}

void render_scene()
{
	extern struct objfile *obj,*obj2;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	/* draw object */
	glTranslatef(-5.0f, 0.0f, -20.0f);
	draw_object(obj);
	glTranslatef(10.0f, 0.0f, 10.0f);
	draw_object(obj2);
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
	extern struct objfile *obj,*obj2;

	if(init_glut(argc, argv))
		return 1;
	obj = init_object();
	if(!obj) return 1;
	if(load_object(obj, "test.obj") < 0) {
		fprintf(stderr, "Error: Cannot load object...\n");
		return 1;
	}
	obj2 = init_object();
	if(!obj2) return 1;
	if(load_object(obj2, "test2.obj") < 0) {
		fprintf(stderr, "Error: Cannot load object...\n");
		destroy_object(obj);
		return 1;
	}
/*	print_object(obj);*/
/*	print_object(obj2);*/
	glutMainLoop();
	cleanup();
	return 0;
}
