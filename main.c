/*
 * main.c - Simple test program for testing objfile Wavefront object loader.
 *
 * Author: Philip R. Simonson
 * Date  : 06/19/2019
 *
 *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unused.h"
#include "object.h"
#include "vector.h"

#include "GL/freeglut.h"

#define FPS 60 // For regulating FPS

static struct objfile *obj, *obj2, *obj3, **anim1;
static int anim1_frame;

/* Clean up all memory resources.
 */
void cleanup()
{
	extern struct objfile *obj, *obj2, *obj3, **anim1;
	destroy_object(obj);
	destroy_object(obj2);
	destroy_object(obj3);
	destroy_anim(anim1);
}
/* What to do when the window's size changes.
 */
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
/* Render the scene.
 */
void render_scene()
{
	extern struct objfile *obj, *obj2, *obj3, **anim1;

	if(anim1_frame >= (int)vector_size(anim1))
		anim1_frame = 0;

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
	glTranslatef(-5.0f, 0.0f, -20.0f);
	draw_object(obj3);
	glTranslatef(-3.0f, 0.0f, 0.0f);

	/* draw anim1 */
	draw_anim(anim1, anim1_frame);

	glutSwapBuffers();
}
/* Timer function for animation.
 */
void timer(int UNUSED(timer_id))
{
	anim1_frame++;
	glutTimerFunc(100, timer, 0);
	glutPostRedisplay();
}
/* Initialize freeglut and return 0 on success.
 */
int init_glut(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	if(!glutCreateWindow("OBJFILE v0.01")) return 1;
	glutTimerFunc(100, timer, 0);
	glutDisplayFunc(render_scene);
	glutIdleFunc(render_scene);
	glutReshapeFunc(change_size);
	return 0;
}
/* Entry point for test program.
 */
int main(int argc, char **argv)
{
	extern struct objfile *obj, *obj2, *obj3, **anim1;

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
		cleanup();
		return 1;
	}
	obj3 = init_object();
	if(!obj3) return 1;
	if(load_object(obj3, "test3.obj") < 0) {
		fprintf(stderr, "Error: Cannot load object...\n");
		cleanup();
		return 1;
	}
//	print_object(obj);
//	print_object(obj2);
//	print_object(obj3);
	anim1 = load_anim("./anim", "cube_anim1");
	if(anim1 == NULL) {
		fprintf(stderr, "Error: Cannot load anim1...\n");
		cleanup();
		return 1;
	}
	glutMainLoop();
	cleanup();
	return 0;
}
