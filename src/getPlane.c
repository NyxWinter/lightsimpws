#include <stdlib.h>
#include <stdio.h>
#include "getPlane.h"
#include "structs.h"
#include "intersections.h"

float *getPlane(struct plane plane){
    // Easier to work with
    float verts[3][3];
    verts[0][0] = plane.verts[0][0];
    verts[0][1] = plane.verts[0][1];
    verts[0][2] = plane.verts[0][2];
    verts[1][0] = plane.verts[1][0];
    verts[1][1] = plane.verts[1][1];
    verts[1][2] = plane.verts[1][2];
    verts[2][0] = plane.verts[2][0];
    verts[2][1] = plane.verts[2][1];
    verts[2][2] = plane.verts[2][2];
    float *values =(float *)malloc(4 * sizeof(float));
    // Cross product madness. Calculates the equation of each plane.
    values[0] = ((verts[1][1] - verts[0][1]) * (verts[2][2] - verts[0][2])) - ((verts[1][2] - verts[0][2]) * (verts[2][1] - verts[0][1]));
    values[1] = ((verts[1][2] - verts[0][2]) * (verts[2][0] - verts[0][0])) - ((verts[1][0] - verts[0][0]) * (verts[2][2] - verts[0][2]));
    values[2] = ((verts[1][0] - verts[0][0]) * (verts[2][1] - verts[0][1])) - ((verts[1][1] - verts[0][1]) * (verts[2][0] - verts[0][0]));
    values[3] = (values[0] * verts[0][0]) + (values[1] * verts[0][1]) + (values[2] * verts[0][2]);
//    printf("Plane:\n %fx + %fy + %fz + %f = 0", values[0], values[1], values[2], values[3]);
    int tester = 0;
    // They came out a little big. This function only runs one so it can be slow. It doesn't really matter
    values[0] /= 10000;
    values[1] /= 10000;
    values[2] /= 10000;
    values[3] /= 10000;

    // Get as many positive values as possible because it looks better
    if (values[0] >= 0)
        tester += 1;
    if (values[1] >= 0)
        tester += 1;
    if (values[2] >= 0)
        tester += 1;
    if (tester <= 2){
        values[0] *= -1;
        values[1] *= -1;
        values[2] *= -1;
        values[3] *= -1;
    }
  return values;
}
