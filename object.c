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
#include <GL/glu.h>

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
/**
 * @brief Create a new vector 3.
 */
static struct vec3 new_vec3(float x, float y, float z)
{
	struct vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}
/**
 * @brief Creates a new face structure and fills it with data.
 */
static struct face new_face(int four, int num, int mat, int f[4], int t[4])
{
	struct face face;
	face.four = four;
	face.num = num;
	face.mat = mat;
	face.face.f1 = f[0];
	face.face.f2 = f[1];
	face.face.f3 = f[2];
	face.face.f4 = f[3];
	face.tex.f1 = t[0];
	face.tex.f2 = t[1];
	face.tex.f3 = t[2];
	face.tex.f4 = t[3];
	return face;
}
/**
 * @brief Creates a new material structure and fills it with data.
 */
static struct material new_material(const char *name, float alpha,
	float ns, float ni, float dif[], float amb[], float spec[],
	int illum, unsigned int tex)
{
	struct material m;
	strncpy(m.name, name, strlen(name));
	m.alpha = alpha;
	m.ns = ns;
	m.ni = ni;
	m.dif[0] = dif[0];
	m.dif[1] = dif[1];
	m.dif[2] = dif[2];
	m.amb[0] = amb[0];
	m.amb[1] = amb[1];
	m.amb[2] = amb[2];
	m.spec[0] = spec[0];
	m.spec[1] = spec[1];
	m.spec[2] = spec[2];
	m.illum = illum;
	m.texture = (tex > 0 ? tex : 0);
	return m;
}
/**
 * @brief Create a new uv coordinate.
 */
static struct texcoord new_coord(float u, float v)
{
	struct texcoord t;
	t.u = u;
	t.v = v;
	return t;
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
	obj->t = NULL;
	obj->l = -1;
	obj->mat = NULL;
	obj->f = NULL;
	return obj;
}
/**
 * @brief Generate a GL list for drawing.
 */
static int make_object(struct objfile *obj)
{
	int unique_number;
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
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
			last = obj->f[i].mat;
			if(obj->mat[obj->f[i].mat].texture == 0) {
				glDisable(GL_TEXTURE_2D);
			} else {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,
				obj->mat[obj->f[i].mat].texture);
			}
		}
		if(obj->f[i].four) {
			glBegin(GL_QUADS);
			if(obj->isnorm) {
				glNormal3f(obj->vn[obj->f[i].num-1].x,
					obj->vn[obj->f[i].num-1].y,
					obj->vn[obj->f[i].num-1].z);
			}
			if(obj->istex) {
				glTexCoord2f(obj->t[obj->f[i].tex.f1-1].u,
					obj->t[obj->f[i].tex.f1-1].v);
			}
			glVertex3f(obj->v[obj->f[i].face.f1-1].x,
				obj->v[obj->f[i].face.f1-1].y,
				obj->v[obj->f[i].face.f1-1].z);
			if(obj->istex) {
				glTexCoord2f(obj->t[obj->f[i].tex.f2-1].u,
					obj->t[obj->f[i].tex.f2-1].v);
			}
			glVertex3f(obj->v[obj->f[i].face.f2-1].x,
				obj->v[obj->f[i].face.f2-1].y,
				obj->v[obj->f[i].face.f2-1].z);
			if(obj->istex) {
				glTexCoord2f(obj->t[obj->f[i].tex.f3-1].u,
					obj->t[obj->f[i].tex.f3-1].v);
			}
			glVertex3f(obj->v[obj->f[i].face.f3-1].x,
				obj->v[obj->f[i].face.f3-1].y,
				obj->v[obj->f[i].face.f3-1].z);
			if(obj->istex) {
				glTexCoord2f(obj->t[obj->f[i].tex.f4-1].u,
					obj->t[obj->f[i].tex.f4-1].v);
			}
			glVertex3f(obj->v[obj->f[i].face.f4-1].x,
				obj->v[obj->f[i].face.f4-1].y,
				obj->v[obj->f[i].face.f4-1].z);
			glEnd();
		} else {
			glBegin(GL_TRIANGLES);
			if(obj->isnorm) {
				glNormal3f(obj->vn[obj->f[i].num-1].x,
					obj->vn[obj->f[i].num-1].y,
					obj->vn[obj->f[i].num-1].z);
			}
			if(obj->istex) {
				glTexCoord2f(obj->t[obj->f[i].tex.f1-1].u,
					obj->t[obj->f[i].tex.f1-1].v);
			}
			glVertex3f(obj->v[obj->f[i].face.f1-1].x,
				obj->v[obj->f[i].face.f1-1].y,
				obj->v[obj->f[i].face.f1-1].z);
			if(obj->istex) {
				glTexCoord2f(obj->t[obj->f[i].tex.f2-1].u,
					obj->t[obj->f[i].tex.f2-1].v);
			}
			glVertex3f(obj->v[obj->f[i].face.f2-1].x,
				obj->v[obj->f[i].face.f2-1].y,
				obj->v[obj->f[i].face.f2-1].z);
			if(obj->istex) {
				glTexCoord2f(obj->t[obj->f[i].tex.f3-1].u,
					obj->t[obj->f[i].tex.f3-1].v);
			}
			glVertex3f(obj->v[obj->f[i].face.f3-1].x,
				obj->v[obj->f[i].face.f3-1].y,
				obj->v[obj->f[i].face.f3-1].z);
			glEnd();
		}
	}
	glEndList();
	if(glGetError() == GL_NO_ERROR)
		return unique_number;
	return -1;
}
/**
 * @brief Load a texture from a filename.
 */
static unsigned int load_texture(const char *filename)
{
	Bitmap *bmp;
	unsigned int tex_id;

	bmp = load_bitmap(filename);
	if(get_last_error_bitmap() != BMP_NO_ERROR)
		return 0;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp->info.width,
		bmp->info.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp->data);
	destroy_bitmap(bmp);
	return tex_id;
}
/**
 * @brief Load material library file.
 */
static int load_material(struct objfile *obj, const char *filename)
{
	float alpha, ns, ni, dif[3], amb[3], spec[3];
	int illum, tex;
	char name[256], fname[256];
	file_t file;
	char buf[256];
	int ismat;

	init_file(&file);
	open_file(&file, filename, "rt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY)
		return 1;
	ismat = tex = 0;
	strcpy(fname, "\0");
	while(readf_file(&file, "%s", buf) != EOF) {
		if(!strcmp(buf, "newmtl")) {
			if(ismat) {
				if(!strcmp(fname, "")) {
					vector_push_back(obj->mat,
					new_material(name, alpha, ns, ni, dif,
					amb, spec, illum, -1));
				} else {
					vector_push_back(obj->mat,
					new_material(name, alpha, ns, ni, dif,
					amb, spec, illum, tex));
				}
			}
			ismat = tex = 0;
			readf_file(&file, "%s", name);
		} else if(!strcmp(buf, "Ns")) {
			readf_file(&file, "%f", &ns);
			ismat = 1;
		} else if(!strcmp(buf, "Ka")) {
			readf_file(&file, "%f %f %f",
				&amb[0], &amb[1], &amb[2]);
			ismat = 1;
		} else if(!strcmp(buf, "Kd")) {
			readf_file(&file, "%f %f %f",
				&dif[0], &dif[1], &dif[2]);
			ismat = 1;
		} else if(!strcmp(buf, "Ks")) {
			readf_file(&file, "%f %f %f",
				&spec[0], &spec[1], &spec[2]);
			ismat = 1;
		} else if(!strcmp(buf, "Ni")) {
			readf_file(&file, "%f", &ni);
			ismat = 1;
		} else if(!strcmp(buf, "d")) {
			readf_file(&file, "%f", &alpha);
			ismat = 1;
		} else if(!strcmp(buf, "illum")) {
			readf_file(&file, "%f", &illum);
			ismat = 1;
		} else if(!strcmp(buf, "map_Kd")) {
			readf_file(&file, "%s", fname);
			tex = load_texture(fname);
		}
	}
	if(ismat) {
		if(!strcmp(fname, "")) {
			vector_push_back(obj->mat,
			new_material(name, alpha, ns, ni, dif, amb,
			spec, illum, -1));
		} else {
			vector_push_back(obj->mat,
			new_material(name, alpha, ns, ni, dif, amb,
			spec, illum, tex));
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

	init_file(&file);
	open_file(&file, filename, "rt");
	if(get_errori_file(&file) != FILE_ERROR_OKAY) {
		fprintf(stderr, "Error: %s\n", get_error_file(&file));
		return 1;
	}
	curmat = 0;
	while(readf_file(&file, "%s", buf) != EOF) {
		if(!strcmp(buf, "v")) {
			float x, y, z;
			readf_file(&file, "%f %f %f", &x, &y, &z);
			vector_push_back(obj->v, new_vec3(x, y, z));
		} else if(!strcmp(buf, "vn")) {
			float x, y, z;
			readf_file(&file, "%f %f %f", &x, &y, &z);
			vector_push_back(obj->vn, new_vec3(x, y, z));
			obj->isnorm = 1;
		} else if(!strcmp(buf, "f")) {
			int f[4], t[4], num, four;
			if(gets_file(&file, buf, sizeof(buf)) == NULL)
				continue;
			if(strichr(buf, ' ') == 4) {
				four = 1;
				if(strstr(buf, "//") != NULL) {
					sscanf(buf,
					"%d//%d %d//%d %d//%d %d//%d",
					&f[0], &num, &f[1],
					&num, &f[2], &num,
					&f[3], &num);
					t[0]=t[1]=t[2]=t[3]=0;
					vector_push_back(obj->f,
					new_face(four, num, curmat, f, t));
				} else if(strstr(buf, "/") != NULL) {
					sscanf(buf,
					"%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&f[0], &t[0], &num,
					&f[1], &t[1], &num,
					&f[2], &t[2], &num,
					&f[3], &t[3], &num);
					vector_push_back(obj->f,
					new_face(four, num, curmat, f, t));
				} else {
					sscanf(buf,
					"%d %d %d %d",
					&f[0], &f[2], &f[3],
					&f[4]);
					t[0]=t[1]=t[2]=t[3]=0;
					vector_push_back(obj->f,
					new_face(four, -1, curmat, f, t));
				}
			} else {
				four = 0;
				if(strstr(buf, "//") != NULL) {
					sscanf(buf, "%d//%d %d//%d %d//%d",
					&f[0], &num, &f[1],
					&num, &f[2], &num);
					f[4] = 0;
					t[0]=t[1]=t[2]=t[3]=0;
					vector_push_back(obj->f,
					new_face(four, num, curmat, f, t));
				} else if(strstr(buf, "/") != NULL) {
					sscanf(buf,
					"%d/%d/%d %d/%d/%d %d/%d/%d",
					&f[0], &t[0], &num,
					&f[1], &t[1], &num,
					&f[2], &t[2], &num);
					f[3] = 0;
					t[3] = 0;
					vector_push_back(obj->f,
					new_face(four, num, curmat, f, t));
				} else {
					sscanf(buf,
					"%d %d %d",
					&f[0], &f[1], &f[2]);
					t[0]=t[1]=t[2]=t[3]=0;
					f[4] = 0;
					vector_push_back(obj->f,
					new_face(four, -1, curmat, f, t));
				}
			}
		} else if(!strcmp(buf, "vt")) {
			float u, v;
			readf_file(&file, "%f %f", &u, &v);
			vector_push_back(obj->t, new_coord(u, 1-v));
			obj->istex = 1;
		} else if(!strcmp(buf, "usemtl")) {
			char tmpname[256];
			size_t i;
			readf_file(&file, "%s", tmpname);
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
				"Warning: Could not load mtllib: %s\n",
				tmpname);
			}
		}
	}
	close_file(&file);
	obj->l = make_object(obj);
	return 0;
}
/**
 * @brief Draw object to screen.
 */
void draw_object(struct objfile *obj)
{
	glCallList(obj->l);
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
	for(i=0; i<vector_size(obj->mat); i++) {
		printf("Name: %s\nAlpha: %f\nNs: %f\nNi: %f\n"
			"Diffuse: %f %f %f\n"
			"Ambient: %f %f %f\n"
			"Specular: %f %f %f\n"
			"Texture ID: %d\n",
			obj->mat[i].name, obj->mat[i].alpha,
			obj->mat[i].ns, obj->mat[i].ni,
			obj->mat[i].dif[0], obj->mat[i].dif[1],
			obj->mat[i].dif[2], obj->mat[i].amb[0],
			obj->mat[i].amb[1], obj->mat[i].amb[2],
			obj->mat[i].spec[0], obj->mat[i].spec[1],
			obj->mat[i].spec[2], obj->mat[i].texture);
	}
	printf("=====================================================\n");
	for(i=0; i<vector_size(obj->t); i++) {
		printf("Texture UV Coords: %f %f\n",
			obj->t[i].u, obj->t[i].v);
	}
	printf("=====================================================\n");
	for(i=0; i<vector_size(obj->f); i++) {
		printf("Face Number: %lu\nTexture Material Indexes [1-4]:\n"
			"%d %d %d %d\n", i,
			obj->f[i].tex.f1,
			obj->f[i].tex.f2,
			obj->f[i].tex.f3,
			obj->f[i].tex.f4
		);
	} 
	printf("=====================================================\n");
}
/**
 * @brief Destroy given object structure.
 */
void destroy_object(struct objfile *obj)
{
	size_t i;

	for(i=0; i<vector_size(obj->mat); i++)
		glDeleteTextures(1, &obj->mat[i].texture);
	glDeleteLists(1, obj->l);
	vector_free(obj->v);
	vector_free(obj->vn);
	vector_free(obj->f);
	vector_free(obj->mat);
	vector_free(obj->t);
	memset(obj, 0, sizeof(struct objfile));
	free(obj);
}

