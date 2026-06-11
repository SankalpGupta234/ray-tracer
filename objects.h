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

#define W 1600
#define H 1200

#define screen 1000 // z-coordinate of the screen

vec camera = {W/2, H/2, 0};
vec camera2 = {W/2 + 5, H/2, 0};
vec camera3 = {W/2, H/2 + 5, 0};
vec camera4 = {W/2 + 3, H/2 + 3, 0};
vec light = {W, H/2, 0};


// OBJECTS

sphere S1 = {W/2.8, H/2, screen/1.7, 100};
sphere S2 = {W/1.6, H/2.3, screen/1.5, 100};
sphere S3 = {W/2, H/3, screen/1.2, 100};

#endif