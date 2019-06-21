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
	obj->vertices = NULL;
	obj->normals = NULL;
	obj->faces = NULL;
	while(readf_file(&file, "%s", buf) != EOF) {
		if(!strcmp(buf, "v")) {
			float v1, v2, v3;
			readf_file(&file, "%f %f %f", &v1, &v2, &v3);
			vector_push_back(obj->vertices, v1);
			vector_push_back(obj->vertices, v2);
			vector_push_back(obj->vertices, v3);
		} else if(!strcmp(buf, "vn")) {
			float v1, v2, v3;
			readf_file(&file, "%f %f %f", &v1, &v2, &v3);
			vector_push_back(obj->normals, v1);
			vector_push_back(obj->normals, v2);
			vector_push_back(obj->normals, v3);
		} else if(!strcmp(buf, "f")) {
			int v1, v2, v3, v4, v5, v6, v7, v8;
			readf_file(&file, "%d//%d %d//%d %d//%d %d//%d",
				&v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8);
			vector_push_back(obj->faces, v1);
			vector_push_back(obj->faces, v2);
			vector_push_back(obj->faces, v3);
			vector_push_back(obj->faces, v4);
			vector_push_back(obj->faces, v5);
			vector_push_back(obj->faces, v6);
			vector_push_back(obj->faces, v7);
			vector_push_back(obj->faces, v8);
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
	size_t i;
	printf("Below is all the data... (Vertices, Normals, Faces).\n"
		"In that order.\n"
		"=====================================================\n");
	for(i=0; i < vector_size(obj->vertices); i+=3) {
		printf("%f %f %f\n", obj->vertices[i],
			obj->vertices[i+1], obj->vertices[i+2]);
	}
	printf("=====================================================\n");
	for(i=0; i < vector_size(obj->vertices); i+=3) {
		printf("%f %f %f\n", obj->vertices[i],
			obj->vertices[i+1], obj->vertices[i+2]);
	}
	printf("=====================================================\n");
	for(i=0; i < vector_size(obj->faces); i+=8) {
		printf("%d//%d %d//%d %d//%d %d//%d\n", obj->faces[i],
			obj->faces[i+1], obj->faces[i+2],
			obj->faces[i+3], obj->faces[i+4],
			obj->faces[i+5], obj->faces[i+6],
			obj->faces[i+7]);
	}
}
/**
 * @brief Destroy given object structure.
 */
void destroy_object(struct objfile *obj)
{
	vector_free(obj->vertices);
	vector_free(obj->normals);
	vector_free(obj->faces);
	memset(obj, 0, sizeof(struct objfile));
	free(obj);
}
