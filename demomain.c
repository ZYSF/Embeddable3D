#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "e3d.h"

e3d_vertex_t* v(e3d_float_t x, e3d_float_t y, e3d_float_t z) {
	e3d_vertex_t* result = calloc(1, sizeof(e3d_vertex_t));

	result->x = x;
	result->y = y;
	result->z = z;

	return result;
}
e3d_triangle_t* t(e3d_vertex_t* a, e3d_vertex_t* b, e3d_vertex_t* c, e3d_pixel_t p) {
	e3d_triangle_t* result = calloc(1, sizeof(e3d_triangle_t));

	result->a = a;
	result->b = b;
	result->c = c;
	result->pixel = p;

	return result;
}
e3d_float_t rad(e3d_float_t deg) {
	return (deg / 360.0) * (2 * 3.1415);
}

int main(int argc, const char** argv) {

	e3d_target_t target;
	target.width = 60;
	target.height = 20;
	target.pixels = calloc(target.width * target.height, sizeof(e3d_pixel_t));
	target.depths = calloc(target.width * target.height, sizeof(e3d_float_t));
	int i;
	for (i = 0; i < target.width * target.height; i++) {
		target.pixels[i] = '#';
		target.depths[i] = -INFINITY;
	}

	e3d_triangle_t* triangles[4];
	triangles[0] = t(v(6, 6, 6), v(-6, -6, 6), v(-6, 6, -6), 'a');
	triangles[1] = t(v(6, 6, 6), v(-6, -6, 6), v(6, -6, -6), 'b');
	triangles[2] = t(v(-6, 6, -6), v(6, -6, -6), v(6, 6, 6), 'c');
	triangles[3] = t(v(-6, 6, -6), v(6, -6, -6), v(-6, -6, 6), 'd');

	e3d_float_t heading = rad(argc >= 2 ? atof(argv[1]) : 110.0);
	
	e3d_matrix_t headx;
	headx.values[0] = cos(heading);
	headx.values[1] = 0;
	headx.values[2] = -sin(heading);
	headx.values[3] = 0;
	headx.values[4] = 1;
	headx.values[5] = 0;
	headx.values[6] = sin(heading);
	headx.values[7] = 0;
	headx.values[8] = cos(heading);

	e3d_float_t pitch = rad(argc >= 3 ? atof(argv[2]) : 190);
	e3d_matrix_t pitchx;
	pitchx.values[0] = 1;
	pitchx.values[1] = 0;
	pitchx.values[2] = 0;
	pitchx.values[3] = 0;
	pitchx.values[4] = cos(pitch);
	pitchx.values[5] = sin(pitch);
	pitchx.values[6] = 0;
	pitchx.values[7] = -sin(pitch);
	pitchx.values[8] = cos(pitch);

	e3d_matrix_t x;
	e3d_matrix_multiply(&x, &headx, &pitchx);

	e3d_target_paint(&target, &x, triangles[0]);
	e3d_target_paint(&target, &x, triangles[1]);
	e3d_target_paint(&target, &x, triangles[2]);
	e3d_target_paint(&target, &x, triangles[3]);

	int y = 0;
	for (y = 0; y < target.height; y++) {
		int x;
		for (x = 0; x < target.width; x++) {
			printf("%c", target.pixels[y * target.width + x]);
		}
		printf("\n");
	}
}