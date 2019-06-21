#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"

int main()
{
	struct objfile *obj;

	obj = load_object("test.obj");
	print_object(obj);
	if(obj) destroy_object(obj);
	return 0;
}
