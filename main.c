#include <stdio.h>
#include <math.h>
#include "objects.h"

typedef struct {
    int r, g, b;
} pixel;

pixel image[H][W]; // this is the image

float min(float a, float b, float c)
{
    if (a <= b && a <= c)
    {
        return a;
    }
    else if (b <= c && b <= a)
    {
        return b;
    }
    else 
    {
        return c;
    }
}

float max(float a, float b, float c)
{
    if (a >= b && a >= c)
    {
        return a;
    }
    else if (b >= c && b >= a)
    {
        return b;
    }
    else 
    {
        return c;
    }
}

float norm(vec ray)
{
    float x = ray.x, y = ray.y, z = ray.z;
    return sqrt(x*x + y*y + z*z);
}

float dot(vec a, vec b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// need to find a way to pass a proper normal into this
// normal is just the direction from which angle is being measured
float rayCos(vec ray, vec normal)
{
    return dot(ray, normal) / (norm(ray) * norm(normal));
}

float raySin(vec ray, vec normal)
{
    return sqrt(1 - pow(rayCos(ray, normal), 2));
}

// returns the distance between initial point and S
// needs repair
// i THINK its fine now
float onSphere(vec ray, vec init, sphere S)
{
    vec normal = {
        S.centre.x - init.x,
        S.centre.y - init.y,
        S.centre.z - init.z
    };
    
    if (rayCos(ray, normal) < 0) // light doesn't look back
    {
        return -1;
    }

    if (raySin(ray, normal) <= S.radius / norm(normal))
    {
        return norm(normal) * rayCos(ray, normal) - sqrt(pow(S.radius, 2) - pow(norm(normal) * raySin(ray, normal), 2));
        //dist from camera to point of sphere
    }
    return -1;
}

// for a ray that's going to strike S it scales the ray to the distance
// between initial and final point
vec ray_len(vec ray, vec init, sphere S)
{
    vec ray_len = {ray.x * onSphere(ray, init, S) / norm(ray),
                   ray.y * onSphere(ray, init, S) / norm(ray),
                   ray.z * onSphere(ray, init, S) / norm(ray)};

    return ray_len;
}

// returns an outward normal at the point where ray strikes S
// needs repair
// i THINK its fixed
vec normal(vec ray, vec init, sphere S)
{
    /*
    if (onSphere(ray, S) == -1)
    {
        vec ret = {0, 0, 0};
        return ret;
    }
    */
    vec toCentre = {
        S.centre.x - init.x,
        S.centre.y - init.y,
        S.centre.z - init.z
    };
    
    vec normal = {
        ray_len(ray, init, S).x - toCentre.x,
        ray_len(ray, init, S).y - toCentre.y,
        ray_len(ray, init, S).z - toCentre.z
    };

    return normal; //outward
}

// matte effect, works fine ig
float diff_illu(vec ray, vec init, sphere S)
{
    vec nm = normal(ray, init, S);
    vec sl = {light.x - S.centre.x, light.y - S.centre.y, light.z - S.centre.z};
    return dot(nm, sl) / (norm(nm) * norm(sl));
}

// returns a vector that is the reflection of ray after striking S
vec reflection(vec ray, vec init, sphere S)
{
    vec nm = normal(ray, init, S);

    vec ref = {
        ray.x - 2 * dot(ray, nm) * nm.x / pow(norm(nm), 2),
        ray.y - 2 * dot(ray, nm) * nm.y / pow(norm(nm), 2),
        ray.z - 2 * dot(ray, nm) * nm.z / pow(norm(nm), 2)
    };

    return ref;
}

// shiny centre glow
float shiny_illu(vec ray, vec init, sphere S)
{
    vec ref = reflection(ray, init, S);

    return -1 * dot(ray, ref) / (norm(ray) * norm(ref));
}

// writes the image data into a ppm file, working fine
void writeFile(pixel image[H][W])
{
    FILE *file = fopen("output.ppm", "w");

    fprintf(file, "P3\n%d %d\n255\n", W, H);

    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            fprintf(file, "%d %d %d  ", image[i][j].r, image[i][j].g, image[i][j].b);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

pixel trace(vec ray, vec init, int depth);

pixel blend (vec ray, vec init, vec ref, sphere S, vec point, int depth)
{
    pixel img = {
        80 + 30*shiny_illu(ray, init, S) + 30*diff_illu(ray, init, S) + 25*trace(ref, point, depth - 1).r / pow(depth, 3),
        80 + 30*shiny_illu(ray, init, S) + 30*diff_illu(ray, init, S) + 25*trace(ref, point, depth - 1).g / pow(depth, 3),
        80 + 30*shiny_illu(ray, init, S) + 30*diff_illu(ray, init, S) + 25*trace(ref, point, depth - 1).b / pow(depth, 3)
    };

    return img;
}

// back tracing a ray coming from a particular pixel on the screen, recursive with a max depth
// needs fixing very much
// i THINK its fine now
pixel trace(vec ray, vec init, int depth)
{
    pixel img;
    vec ref;
    vec nm;
    vec point = init;

    int i;

    float a, b, c;
    a = onSphere(ray, init, S1);
    b = onSphere(ray, init, S2);
    c = onSphere(ray, init, S3);

    if (max(a, b, c) == -1)
    {
        img = (pixel) {0, 0, 20};
        return img;
    }

    if (depth == 0)
    {
        img = (pixel) {0, 0, 0};
        return img;
    }

    if (a == -1) a = 2000;
    if (b == -1) b = 2000;
    if (c == -1) c = 2000;

    if (a == min(a, b, c))
    {
        ref = reflection(ray, init, S1);
        nm = normal(ray, init, S1);
        point.x += ray_len(ray, init, S1).x + 0.001 * nm.x;
        point.y += ray_len(ray, init, S1).y + 0.001 * nm.y;
        point.z += ray_len(ray, init, S1).z + 0.001 * nm.z;
        i = 1;
    }
    else if (b == min(a, b, c))
    {
        ref = reflection(ray, init, S2);
        nm = normal(ray, init, S2);
        point.x += ray_len(ray, init, S2).x + 0.001 * nm.x;
        point.y += ray_len(ray, init, S2).y + 0.001 * nm.y;
        point.z += ray_len(ray, init, S2).z + 0.001 * nm.z;
        i = 2;
    }
    else if (c == min(a, b, c))
    {
        ref = reflection(ray, init, S3);
        nm = normal(ray, init, S3);
        point.x += ray_len(ray, init, S3).x + 0.001 * nm.x;
        point.y += ray_len(ray, init, S3).y + 0.001 * nm.y;
        point.z += ray_len(ray, init, S3).z + 0.001 * nm.z;
        i = 3;
    }

    switch (i)
    {
        case 1:
        {
            img = blend(ray, init, ref, S1, point, depth - 1);
            img.r += 50;
            break;
        }
        case 2:
        {
            img = blend(ray, init, ref, S2, point, depth - 1);
            img.g += 50;
            break;
        }
        case 3:
        {
            img = blend(ray, init, ref, S3, point, depth - 1);
            img.b += 50;
            break;
        }
    }

    return img;
}

int main()
{
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            vec dirn = {
                j - camera.x, i - camera.y, screen
            };

            image[i][j] = trace(dirn, camera, 10);
        }
    }

    writeFile(image);

    return 0;
}