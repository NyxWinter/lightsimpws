#include "intersections.h"
#include "structs.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float getArea(float P[3], float Q[3], float R[3], int ommitedPlane){
    float area = 0;

    // Ommits the ommitable plane: Aka a projection into that plane, which makes calculating areas a *lot* easier
    int x, y;
    if (ommitedPlane == 0){
        x = 1;
        y = 2;
    }
    else if (ommitedPlane == 1){
        x = 0;
        y = 2;
    }
    else {
        x = 0;
        y = 1;
    }

    // technically it's twice the area but like that doesn't matter for the check
    area = fabsf(((P[y]-Q[y])*R[x]) + ((Q[x]-P[x])*R[y]) + (P[x]*Q[y])-(P[y]*Q[x]));

    return area;
}

float intersectPlane(struct plane plane, struct ray ray){
    // Calculates the u for the equations where the plane is equal to the line
    float pointu = (plane.values[3] - (plane.values[0] * ray.modx) - (plane.values[1] * ray.mody) - (plane.values[2] * ray.modz)) / ((plane.values[0] * ray.angx) + (plane.values[1] * ray.angy) + (plane.values[2] * ray.angz));

    if (pointu < 0.001){ // something something floating point inaccuracies.
        return 3.4E38; // See olddistance in intersectAll()
    }
    // Calculates the point where the plane intersects the line
    float point[3];
    point[0] = ray.modx + (pointu * ray.angx);
    point[1] = ray.mody + (pointu * ray.angy);
    point[2] = ray.modz + (pointu * ray.angz);
    // *Should* calculate the distance between previous point it bounced from and new point (well, the square)
    float dist = (pointu*pointu)*(ray.angx*ray.angx + ray.angy*ray.angy + ray.angz*ray.angz);
    // Add a very small multiplier if stuff pops out or something

    int ommitPlane;

    if (plane.values[0] != 0){ // Projects into x plane if x != 0
        ommitPlane = 0;
    }
    else if (plane.values[1] != 0){ // Same deal
        ommitPlane = 1;
    }
    else
        ommitPlane = 2;

    // makes them easier to work with, waste of cycles
    float pointp[3] = {plane.verts[0][0], plane.verts[0][1], plane.verts[0][2]};
    float pointq[3] = {plane.verts[1][0], plane.verts[1][1], plane.verts[1][2]};
    float pointr[3] = {plane.verts[2][0], plane.verts[2][1], plane.verts[2][2]};

    // Calculates all the area of the plane and the area of all the triangles that the interesection makes
    float areaPQR = getArea(pointp, pointq, pointr, ommitPlane);
    float areaPQS = getArea(pointp, pointq, point, ommitPlane);
    float areaPRS = getArea(pointp, pointr, point, ommitPlane);
    float areaQRS = getArea(pointq, pointr, point, ommitPlane);

    // If the areas are equal that means the intersection is in the plane.
    if (fabsf(areaPQR - areaPQS - areaPRS - areaQRS) < 4) // Gotta account for float inacc
        return dist;
    else {
        return 3.4E38; // See olddistance in intersectAll()
    }
}

struct ray intersectAll(struct plane* planes, struct ray ray, size_t totalPlanes, int maxBounces){
    float olddistance;
    float newdistance = 0.0;
    size_t correctPlane = 800; // HACK
    // If you uncomment the skybox math, these are needed
    /*i
    int skyboxR = 0;
    int skyboxG = 0;
    int skyboxB = 0;
    int skyboxlumR = 0;
    int skyboxlumG = 0;
    int skyboxlumB = 0;
    */

    while (ray.bounces < maxBounces){
        correctPlane = 2000; // Still a HACK
        olddistance = 3.4E38; // Also kind of HACK-y, limits total distance.
        for (size_t i = 0; i < totalPlanes; i++){
            // Checks the distance of each interesection with a plane. (returns 3.4E38 if it doesn't intersect)
            newdistance = intersectPlane(planes[i], ray);
            // Makes sure the smallest distance is the plane used
            if (newdistance < olddistance){
                olddistance = newdistance;
                correctPlane = i;
            }
        }
        // If it doesn't hit anything, stop trying
        if ((int)correctPlane >= 2000){

            /* // Skybox math. Kind of stupid but eh why not
            ray.R = (skyboxR + ray.R * ray.bounces) / (ray.bounces + 1);
            ray.G = (skyboxG + ray.G * ray.bounces) / (ray.bounces + 1);
            ray.B = (skyboxB + ray.B * ray.bounces) / (ray.bounces + 1);
            ray.luminanceR += skyboxlumR;
            ray.luminanceG += skyboxlumG;
            ray.luminanceB += skyboxlumB;
            if (ray.bounces == 0){
                ray.luminanceR = 255;
                ray.luminanceG = 255;
                ray.luminanceB = 255;
            }
            */

            break;
        }
        // Mirrors the ray in the plane and starts over
        struct ray mirroredRay = mirrorLine(planes[correctPlane], ray);
        ray = mirroredRay;
        // printf("PreBreak: %d, %d, %d\n", ray.R, ray.G, ray.B);
    }
    // Color magic
    ray.R *= (float)ray.luminanceR/255.0;
    ray.G *= (float)ray.luminanceG/255.0;
    ray.B *= (float)ray.luminanceB/255.0;
    return ray;
}

struct ray mirrorLine(struct plane plane, struct ray ray){
    // Calculates the u for the line <x, y, z> = <a, b, c> + u * <d, e, f> where the line cuts the plane
    float pointu = (plane.values[3] - (plane.values[0] * ray.modx) - (plane.values[1] * ray.mody) - (plane.values[2] * ray.modz)) / ((plane.values[0] * ray.angx) + (plane.values[1] * ray.angy) + (plane.values[2] * ray.angz));

    // Calculates the actual point in 3d space where line = plane
    float point[3];
    point[0] = ray.modx + (pointu * ray.angx);
    point[1] = ray.mody + (pointu * ray.angy);
    point[2] = ray.modz + (pointu * ray.angz);

    // Calculate an arbitrary point on the ray (that's not on the plane)
    float pointonline[3];
    pointonline[0] = ray.modx + ((pointu + 1) * 2 * ray.angx);
    pointonline[1] = ray.mody + ((pointu + 1) * 2 * ray.angy);
    pointonline[2] = ray.modz + ((pointu + 1) * 2 * ray.angz);

    float pointud;
    float newline[3];
    // Now use the normal of the plane to find the new u
    // normal : values[0,1,2] for <x,y,z>
    // so <x,y,z>*u + pointonline = newxyz and we need to know where that newxyz is equal to the plane
    pointud = (plane.values[3] - (plane.values[0] * pointonline[0]) - (plane.values[1] * pointonline[1]) - (plane.values[2] * pointonline[2])) / ((plane.values[0] * plane.values[0]) + (plane.values[1] * plane.values[1]) + (plane.values[2] * plane.values[2]));
    // now multiply by two and huzzah, we got a point on the mirrored line
    newline[0] = pointonline[0] + (pointud * 2 * plane.values[0]);
    newline[1] = pointonline[1] + (pointud * 2 * plane.values[1]);
    newline[2] = pointonline[2] + (pointud * 2 * plane.values[2]);

    // Random stuff for roughness
    float r1 = ray.random[0];
    float r2 = ray.random[1];
    float r3 = ray.random[2];
    float r4 = ray.random[3];
    float sign1 = 1;
    float sign2 = 1;
    float sign3 = 1;
    // Manipulates the numbers so we only need 4 random numbers
    if ((r1 - r2) > 0.0)
        sign1 = -1;
    if ((r3 - r4) > 0.0)
        sign2 = -1;
    if (((r1 * r2) - (r3 * r4)) > 0.0)
        sign3 = -1;

    float random1 = sign1 * plane.roughness * ((float)((int)r1%35) * (float)((int)r1%15) + (float)((int)r2%91) - 45);
    float random2 = sign2 * plane.roughness * ((float)((int)r2%35) * (float)((int)r2%15) + (float)((int)r3%91) - 45);
    float random3 = sign3 * plane.roughness * ((float)((int)r3%35) * (float)((int)r3%15) + (float)((int)r1%91) - 45);

    struct ray mirrorRay;
    mirrorRay.angx = (((newline[0] - point[0])*(100-plane.roughness)) + random1)/100;
    mirrorRay.angy = (((newline[1] - point[1])*(100-plane.roughness)) + random2)/100;
    mirrorRay.angz = (((newline[2] - point[2])*(100-plane.roughness)) + random3)/100;
    mirrorRay.modx = point[0];
    mirrorRay.mody = point[1];
    mirrorRay.modz = point[2];
    mirrorRay.bounces = ray.bounces + 1;
    // Hash the randoms so they're different for the next bounces. Doesn't have to be properly random
    mirrorRay.random[0] = ((int)ray.random[0] >> 5) * 0xab025f2;
    mirrorRay.random[1] = ((int)ray.random[1] >> 5) * 0xeb0554f;
    mirrorRay.random[2] = ((int)ray.random[2] >> 5) * 0xcba23f1;
    mirrorRay.random[3] = ((int)ray.random[3] >> 5) * 0x3fa23f1;

    // Color logic
    mirrorRay.R = (plane.R + ray.R * ray.bounces) / (ray.bounces + 1);
    mirrorRay.G = (plane.G + ray.G * ray.bounces) / (ray.bounces + 1);
    mirrorRay.B = (plane.B + ray.B * ray.bounces) / (ray.bounces + 1);
    mirrorRay.luminanceR = ray.luminanceR + plane.luminanceR;
    mirrorRay.luminanceG = ray.luminanceG + plane.luminanceG;
    mirrorRay.luminanceB = ray.luminanceB + plane.luminanceB;

    return mirrorRay;
}
