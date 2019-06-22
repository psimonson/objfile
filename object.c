#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>

#include "object.h"
#include "vector.h"
#include "file.h"
#include "unused.h"

/* --------------------------- Helper Functions -------------------------- */

/**
 * @brief Count number of chars in string that occured.
 */
int strichr(const char *s, int ch)
{
	int i;
	for(i=0; *s; s++)
		if(*s == ch)
			i++;
	return i;
}

/* --------------------------- Object Functions -------------------------- */

/**
 * @brief Create object from file.
 */
struct objfile *load_object(const char *filename)
{
	struct objfile *obj;
	file_t file;
	char buf[256];

	init_file(&file);
	open_file(&file, filename, "rt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY) {
		fprintf(stderr, "Error: %s\n", get_error_file(&file));
		return NULL;
	}
	obj = (struct objfile*)malloc(sizeof(struct objfile));
	if(!obj) return NULL;
	obj->v = NULL;
	obj->vn = NULL;
	obj->f = NULL;
	while(readf_file(&file, "%s", buf) != EOF) {
		if(!strcmp(buf, "v")) {
			struct vec3 v;
			readf_file(&file, "%f %f %f", &v.x, &v.y, &v.z);
			vector_push_back(obj->v, v);
		} else if(!strcmp(buf, "vn")) {
			struct vec3 v;
			readf_file(&file, "%f %f %f", &v.x, &v.y, &v.z);
			vector_push_back(obj->vn, v);
		} else if(!strcmp(buf, "f")) {
			struct face f;
			if(gets_file(&file, buf, sizeof(buf)) == NULL)
				continue;
			if(strichr(buf, ' ') == 4) {
				f.four = 1;
				sscanf(buf, "%d//%d %d//%d %d//%d %d//%d",
					&f.face.f1, &f.num, &f.face.f2,
					&f.num, &f.face.f3, &f.num,
					&f.face.f4, &f.num);
				vector_push_back(obj->f, f);
			} else {
				f.four = 0;
				sscanf(buf, "%d//%d %d//%d %d//%d",
					&f.face.f1, &f.num, &f.face.f2,
					&f.num, &f.face.f3, &f.num);
				f.face.f4 = 0;
				vector_push_back(obj->f, f);
			}
		}
	}
	close_file(&file);
	return obj;
}
/**
 * @brief Generate a gl list for drawing.
 */
int make_object(struct objfile *obj)
{
	int unique_number;
	size_t i;

	/* Generate an object list for drawing later. */
	unique_number = glGenLists(1);
	glNewList(unique_number, GL_COMPILE);
	for(i=0; i < vector_size(obj->f); i++) {
		if(obj->f[i].four) {
			glBegin(GL_QUADS);
			glNormal3f(obj->vn[obj->f[i].num-1].x, obj->vn[obj->f[i].num-1].y,
				obj->vn[obj->f[i].num-1].z);
			glVertex3f(obj->v[obj->f[i].face.f1].x,
				obj->v[obj->f[i].face.f1].y,
				obj->v[obj->f[i].face.f1].z);
			glVertex3f(obj->v[obj->f[i].face.f2].x,
				obj->v[obj->f[i].face.f2].y,
				obj->v[obj->f[i].face.f2].z);
			glVertex3f(obj->v[obj->f[i].face.f3].x,
				obj->v[obj->f[i].face.f3].y,
				obj->v[obj->f[i].face.f3].z);
			glVertex3f(obj->v[obj->f[i].face.f4].x,
				obj->v[obj->f[i].face.f4].y,
				obj->v[obj->f[i].face.f4].z);
			glEnd();
		} else {
			glBegin(GL_TRIANGLES);
			glNormal3f(obj->vn[obj->f[i].num-1].x, obj->vn[obj->f[i].num-1].y,
				obj->vn[obj->f[i].num-1].z);
			glVertex3f(obj->v[obj->f[i].face.f1].x,
				obj->v[obj->f[i].face.f1].y,
				obj->v[obj->f[i].face.f1].z);
			glVertex3f(obj->v[obj->f[i].face.f2].x,
				obj->v[obj->f[i].face.f2].y,
				obj->v[obj->f[i].face.f2].z);
			glVertex3f(obj->v[obj->f[i].face.f3].x,
				obj->v[obj->f[i].face.f3].y,
				obj->v[obj->f[i].face.f3].z);
			glEnd();
		}
	}
	glEndList();
	return unique_number;
}
/**
 * @brief Draw object to screen.
 */
void draw_object(int id)
{
	glCallList(id);
}
/**
 * @brief Print object data.
 */
void print_object(struct objfile *obj)
{
	size_t i;
	printf("Below is all the data... (Vertices, Normals, Faces).\n"
		"In that order.\n"
		"=====================================================\n");
	for(i=0; i < vector_size(obj->v); i++) {
		printf("%f %f %f\n", obj->v[i].x,
			obj->v[i].y, obj->v[i].z);
	}
	printf("=====================================================\n");
	for(i=0; i < vector_size(obj->vn); i++) {
		printf("%f %f %f\n", obj->vn[i].x,
			obj->vn[i].y, obj->vn[i].z);
	}
	printf("=====================================================\n");
	for(i=0; i < vector_size(obj->f); i++) {
		if(obj->f[i].four) {
			printf("Quad: %d\n%d//%d %d//%d %d//%d %d//%d\n",
				obj->f[i].four, obj->f[i].face.f1, obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].num,
				obj->f[i].face.f4, obj->f[i].num);
		} else {
			printf("Quad: %d\n%d//%d %d//%d %d//%d\n",
				obj->f[i].four, obj->f[i].face.f1, obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].num);
		}
	}
	printf("=====================================================\n");
}
/**
 * @brief Destroy given object structure.
 */
void destroy_object(struct objfile *obj)
{
	vector_free(obj->v);
	vector_free(obj->vn);
	vector_free(obj->f);
	memset(obj, 0, sizeof(struct objfile));
	free(obj);
}
