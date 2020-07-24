/*
 * anim.h - Header file for animation frames using objfile loader.
 *
 * Author: Philip R. Simonson
 * Date  : 07/24/2020
 *
 *********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "object.h"
#include "vector.h"

/* Convert anim name to object name.
 */
static char *get_name(const char *dir_name, const char *anim_name)
{
	static char name[512];
	struct dirent *p;
	static DIR *dir = NULL;

	// Start processing directory
	if(dir == NULL) {
		errno = 0;
		if((dir = opendir((dir_name != NULL ? dir_name : "."))) == NULL) {
			fprintf(stderr, "Error: %s\n", strerror(errno));
			return NULL;
		}
	}

	// Process directory
	errno = 0;
	if((p = readdir(dir)) != NULL) {
		if((strcmp(p->d_name, ".") && strcmp(p->d_name, "..")) != 0) {
			memset(name, 0, sizeof(name));
			if(strstr(p->d_name, ".obj") && strstr(p->d_name, anim_name))
				strncpy(name, p->d_name, strlen(p->d_name));
		}
	} else {
		closedir(dir);
		return NULL;
	}
	return name;
}
/* Load an animation from it's name.
 */
struct objfile **load_anim(const char *dir, const char *anim_name)
{
//	struct objfile **anim = NULL;
	char *name = NULL;

	while((name = get_name(dir, anim_name)) != NULL)
		printf("%s\n", name);

	return NULL;
}
