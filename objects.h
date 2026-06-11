#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>

typedef struct {
    float x, y, z;
} vec;

typedef struct {
    vec centre;
    float radius;
} sphere;

#define W 800
#define H 600

#define screen 500 // z-coordinate of the screen

vec camera = {W/2, H/2, 0};
vec light = {W, 0, 0};


// OBJECTS

sphere S1 = {W/3, H/2, screen/1.5, 50};
sphere S2 = {2*W/3, H/2, screen/1.5, 50};
sphere S3 = {W/2, H/3, screen/1.4, 50};

#endif