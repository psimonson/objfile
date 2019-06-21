/**
 * @file vector.c
 * @author Philip R. Simonson
 * @date 20 June 2019
 * @brief Main implementation source file for a simple vector.
 *
 * @details Simple Implementation of a vector class in pure C.
 * Very simple, not much error checking though will add later.
 */

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

/**
 * @brief Initialize a vector structure.
 */
void vector_init(vector_t *v)
{
	v->capacity = VECTOR_INIT_CAPACITY;
	v->total = 0;
	v->items = malloc(sizeof(void*)*v->capacity);
}
/**
 * @brief Get current total size of given vector.
 */
int vector_total(vector_t *v)
{
	return v->total;
}
/**
 * @brief Resize vector to new size.
 */
static void vector_resize(vector_t *v, int capacity)
{
	void **items;

	#ifndef NDEBUG
	printf("vector_resize: %d to %d\n", v->capacity, capacity);
	#endif

	items = realloc(v->items, sizeof(void*)*capacity);
	if(items) {
		v->items = items;
		v->capacity = capacity;
	}
}
/**
 * @brief Add element to given vector.
 */
void vector_add(vector_t *v, void *item)
{
	if(v->capacity == v->total)
		vector_resize(v, v->capacity*2);
	v->items[v->total++] = item;
}
/**
 * @brief Set element at id in given vector.
 */
void vector_set(vector_t *v, int index, void *item)
{
	if(index >= 0 && index < v->total)
		v->items[index] = item;
}
/**
 * @brief Get element at id in given vector.
 */
void *vector_get(vector_t *v, int index)
{
	if(index >= 0 && index < v->total)
		return v->items[index];
	return NULL;
}
/**
 * @brief Delete an element at id in given vector.
 */
void vector_delete(vector_t *v, int index)
{
	int i;

	if(index < 0 || index >= v->total)
		return;

	v->items[index] = NULL;
	for(i=index; i<v->total-1; i++) {
		v->items[i] = v->items[i+1];
		v->items[i+1] = NULL;
	}
	v->total--;

	if(v->total > 0 && v->total == v->capacity/4)
		vector_resize(v, v->capacity/2);
}
/**
 * @brief Free entire given vector.
 */
void vector_free(vector_t *v)
{
	free(v->items);
}
