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
#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif

struct objfile {
	char name[64];
	vector_t vertices;
	vector_t normals;
};

PRS_EXPORT struct objfile *load_object(const char*);
PRS_EXPORT void destroy_object(struct objfile*);
PRS_EXPORT void print_object(struct objfile*);

#ifdef __cplusplus
}
#endif

#endif
