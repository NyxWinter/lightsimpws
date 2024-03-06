#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "structs.h"
#include "getPlane.h"
#include "intersections.h"


const int screenWidth = 1920;
const int screenHeight = 1080;

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;



int drawScreen(char* argv[]){
    // "Camera":
    // focalLength doesn't represent any actual realworld focalLength.
    float focalLength = 2000.0; // The amount of times I've misspelled "length"...
    // Should be in the 3d file
    int samples = 1;
    // Should be in the 3d file
    int maxBounces = 5;
    // Random numbers are slow
    size_t seed = 2393203;
    srand(seed);
    // DoF coming soon(tm) /s

    // defaultRay is a boring ray
    struct ray defaultRay;

    if (argv[1] == NULL) {
        perror("Please add a file.\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    // If you messed up writing your 3d files you can mass-edit values here
    float multiplier = 1;
    float stretchStart = 0;
    float stretch = 0;
    int updateroom = 0;

    // I *DON'T* want to type that whole thing again. // IGNORE this
    if (!strcmp(argv[1], "updatedroom.r3f")){ // Love it when your IDE just tells you NO WORKY DO THIS INSTEAD (verbatim)
        printf("Succes\n");
        multiplier = 0.5;
        stretch = 500;
        stretchStart = 200;
        updateroom = 1;
    }

    if (file == NULL) {
        perror("Error opening file. Please fix your file.\n");
        return 1;
    }

    // <Reading the file>
    size_t totalPlanes;
    fscanf(file, "%lu : Total Planes", &totalPlanes);
    fscanf(file, "%d : Samples", &samples);
    fscanf(file, "%d : Max Bounces", &maxBounces);
    struct plane* planes = (struct plane*)malloc(totalPlanes * sizeof(struct plane));
    char dash;
    for (size_t i = 0; i < totalPlanes; ++i){
        fscanf(file, "%c", &dash);
        for (int j = 0; j < 3; ++j){
            fscanf(file, "%f, %f, %f", &planes[i].verts[j][0], &planes[i].verts[j][1], &planes[i].verts[j][2]);
            if (planes[i].verts[j][1] > stretchStart){
                planes[i].verts[j][1] += stretch;
            }
            if (updateroom && (fabsf(planes[i].verts[j][2]) == 400)){
                planes[i].verts[j][2] *= 2.5;
            }
            planes[i].verts[j][0] *= multiplier;
            planes[i].verts[j][1] *= multiplier;
            planes[i].verts[j][2] *= multiplier;
        }
        fscanf(file, "%d : Roughness", &planes[i].roughness);
        fscanf(file, "%d, %d, %d : RGB", &planes[i].R, &planes[i].G, &planes[i].B);
        fscanf(file, "%c", &dash);
        fscanf(file, "%d, %d, %d : RGB Luminance", &planes[i].luminanceR, &planes[i].luminanceG, &planes[i].luminanceB);
    }
    // </Reading the file>
    fclose(file);

    for (size_t i = 0; i < totalPlanes; i++){
        // Gets the equation for each plane
        float *vertValues = getPlane(planes[i]);
        planes[i].values[0] = vertValues[0];
        planes[i].values[1] = vertValues[1];
        planes[i].values[2] = vertValues[2];
        planes[i].values[3] = vertValues[3];

        printf("Plane %lu: %fx + %fy + %fz + %f = 0\n", i, planes[i].values[0], planes[i].values[1], planes[i].values[2], planes[i].values[3]);
        free(vertValues);
    }

    int R = 0;
    int G = 0;
    int B = 0;
    printf("Starting draw\n");
    for (int i = 0; i < screenWidth; i++) {
        // Angle of X dependent on where it is on the screen. Aka: perspective (which is 1:1 tied to the resolution)
        defaultRay.modx = ((float)i - (float)(screenWidth >> 1));
        defaultRay.mody = 0.0;

        for (int j = 0; j < screenHeight; j++){
            // Angle of Y depends on where it is on the screen: perspective again.
            defaultRay.modz = -((float)j - (float)(screenHeight >> 1));
            // Set up default ray with default values
            defaultRay.angx = defaultRay.modx / focalLength;
            defaultRay.angy = 1;
            defaultRay.angz = defaultRay.modz / focalLength;
            defaultRay.R = 0;
            defaultRay.G = 0;
            defaultRay.B = 0;
            defaultRay.luminanceR = 0;
            defaultRay.luminanceG = 0;
            defaultRay.luminanceB = 0;
            defaultRay.bounces = 0;
            struct ray finalRay;
            R = 0;
            G = 0;
            B = 0;
            for (int k = 0; k < samples; k++){
                // Makes new random numbers for each line for that sweet, sweet roughness.
                defaultRay.random[0] = (float)rand();
                defaultRay.random[1] = (float)rand();
                defaultRay.random[2] = (float)rand();
                defaultRay.random[3] = (float)rand();
                finalRay = intersectAll(planes, defaultRay, totalPlanes, maxBounces);
                R += finalRay.R;
                G += finalRay.G;
                B += finalRay.B;
            }
            R /= samples;
            G /= samples;
            B /= samples;
            if (R > 255)
                R = 255;
            if (G > 255)
                G = 255;
            if (B > 255)
                B = 255;
            // Draws the calculated pixel
            SDL_SetRenderDrawColor(renderer, R, G, B, 255);
            SDL_RenderDrawPoint(renderer, i, j);
        }
        // Only presents every time a row is completed because SDL is really slow
        SDL_RenderPresent(renderer);
    }

    free(planes);
    return 0;
}


int initSDL(char* argv[]){
    // Creates SDL environment
    window = SDL_CreateWindow("Lightsim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);
    // Calls drawscreen, passing through the arg
    if (SDL_RENDERER_ACCELERATED)
        printf("Accel\n");

    if (drawScreen(argv))
        return 1;

    // SDL Draw loop. Redraws a new frame every time a key is pressed.
    while (1) {
        SDL_RenderPresent(renderer);
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                // Kills SDL on Escape
                case SDLK_ESCAPE:
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return EXIT_SUCCESS;
                    break;
            }
        }
    }

    return 0;
}

int main(int argc, char* argv[]){
    initSDL(argv);
    return 0;
}
