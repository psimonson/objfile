/**
 * @file object.h
 * @author Philip R. Simonson
 * @date 19 June 2019
 * @brief Blender wavefront OBJ loader.
 *
 * @details This is a simple object file format loader for
 * Wavefront OBJ files.
 */

#ifndef PRS_OBJECT_H
#define PRS_OBJECT_H

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

struct vec3 {
	float x;
	float y;
	float z;
};

struct face {
	char four;
	int num;
	int mat;
	struct {
		int f1, f2;
		int f3, f4;
	} face, tex;
};

struct material {
	char name[256];
	float alpha, ns, ni;
	float dif[3], amb[3], spec[3];
	int illum,texture;
	struct {
		float u, v;
	} tex;
};

struct objfile {
	struct vec3 *v;
	struct vec3 *vn;
	struct face *f;
	struct material *mat;
	unsigned int *t;
	unsigned int *l;
	char istex;
	char isnorm;
	char ismat;
};

PRS_EXPORT struct objfile *init_object(void);
PRS_EXPORT int load_object(struct objfile *obj, const char*);
PRS_EXPORT void destroy_object(struct objfile*);
PRS_EXPORT void draw_object(int id);
PRS_EXPORT void print_object(struct objfile*);

#ifdef __cplusplus
}
#endif

#endif
