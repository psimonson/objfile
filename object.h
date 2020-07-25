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
	unsigned int texture;
	int illum;
};

struct texcoord {
	float u, v;
};

struct objfile {
	struct vec3 *v;
	struct vec3 *vn;
	struct face *f;
	struct material *mat;
	struct texcoord *t;
	int l;
	char istex;
	char isnorm;
	char ismat;
};

#ifdef __cplusplus
extern "C" {
#endif

PRS_EXPORT struct objfile *init_object(void);
PRS_EXPORT int load_object(struct objfile *obj, const char*);
PRS_EXPORT void destroy_object(struct objfile*);
PRS_EXPORT void draw_object(struct objfile*);
PRS_EXPORT void print_object(struct objfile*);
PRS_EXPORT struct objfile **load_anim(const char *dir, const char *anim_name);
PRS_EXPORT void draw_anim(struct objfile **anim, int frame);
PRS_EXPORT void destroy_anim(struct objfile **anim);

#ifdef __cplusplus
}
#endif

#endif
