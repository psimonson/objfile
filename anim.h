/*
 * anim.h - Header file for animation frames using objfile loader.
 *
 * Author: Philip R. Simonson
 * Date  : 07/24/2020
 *
 *********************************************************************
 */

#ifndef _ANIM_H_
#define _ANIM_H_

#include "object.h" // Include objfile loader header file.

/* Load animation from files. */
struct objfile **load_anim(const char *dir, const char *anim);

#endif
