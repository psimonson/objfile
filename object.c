/**
 * @file object.c
 * @author Philip R. Simonson
 * @date 19 June 2019
 * @brief Blender wavefront OBJ loader.
 *
 * @details This is a simple object file format loader for
 * Wavefront OBJ files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>

#include "bitmap.h"
#include "object.h"
#include "vector.h"
#include "file.h"
#include "unused.h"

/* --------------------------- Helper Functions -------------------------- */

/**
 * @brief Count number of chars in string that occured.
 */
int strichr(const char *s, int ch)
{
	int i;
	for(i=0; *s; s++)
		if(*s == ch)
			i++;
	return i;
}

/* --------------------------- Object Functions -------------------------- */

/**
 * @brief Create object and set to default values.
 */
struct objfile *init_object(void)
{
	struct objfile *obj;
	obj = (struct objfile*)malloc(sizeof(struct objfile));
	if(!obj) {
		fprintf(stderr, "Error: Cannot create object, out of memory.\n");
		return NULL;
	}
	obj->ismat = obj->istex = obj->isnorm = 0;
	obj->v = obj->vn = NULL;
	obj->t = obj->l = NULL;
	obj->f = NULL;
	return obj;
}
/**
 * @brief Generate a GL list for drawing.
 */
static unsigned int make_object(struct objfile *obj)
{
	unsigned int unique_number;
	size_t i;

	/* Generate an object list for drawing later. */
	unique_number = glGenLists(1);
	glNewList(unique_number, GL_COMPILE);
	for(i=0; i < vector_size(obj->f); i++) {
		if(obj->f[i].four) {
			glBegin(GL_QUADS);
			glNormal3f(obj->vn[obj->f[i].num-1].x, obj->vn[obj->f[i].num-1].y,
				obj->vn[obj->f[i].num-1].z);
			glVertex3f(obj->v[obj->f[i].face.f1-1].x,
				obj->v[obj->f[i].face.f1-1].y,
				obj->v[obj->f[i].face.f1-1].z);
			glVertex3f(obj->v[obj->f[i].face.f2-1].x,
				obj->v[obj->f[i].face.f2-1].y,
				obj->v[obj->f[i].face.f2-1].z);
			glVertex3f(obj->v[obj->f[i].face.f3-1].x,
				obj->v[obj->f[i].face.f3-1].y,
				obj->v[obj->f[i].face.f3-1].z);
			glVertex3f(obj->v[obj->f[i].face.f4-1].x,
				obj->v[obj->f[i].face.f4-1].y,
				obj->v[obj->f[i].face.f4-1].z);
			glEnd();
		} else {
			glBegin(GL_TRIANGLES);
			glNormal3f(obj->vn[obj->f[i].num-1].x, obj->vn[obj->f[i].num-1].y,
				obj->vn[obj->f[i].num-1].z);
			glVertex3f(obj->v[obj->f[i].face.f1-1].x,
				obj->v[obj->f[i].face.f1-1].y,
				obj->v[obj->f[i].face.f1-1].z);
			glVertex3f(obj->v[obj->f[i].face.f2-1].x,
				obj->v[obj->f[i].face.f2-1].y,
				obj->v[obj->f[i].face.f2-1].z);
			glVertex3f(obj->v[obj->f[i].face.f3-1].x,
				obj->v[obj->f[i].face.f3-1].y,
				obj->v[obj->f[i].face.f3-1].z);
			glEnd();
		}
	}
	glEndList();
	return unique_number;
}
/**
 * @brief Load a texture from a filename.
 */
static unsigned int load_texture(const char *filename)
{
	unsigned int tex_id;
	Bitmap *bmp;

	bmp = load_bitmap(filename);
	if(!bmp) return (0xff << 24) + 1;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	/* Map the image to the texture */
	glTexImage2D(GL_TEXTURE_2D,
			0,	/* 0 for now */
			GL_RGB, /* Format OpenGL uses for textures */
			bmp->info.width,
			bmp->info.height,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			bmp->data);
	destroy_bitmap(bmp);
	return tex_id;
}
/**
 * @brief Create object from file.
 */
int load_object(struct objfile *obj, const char *filename)
{
	file_t file;
	char buf[256];

	UNUSED(load_texture(NULL));
	init_file(&file);
	open_file(&file, filename, "rt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY) {
		fprintf(stderr, "Error: %s\n", get_error_file(&file));
		return 1;
	}
	while(readf_file(&file, "%s", buf) != EOF) {
		if(!strcmp(buf, "v")) {
			struct vec3 v;
			readf_file(&file, "%f %f %f", &v.x, &v.y, &v.z);
			vector_push_back(obj->v, v);
		} else if(!strcmp(buf, "vn")) {
			struct vec3 v;
			readf_file(&file, "%f %f %f", &v.x, &v.y, &v.z);
			vector_push_back(obj->vn, v);
		} else if(!strcmp(buf, "f")) {
			struct face f;
			if(gets_file(&file, buf, sizeof(buf)) == NULL)
				continue;
			if(strichr(buf, ' ') == 4) {
				f.four = 1;
				if(strstr(buf, "//") != NULL) {
					sscanf(buf,
					"%d//%d %d//%d %d//%d %d//%d",
					&f.face.f1, &f.num, &f.face.f2,
					&f.num, &f.face.f3, &f.num,
					&f.face.f4, &f.num);
					f.tex.t1=f.tex.t2=f.tex.t3=f.tex.t4=0;
					vector_push_back(obj->f, f);
				} else if(strstr(buf, "/") != NULL) {
					sscanf(buf,
					"%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&f.face.f1, &f.tex.t1, &f.num,
					&f.face.f2, &f.tex.t2, &f.num,
					&f.face.f3, &f.tex.t3, &f.num,
					&f.face.f4, &f.tex.t4, &f.num);
					vector_push_back(obj->f, f);
				} else {
					sscanf(buf,
					"%d %d %d %d",
					&f.face.f1, &f.face.f2, &f.face.f3,
					&f.face.f4);
					f.tex.t1=f.tex.t2=f.tex.t3=f.tex.t4=0;
					f.num=0;
					vector_push_back(obj->f, f);
				}
			} else {
				f.four = 0;
				if(strstr(buf, "//") != NULL) {
					sscanf(buf, "%d//%d %d//%d %d//%d",
					&f.face.f1, &f.num, &f.face.f2,
					&f.num, &f.face.f3, &f.num);
					f.face.f4 = 0;
					f.tex.t1=f.tex.t2=f.tex.t3=f.tex.t4=0;
					vector_push_back(obj->f, f);
				} else if(strstr(buf, "/") != NULL) {
					sscanf(buf,
					"%d/%d/%d %d/%d/%d %d/%d/%d",
					&f.face.f1, &f.tex.t1, &f.num,
					&f.face.f2, &f.tex.t2, &f.num,
					&f.face.f3, &f.tex.t3, &f.num);
					f.face.f4 = 0;
					f.tex.t4 = 0;
					vector_push_back(obj->f, f);
				} else {
					sscanf(buf,
					"%d %d %d",
					&f.face.f1, &f.face.f2, &f.face.f3);
					f.tex.t1=f.tex.t2=f.tex.t3=f.tex.t4=0;
					f.face.f4 = f.num = 0;
					vector_push_back(obj->f, f);
				}
			}
		}
	}
	close_file(&file);
	vector_push_back(obj->l, make_object(obj));
	return 0;
}
/**
 * @brief Draw object to screen.
 */
void draw_object(int id)
{
	glCallList(id);
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
	for(i=0; i < vector_size(obj->v); i++) {
		printf("%f %f %f\n", obj->v[i].x,
			obj->v[i].y, obj->v[i].z);
	}
	printf("=====================================================\n");
	for(i=0; i < vector_size(obj->vn); i++) {
		printf("%f %f %f\n", obj->vn[i].x,
			obj->vn[i].y, obj->vn[i].z);
	}
	printf("=====================================================\n");
	for(i=0; i < vector_size(obj->f); i++) {
		if(obj->f[i].four) {
			if(obj->f[i].tex.t1 == 0 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d//%d %d//%d %d//%d %d//%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].num,
				obj->f[i].face.f4, obj->f[i].num);
			} else if(obj->f[i].tex.t1 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].tex.t1,
				obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].tex.t2,
				obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].tex.t3,
				obj->f[i].num,
				obj->f[i].face.f4, obj->f[i].tex.t4,
				obj->f[i].num);
			} else {
				printf("Quad: %d\n"
				"%d %d %d %d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].face.f2,
				obj->f[i].face.f3, obj->f[i].face.f4);
			}
		} else {
			if(obj->f[i].tex.t1 == 0 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d//%d %d//%d %d//%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].num);
			} else if(obj->f[i].tex.t1 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d/%d/%d %d/%d/%d %d/%d/%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].tex.t1,
				obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].tex.t2,
				obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].tex.t3,
				obj->f[i].num);
			} else {
				printf("Quad: %d\n"
				"%d %d %d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].face.f2,
				obj->f[i].face.f3);
			}
		}
	}
	printf("=====================================================\n");
}
/**
 * @brief Destroy given object structure.
 */
void destroy_object(struct objfile *obj)
{
	vector_free(obj->v);
	vector_free(obj->vn);
	vector_free(obj->f);
	vector_free(obj->t);
	vector_free(obj->l);
	memset(obj, 0, sizeof(struct objfile));
	free(obj);
}

