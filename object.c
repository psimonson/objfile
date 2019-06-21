#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>

#include "object.h"
#include "vector.h"
#include "file.h"
#include "unused.h"

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
			readf_file(&file, "%d//%d %d//%d %d//%d %d//%d",
				&f.f1, &f.f2, &f.f3, &f.f4,
				&f.f5, &f.f6, &f.f7, &f.f8);
			vector_push_back(obj->f, f);
		}
	}
	close_file(&file);
	return obj;
}
/**
 * @brief Draw object to screen.
 */
void draw_object(struct objfile *obj)
{
	UNUSED(obj);
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
		printf("%d//%d %d//%d %d//%d %d//%d\n",
			obj->f[i].f1, obj->f[i].f2,
			obj->f[i].f3, obj->f[i].f4,
			obj->f[i].f5, obj->f[i].f6,
			obj->f[i].f7, obj->f[i].f8);
	}
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
