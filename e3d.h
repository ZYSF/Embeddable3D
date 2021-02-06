#ifndef E3D_H
#define E3D_H

/* NOTE: This is currently just a very simple renderer.
 * It's based loosely on the algorithms at http://blog.rogach.org/2015/08/how-to-create-your-own-simple-3d-render.html
 * It should be easy to customise with more advanced behaviour though.
 */

typedef double e3d_float_t;
typedef long long int e3d_int_t;
typedef int e3d_pixel_t;

typedef struct e3d_vertex_s e3d_vertex_t;
typedef struct e3d_triangle_s e3d_triangle_t;
typedef struct e3d_matrix_s e3d_matrix_t;
typedef struct e3d_target_s e3d_target_t;

struct e3d_vertex_s {
    e3d_float_t x;
    e3d_float_t y;
    e3d_float_t z;
};

struct e3d_triangle_s {
    e3d_vertex_t* a;
    e3d_vertex_t* b;
    e3d_vertex_t* c;
    e3d_pixel_t pixel;
};

struct e3d_matrix_s {
    e3d_float_t values[9];
};

struct e3d_target_s {
    e3d_int_t width;
    e3d_int_t height;
    e3d_pixel_t* pixels;
    e3d_float_t* depths;
};

static void e3d_matrix_multiply(e3d_matrix_t* result, e3d_matrix_t* a, e3d_matrix_t* b) {
    e3d_int_t tmpi;
    for (tmpi = 0; tmpi < 9; tmpi++) {
        result->values[tmpi] = 0.0;
    }
    e3d_int_t y;
    for (y = 0; y < 3; y++) {
        e3d_int_t x;
        for (x = 0; x < 3; x++) {
            e3d_int_t i;
            for (i = 0; i < 3; i++) {
                result->values[y * 3 + x] += a->values[(y * 3) + i] * b->values[(i * 3) + x];
            }
        }
    }
}

static void e3d_vertex_multiply(e3d_vertex_t* result, e3d_matrix_t* matrix, e3d_vertex_t* vertex) {
    result->x = vertex->x * matrix->values[0] + vertex->y * matrix->values[3] + vertex->z * matrix->values[6];
    result->y = vertex->x * matrix->values[1] + vertex->y * matrix->values[4] + vertex->z * matrix->values[7];
    result->z = vertex->x * matrix->values[2] + vertex->y * matrix->values[5] + vertex->z * matrix->values[8];
}

static e3d_float_t e3d_float_min(e3d_float_t a, e3d_float_t b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

static e3d_float_t e3d_float_max(e3d_float_t a, e3d_float_t b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

static void e3d_target_paint(e3d_target_t* target, e3d_matrix_t* matrix, e3d_triangle_t* triangle) {
    e3d_vertex_t a;
    e3d_vertex_t b;
    e3d_vertex_t c;

    /*a = *triangle->a;
    b = *triangle->b;
    c = *triangle->c;*/

    e3d_vertex_multiply(&a, matrix, triangle->a);
    a.x += target->width / 2;
    a.y += target->height / 2;
    e3d_vertex_multiply(&b, matrix, triangle->b);
    b.x += target->width / 2;
    b.y += target->height / 2;
    e3d_vertex_multiply(&c, matrix, triangle->c);
    c.x += target->width / 2;
    c.y += target->height / 2;
    
    e3d_vertex_t ab;
    e3d_vertex_t ac;
    
    ab.x = b.x - a.x;
    ab.y = b.y - a.y;
    ab.z = b.z - a.z;

    ac.x = c.x - a.x;
    ac.y = c.y - a.y;
    ac.z = c.z - a.z;

    e3d_vertex_t n;
    n.x = ab.y * ac.z - ab.z * ac.y;
    n.y = ab.z * ac.x - ab.x * ac.z;
    n.z = ab.x * ac.y - ab.y * ac.x;

    e3d_float_t nlen = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    n.x /= nlen;
    n.y /= nlen;
    n.z /= nlen;

    e3d_float_t acos = (n.z < 0) ? -n.z : n.z;

    
    e3d_int_t minx = (e3d_int_t)e3d_float_max(0, ceil(e3d_float_min(a.x, e3d_float_min(b.x, c.x))));
    e3d_int_t maxx = (e3d_int_t)e3d_float_max(target->width - 1, floor(e3d_float_max(a.x, e3d_float_max(b.x, c.x))));

    e3d_int_t miny = (e3d_int_t)e3d_float_max(0, ceil(e3d_float_min(a.y, e3d_float_min(b.y, c.y))));
    e3d_int_t maxy = (e3d_int_t)e3d_float_max(target->height - 1, floor(e3d_float_max(a.y, e3d_float_max(b.y, c.y))));
    /*
    e3d_int_t minx = 0;
    e3d_int_t maxx = target->width - 1;
    e3d_int_t miny = 0;
    e3d_int_t maxy = target->height - 1;
    */
    e3d_float_t tarea = (a.y - c.y) * (b.x - c.x) + (b.y - c.y) * (c.x - a.x);

    //printf("minx=%d maxx=%d miny=%d maxy=%d\n", minx, maxx, miny, maxy);
    e3d_int_t y;
    for (y = miny; y <= maxy; y++) {
        e3d_int_t x;
        for (x = minx; x <= maxx; x++) {
            e3d_float_t pa = ((y - c.y) * (b.x - c.x) + (b.y - c.y) * (c.x - x)) / tarea;
            e3d_float_t pb = ((y - a.y) * (c.x - a.x) + (c.y - a.y) * (a.x - x)) / tarea;
            e3d_float_t pc = ((y - b.y) * (a.x - b.x) + (a.y - b.y) * (b.x - x)) / tarea;
            if (pa >= 0 && pa <= 1 && pb >= 0 && pb <= 1 && pc >= 0 && pc <= 1)
            {
                e3d_float_t d = pa * a.z + pb * b.z + pc * c.z;
                e3d_int_t idx = y * target->width + x;
                //printf("Near hit @ %dx%d\n", x, y);
                if (target->depths[idx] < d)
                {
                    //printf("Hit @ %dx%d\n", x, y);
                    target->pixels[idx] = triangle->pixel;
                    target->depths[idx] = d;
                }
            }
        }
    }
    //printf("done\n");
}

/* From ifndef at top of file: */
#endif