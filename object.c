#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "object.h"
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
	VECTOR_INIT(obj->vertices);
	VECTOR_INIT(obj->normals);
	while(readf_file(&file, "%s", buf) != EOF) {
		if(!strcmp(buf, "v")) {
			float *v1, *v2, *v3;
			v1 = malloc(sizeof(float));
			if(!v1) { destroy_object(obj); return NULL; }
			v2 = malloc(sizeof(float));
			if(!v2) { destroy_object(obj); return NULL; }
			v3 = malloc(sizeof(float));
			if(!v3) { destroy_object(obj); return NULL; }
			readf_file(&file, "%f %f %f", v1, v2, v3);
			VECTOR_ADD(obj->vertices, v1);
			VECTOR_ADD(obj->vertices, v2);
			VECTOR_ADD(obj->vertices, v3);
		} else if(!strcmp(buf, "vn")) {
			float *v1, *v2, *v3;
			v1 = malloc(sizeof(float));
			if(!v1) { destroy_object(obj); return NULL; }
			v2 = malloc(sizeof(float));
			if(!v2) { destroy_object(obj); return NULL; }
			v3 = malloc(sizeof(float));
			if(!v3) { destroy_object(obj); return NULL; }
			readf_file(&file, "%f %f %f", v1, v2, v3);
			VECTOR_ADD(obj->normals, v1);
			VECTOR_ADD(obj->normals, v2);
			VECTOR_ADD(obj->normals, v3);
		}
	}
	close_file(&file);
	return obj;
}
/**
 * @brief Print object data.
 */
void print_object(struct objfile *obj)
{
	int i;
	for(i=0; i < VECTOR_TOTAL(obj->vertices); i+=3) {
		printf("Vertex Group %d: %f %f %f\n", i,
			*VECTOR_GET(obj->vertices, float*, i),
			*VECTOR_GET(obj->vertices, float*, i+1),
			*VECTOR_GET(obj->vertices, float*, i+2));
	}
	for(i=0; i < VECTOR_TOTAL(obj->normals); i+=3) {
		printf("Normal Group %d: %f %f %f\n", i,
			*VECTOR_GET(obj->normals, float*, i),
			*VECTOR_GET(obj->normals, float*, i+1),
			*VECTOR_GET(obj->normals, float*, i+2));
	}
}
/**
 * @brief Destroy given object structure.
 */
void destroy_object(struct objfile *obj)
{
	int i;

	for(i=0; i < VECTOR_TOTAL(obj->vertices); i++)
		free(VECTOR_GET(obj->vertices, float*, i));
	VECTOR_FREE(obj->vertices);
	for(i=0; i < VECTOR_TOTAL(obj->normals); i++)
		free(VECTOR_GET(obj->normals, float*, i));
	VECTOR_FREE(obj->normals);
	memset(obj, 0, sizeof(struct objfile));
	free(obj);
}
