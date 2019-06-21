/**
 * @file vector.h
 * @author Philip R. Simonson
 * @date 20 June 2019
 * @brief Header file for a very simple vector implementation.
 * @details
 *
 * This is a vector implementation like that of the C++ vector
 * implementation.
 */

#ifndef PRS_VECTOR_H
#define PRS_VECTOR_H

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VECTOR_INIT_CAPACITY 4

#define VECTOR_INIT(vec) vector_init(&vec)
#define VECTOR_MAKE(type, name) name = malloc(sizeof(type)); if(!name) NULL
#define VECTOR_ADD(vec, item) vector_add(&vec, (void*)item)
#define VECTOR_SET(vec, id, item) vector_set(&vec, id, (void*)item)
#define VECTOR_GET(vec, type, id) (type)vector_get(&vec, id)
#define VECTOR_DELETE(vec, id) vector_delete(&vec, id)
#define VECTOR_TOTAL(vec) vector_total(&vec)
#define VECTOR_FREE(vec) vector_free(&vec)

typedef struct vector {
	void **items;
	int capacity;
	int total;
} vector_t;

PRS_EXPORT void vector_init(vector_t*);
PRS_EXPORT int vector_total(vector_t*);
PRS_EXPORT void vector_add(vector_t*, void*);
PRS_EXPORT void vector_set(vector_t*, int, void*);
PRS_EXPORT void *vector_get(vector_t*, int);
PRS_EXPORT void vector_delete(vector_t*, int);
PRS_EXPORT void vector_free(vector_t*);

#ifdef __cplusplus
}
#endif

#endif
