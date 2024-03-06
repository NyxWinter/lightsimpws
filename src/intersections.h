#ifndef INTERSECTIONS_H_
#define INTERSECTIONS_H_
#include "structs.h"
#include <stdlib.h>

float getArea(float P[3], float Q[3], float R[3], int ommitedPlane);
float intersectPlane(struct plane plane, struct ray ray);
struct ray intersectAll(struct plane* planes, struct ray ray, size_t totalPlanes, int maxBounces);
struct ray mirrorLine(struct plane plane, struct ray ray);

#endif // INTERSECTIONS_H_
