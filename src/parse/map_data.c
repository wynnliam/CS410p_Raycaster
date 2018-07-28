// Liam Wynn, 7/23/2018, Raycaster: Map File Loader

#include "map_data.h"

#include <stdlib.h>

int add_component(struct map_data* add_to) {
	if(!add_to)
		return 0;

	struct component* to_add = (struct component*)malloc(sizeof(struct component));

	to_add->x = 0;
	to_add->y = 0;
	to_add->w = 0;
	to_add->h = 0;

	to_add->is_floor_ceil = 0;
	to_add->tex_0 = NULL;
	to_add->tex_1 = NULL;

	to_add->next = add_to->component_head;
	add_to->component_head = to_add;

	return 1;
}

int add_thing_data(struct map_data* add_to) {
	if(!add_to)
		return 0;

	struct thing_data* to_add = (struct thing_data*)malloc(sizeof(struct thing_data));

	to_add->x = 0;
	to_add->y = 0;
	to_add->type = 0;

	to_add->next = add_to->thing_head;
	add_to->thing_head = to_add;

	return 1;
}
