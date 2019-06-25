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
	obj->mat = NULL;
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
	int last;

	/* Generate an object list for drawing later. */
	last = -1;
	unique_number = glGenLists(1);
	glNewList(unique_number, GL_COMPILE);
	for(i=0; i < vector_size(obj->f); i++) {
		if(last != obj->f[i].mat && obj->ismat) {
			float dif[] = {obj->mat[obj->f[i].mat].dif[0],
				obj->mat[obj->f[i].mat].dif[1],
				obj->mat[obj->f[i].mat].dif[2], 1.0f};
			float amb[] = {obj->mat[obj->f[i].mat].amb[0],
				obj->mat[obj->f[i].mat].amb[1],
				obj->mat[obj->f[i].mat].amb[2], 1.0f};
			float spec[] = {obj->mat[obj->f[i].mat].spec[0],
				obj->mat[obj->f[i].mat].spec[1],
				obj->mat[obj->f[i].mat].spec[2], 1.0f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
			glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
			glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
			last = obj->f[i].mat;
			if(obj->mat[obj->f[i].mat].texture==-1)
				glDisable(GL_TEXTURE_2D);
			else {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,
				obj->mat[obj->f[i].mat].texture);
			}
		}
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
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bmp->data);
	destroy_bitmap(bmp);
	return tex_id;
}
/**
 * @brief Load material library file.
 */
static int load_material(struct objfile *obj, const char *filename)
{
	file_t file;
	char buf[256];
	int ismat;

	init_file(&file);
	open_file(&file, filename, "rt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY)
		return 1;
	ismat = 0;
	while(readf_file(&file, "%s", buf) != EOF) {
		struct material mat;
		memset(&mat, 0, sizeof(struct material));
		mat.texture = -1;
		if(!strcmp(buf, "newmtl")) {
			if(ismat)
				vector_push_back(obj->mat, mat);
			ismat = 0;
			readf_file(&file, "%s", mat.name);
		} else if(!strcmp(buf, "Ns")) {
			readf_file(&file, "%f", &mat.ns);
			ismat = 1;
		} else if(!strcmp(buf, "Ka")) {
			readf_file(&file, "%f %f %f",
				&mat.amb[0], &mat.amb[1], &mat.amb[2]);
			ismat = 1;
		} else if(!strcmp(buf, "Kd")) {
			readf_file(&file, "%f %f %f",
				&mat.dif[0], &mat.dif[1], &mat.dif[2]);
			ismat = 1;
		} else if(!strcmp(buf, "Ks")) {
			readf_file(&file, "%f %f %f",
				&mat.spec[0], &mat.spec[1], &mat.spec[2]);
			ismat = 1;
		} else if(!strcmp(buf, "Ni")) {
			readf_file(&file, "%f", &mat.ni);
			ismat = 1;
		} else if(!strcmp(buf, "d")) {
			readf_file(&file, "%f", &mat.alpha);
			ismat = 1;
		} else if(!strcmp(buf, "illum")) {
			readf_file(&file, "%f", &mat.illum);
			ismat = 1;
		} else if(!strcmp(buf, "map_Kd")) {
			char tmp[256];
			readf_file(&file, "%s", tmp);
			mat.texture = load_texture(tmp);
			ismat = 1;
		} else if(!strcmp(buf, "vt")) {
			readf_file(&file, "%f %f", &mat.tex.u, &mat.tex.v);
			mat.tex.v = 1-mat.tex.v;
			obj->istex = 1;
		}
	}
	if(vector_size(obj->mat) == 0)
		obj->ismat = 0;
	else
		obj->ismat = 1;
	return 0;
}
/**
 * @brief Create object from file.
 */
int load_object(struct objfile *obj, const char *filename)
{
	file_t file;
	char buf[256];
	int curmat;

	UNUSED(load_texture(NULL));
	init_file(&file);
	open_file(&file, filename, "rt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY) {
		fprintf(stderr, "Error: %s\n", get_error_file(&file));
		return 1;
	}
	curmat = 0;
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
			memset(&f, 0, sizeof(struct face));
			if(strichr(buf, ' ') == 4) {
				f.four = 1;
				if(strstr(buf, "//") != NULL) {
					sscanf(buf,
					"%d//%d %d//%d %d//%d %d//%d",
					&f.face.f1, &f.num, &f.face.f2,
					&f.num, &f.face.f3, &f.num,
					&f.face.f4, &f.num);
					f.tex.f1=f.tex.f2=f.tex.f3=f.tex.f4=0;
					f.mat = curmat;
					vector_push_back(obj->f, f);
				} else if(strstr(buf, "/") != NULL) {
					sscanf(buf,
					"%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&f.face.f1, &f.tex.f1, &f.num,
					&f.face.f2, &f.tex.f2, &f.num,
					&f.face.f3, &f.tex.f3, &f.num,
					&f.face.f4, &f.tex.f4, &f.num);
					f.mat = curmat;
					vector_push_back(obj->f, f);
				} else {
					sscanf(buf,
					"%d %d %d %d",
					&f.face.f1, &f.face.f2, &f.face.f3,
					&f.face.f4);
					f.tex.f1=f.tex.f2=f.tex.f3=f.tex.f4=0;
					f.num=0;
					f.mat = curmat;
					vector_push_back(obj->f, f);
				}
			} else {
				f.four = 0;
				if(strstr(buf, "//") != NULL) {
					sscanf(buf, "%d//%d %d//%d %d//%d",
					&f.face.f1, &f.num, &f.face.f2,
					&f.num, &f.face.f3, &f.num);
					f.face.f4 = 0;
					f.tex.f1=f.tex.f2=f.tex.f3=f.tex.f4=0;
					f.mat = curmat;
					vector_push_back(obj->f, f);
				} else if(strstr(buf, "/") != NULL) {
					sscanf(buf,
					"%d/%d/%d %d/%d/%d %d/%d/%d",
					&f.face.f1, &f.tex.f1, &f.num,
					&f.face.f2, &f.tex.f2, &f.num,
					&f.face.f3, &f.tex.f3, &f.num);
					f.face.f4 = 0;
					f.tex.f4 = 0;
					f.mat = curmat;
					vector_push_back(obj->f, f);
				} else {
					sscanf(buf,
					"%d %d %d",
					&f.face.f1, &f.face.f2, &f.face.f3);
					f.tex.f1=f.tex.f2=f.tex.f3=f.tex.f4=0;
					f.face.f4 = f.num = 0;
					f.mat = curmat;
					vector_push_back(obj->f, f);
				}
			}
		} else if(!strcmp(buf, "usemtl")) {
			char tmpname[256];
			size_t i;

			sscanf(buf, "%s", tmpname);
			for(i=0; i<vector_size(obj->mat); i++) {
				if(!strcmp(obj->mat[i].name, tmpname)) {
					curmat = i;
					break;
				}
			}
		} else if(!strcmp(buf, "mtllib")) {
			char tmpname[256];

			readf_file(&file, "%s", tmpname);
			if(load_material(obj, tmpname)) {
				fprintf(stderr,
				"Warning: Could not load material library: %s",
				tmpname);
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
			if(obj->f[i].tex.f1 == 0 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d//%d %d//%d %d//%d %d//%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].num,
				obj->f[i].face.f4, obj->f[i].num);
			} else if(obj->f[i].tex.f1 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].tex.f1,
				obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].tex.f2,
				obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].tex.f3,
				obj->f[i].num,
				obj->f[i].face.f4, obj->f[i].tex.f4,
				obj->f[i].num);
			} else {
				printf("Quad: %d\n"
				"%d %d %d %d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].face.f2,
				obj->f[i].face.f3, obj->f[i].face.f4);
			}
		} else {
			if(obj->f[i].tex.f1 == 0 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d//%d %d//%d %d//%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].num);
			} else if(obj->f[i].tex.f1 && obj->f[i].num) {
				printf("Quad: %d\n"
				"%d/%d/%d %d/%d/%d %d/%d/%d\n",
				obj->f[i].four,
				obj->f[i].face.f1, obj->f[i].tex.f1,
				obj->f[i].num,
				obj->f[i].face.f2, obj->f[i].tex.f2,
				obj->f[i].num,
				obj->f[i].face.f3, obj->f[i].tex.f3,
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
	vector_free(obj->mat);
	vector_free(obj->t);
	vector_free(obj->l);
	memset(obj, 0, sizeof(struct objfile));
	free(obj);
}

