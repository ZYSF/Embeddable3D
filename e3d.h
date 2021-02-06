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
    e3d_float_t a;
};

struct e3d_triangle_s {
    e3d_vertex_t* a;
    e3d_vertex_t* b;
    e3d_vertex_t* c;
    e3d_pixel_t pixel;
};

struct e3d_matrix_s {
    //e3d_float_t values[9];
    e3d_float_t values[16];
};

struct e3d_target_s {
    e3d_int_t width;
    e3d_int_t height;
    e3d_pixel_t* pixels;
    e3d_float_t* depths;
};

static void e3d_matrix_assign(e3d_matrix_t* result, e3d_matrix_t* source) {
    e3d_int_t i;
    for (i = 0; i < 16; i++) {
        result->values[i] = source->values[i];
    }
}

/* Resets the matrix to an "identity matrix" (i.e. with no transformations applied). */
static void e3d_matrix_reset(e3d_matrix_t* r) {
    e3d_int_t i;
    for (i = 0; i < 4; i++) {
        e3d_int_t j;
        for (j = 0; j < 4; j++) {
            r->values[i * 4 + j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

static void e3d_matrix_multiply(e3d_matrix_t* r, e3d_matrix_t* a, e3d_matrix_t* b) {
    e3d_int_t i;
    for (i = 0; i < 4; i++) {
        e3d_int_t j;
        for (j = 0; j < 4; j++) {
            r->values[i * 4 + j] = a->values[j + 0] * b->values[i * 4 + 0]
                + a->values[j + 4] * b->values[i * 4 + 1]
                + a->values[j + 8] * b->values[i * 4 + 2]
                + a->values[j + 12] * b->values[i * 4 + 3];
        }

    }

    /*e3d_int_t tmpi;
    for (tmpi = 0; tmpi < 9; tmpi++) {
        r->values[tmpi] = 0.0;
    }
    e3d_int_t y;
    for (y = 0; y < 3; y++) {
        e3d_int_t x;
        for (x = 0; x < 3; x++) {
            e3d_int_t i;
            for (i = 0; i < 3; i++) {
                r->values[y * 3 + x] += a->values[(y * 3) + i] * b->values[(i * 3) + x];
            }
        }
    }*/
}

static void e3d_vertex_multiply(e3d_vertex_t* result, e3d_matrix_t* matrix, e3d_vertex_t* vertex) {
    result->x = matrix->values[0] * vertex->x + matrix->values[4] * vertex->y + matrix->values[8] * vertex->z + matrix->values[12] * vertex->a;
    result->y = matrix->values[1] * vertex->x + matrix->values[5] * vertex->y + matrix->values[9] * vertex->z + matrix->values[13] * vertex->a;
    result->z = matrix->values[2] * vertex->x + matrix->values[6] * vertex->y + matrix->values[10] * vertex->z + matrix->values[14] * vertex->a;
    result->a = matrix->values[3] * vertex->x + matrix->values[7] * vertex->y + matrix->values[11] * vertex->z + matrix->values[15] * vertex->a;
    /*
    result->x = vertex->x * matrix->values[0] + vertex->y * matrix->values[3] + vertex->z * matrix->values[6];
    result->y = vertex->x * matrix->values[1] + vertex->y * matrix->values[4] + vertex->z * matrix->values[7];
    result->z = vertex->x * matrix->values[2] + vertex->y * matrix->values[5] + vertex->z * matrix->values[8];
    */
}

static void e3d_matrix_translate(e3d_matrix_t* matrix, e3d_float_t tx, e3d_float_t ty, e3d_float_t tz) {
    matrix->values[12] += matrix->values[0] * tx + matrix->values[4] * ty + matrix->values[8] * tz;
    matrix->values[13] += matrix->values[1] * tx + matrix->values[5] * ty + matrix->values[9] * tz;
    matrix->values[14] += matrix->values[2] * tx + matrix->values[6] * ty + matrix->values[10] * tz;
    matrix->values[15] += matrix->values[3] * tx + matrix->values[7] * ty + matrix->values[11] * tz;
}

static void e3d_matrix_scale(e3d_matrix_t* matrix, e3d_float_t sx, e3d_float_t sy, e3d_float_t sz) {
    e3d_int_t i;
    for (i = 0; i < 4; i++) {
        matrix->values[i] *= sx;
    }
    for (i = 0; i < 4; i++) {
        matrix->values[i + 4] *= sy;
    }
    for (i = 0; i < 4; i++) {
        matrix->values[i + 8] *= sz;
    }
}

/* Calculates inverse square root (NOTE: there is almost certainly a better way of doing this). */
static e3d_float_t e3d_invsqrt(e3d_float_t value) {
    return 1.0 / sqrt(value);
}

static void e3d_matrix_rotate(e3d_matrix_t* matrix, e3d_float_t xm, e3d_float_t ym, e3d_float_t zm, e3d_float_t radians) {
    e3d_float_t l = xm * xm + ym * ym + zm * zm;

    if (l != 0 && l != 1) {
        e3d_float_t inv = e3d_invsqrt(l);
        xm *= inv;
        ym *= inv;
        zm *= inv;
    }

    e3d_float_t mysin = sin(radians);
    e3d_float_t mycos = cos(radians);
    e3d_float_t ncos = 1 - mycos;

    e3d_matrix_t rotx;

    e3d_matrix_reset(&rotx);
    rotx.values[0] = xm * xm * ncos + mycos;
    rotx.values[1] = ym * xm * ncos + zm * mysin;
    rotx.values[2] = xm * zm * ncos - ym * mysin;
    rotx.values[4] = xm * ym * ncos - zm * mysin;
    rotx.values[5] = ym * ym * ncos + mycos;
    rotx.values[6] = ym * zm * ncos + xm * mysin;
    rotx.values[8] = xm * zm * ncos + ym * mysin;
    rotx.values[9] = ym * zm * ncos - xm * mysin;
    rotx.values[10] = zm * zm * ncos + mycos;

    e3d_matrix_t dupx;
    e3d_matrix_assign(&dupx, matrix);
    e3d_matrix_multiply(matrix, &dupx, &rotx);
    /*

    // Pre-compute rotation values
    const PRfloat s = PR_SIN(angle);
    const PRfloat c = PR_COS(angle);
    const PRfloat cc = 1.0f - c;

    // Setup rotation matrix
    pr_matrix4 rhs;
    PRfloat* m = &(rhs.m[0][0]);

    m[0] = x*x*cc + c;   m[4] = x*y*cc - z*s; m[ 8] = x*z*cc + y*s; m[12] = 0.0f;
    m[1] = y*x*cc + z*s; m[5] = y*y*cc + c;   m[ 9] = y*z*cc - x*s; m[13] = 0.0f;
    m[2] = x*z*cc - y*s; m[6] = y*z*cc + x*s; m[10] = z*z*cc + c;   m[14] = 0.0f;
    m[3] = 0.0f;         m[7] = 0.0f;         m[11] = 0.0f;         m[15] = 1.0f;

    // Multiply input matrix (lhs) with rotation matrix (rhs)
    pr_matrix4 lhs;
    _pr_matrix_copy(&lhs, result);

    _pr_matrix_mul_matrix(result, &lhs, &rhs);*/
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
    e3d_int_t maxx = (e3d_int_t)e3d_float_min(target->width - 1, floor(e3d_float_max(a.x, e3d_float_max(b.x, c.x))));

    e3d_int_t miny = (e3d_int_t)e3d_float_max(0, ceil(e3d_float_min(a.y, e3d_float_min(b.y, c.y))));
    e3d_int_t maxy = (e3d_int_t)e3d_float_min(target->height - 1, floor(e3d_float_max(a.y, e3d_float_max(b.y, c.y))));
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