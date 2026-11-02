// image_types.h
#ifndef IMAGE_TYPES_H
#define IMAGE_TYPES_H

#include <stdio.h>

// --- Constantes e Macros Gerais ---
#define LINE_SIZE 100
#define DIV_MASK  2
#define RED   0
#define GREEN 1
#define BLUE  2
#define MIN(x,y) (x) < (y) ? (x) : (y)
#define INTENSITY_LEVEL 256

// --- Estruturas de Dados ---

struct pixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;        
};

struct image
{
    char type[4];
    int width;
    int height;
    int maxcor;
    struct pixel **pixel_array; // Matriz de pixels [height][width]
};

struct point
{
    int x;
    int y;
};

// Estruturas auxiliares para K-Means
struct centroide
{
    struct pixel pixel;
    int id;
};

struct elem_centroide
{
    struct pixel pixel; 
    int centro_id;
    int x, y;
};

#endif // IMAGE_TYPES_H
