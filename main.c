#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unused.h"
#include "object.h"
#include "vector.h"
#include "GL/glew.h"
#include "GL/glut.h"

static struct objfile *obj;

void render_scene(void)
{
	extern struct objfile *obj;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(vector_size(obj->vertices), GL_FLOAT, 0, obj->vertices);
	glDisableClientState(GL_VERTEX_ARRAY);

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
	return 0;
}

int main(int argc, char **argv)
{
	extern struct objfile *obj;

	if(init_glut(argc, argv))
		return 1;

	obj = load_object("test.obj");
	if(!obj) return 1;
	glutMainLoop();
	destroy_object(obj);
	return 0;
}
