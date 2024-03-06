#ifndef STRUCTS_H_
#define STRUCTS_H_
#include <stdlib.h>


struct plane {
    float verts[3][3];
    float values[4];
    int R, G, B;
    int roughness;
    int luminanceR;
    int luminanceG;
    int luminanceB;
    float area;
};

struct threadParameters{
    int i;
    int samples;
    int seed;
    size_t totalPlanes;
    struct plane* threadplanes;
};

struct ray {
    // Should be read as a paramatric func:
    // <angx, angy, angz>*u + <modx, mody, modz>
    float modx, mody, modz, angx, angy, angz;
    int R, G, B;
    int bounces;
    int luminanceR;
    int luminanceG;
    int luminanceB;
    float random[4];
};

#endif // STRUCTS_H_
