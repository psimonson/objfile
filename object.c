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
	VECTOR_INIT(obj->faces);
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
		} else if(!strcmp(buf, "f")) {
			int *v1, *v2, *v3, *v4, *v5, *v6, *v7, *v8;
			v1 = malloc(sizeof(int));
			if(!v1) { destroy_object(obj); return NULL; }
			v2 = malloc(sizeof(int));
			if(!v2) { destroy_object(obj); return NULL; }
			v3 = malloc(sizeof(int));
			if(!v3) { destroy_object(obj); return NULL; }
			v4 = malloc(sizeof(int));
			if(!v4) { destroy_object(obj); return NULL; }
			v5 = malloc(sizeof(int));
			if(!v5) { destroy_object(obj); return NULL; }
			v6 = malloc(sizeof(int));
			if(!v6) { destroy_object(obj); return NULL; }
			v7 = malloc(sizeof(int));
			if(!v7) { destroy_object(obj); return NULL; }
			v8 = malloc(sizeof(int));
			if(!v8) { destroy_object(obj); return NULL; }
			readf_file(&file, "%d//%d %d//%d %d//%d %d//%d",
				v1, v2, v3, v4, v5, v6, v7, v8);
			VECTOR_ADD(obj->faces, v1);
			VECTOR_ADD(obj->faces, v2);
			VECTOR_ADD(obj->faces, v3);
			VECTOR_ADD(obj->faces, v4);
			VECTOR_ADD(obj->faces, v5);
			VECTOR_ADD(obj->faces, v6);
			VECTOR_ADD(obj->faces, v7);
			VECTOR_ADD(obj->faces, v8);
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
	printf("==============================================\n");
	for(i=0; i < VECTOR_TOTAL(obj->vertices); i+=3) {
		printf("Vertex Group %d: %f %f %f\n", i,
			*VECTOR_GET(obj->vertices, float*, i),
			*VECTOR_GET(obj->vertices, float*, i+1),
			*VECTOR_GET(obj->vertices, float*, i+2));
	}
	printf("==============================================\n");
	for(i=0; i < VECTOR_TOTAL(obj->normals); i+=3) {
		printf("Normal Group %d: %f %f %f\n", i,
			*VECTOR_GET(obj->normals, float*, i),
			*VECTOR_GET(obj->normals, float*, i+1),
			*VECTOR_GET(obj->normals, float*, i+2));
	}
	printf("==============================================\n");
	for(i=0; i < VECTOR_TOTAL(obj->faces); i+=8) {
		printf("Faces Group %d: %d//%d %d//%d %d//%d %d//%d\n", i,
			*VECTOR_GET(obj->faces, int*, i),
			*VECTOR_GET(obj->faces, int*, i+1),
			*VECTOR_GET(obj->faces, int*, i+2),
			*VECTOR_GET(obj->faces, int*, i+3),
			*VECTOR_GET(obj->faces, int*, i+4),
			*VECTOR_GET(obj->faces, int*, i+5),
			*VECTOR_GET(obj->faces, int*, i+6),
			*VECTOR_GET(obj->faces, int*, i+7));
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
	for(i=0; i < VECTOR_TOTAL(obj->faces); i++)
		free(VECTOR_GET(obj->faces, int*, i));
	VECTOR_FREE(obj->faces);
	memset(obj, 0, sizeof(struct objfile));
	free(obj);
}
