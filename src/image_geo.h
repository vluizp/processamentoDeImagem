// image_geo.h
#ifndef IMAGE_GEO_H
#define IMAGE_GEO_H

#include "image_types.h"

// Funcoes Matematicas Auxiliares
double grau2rad(double grau);
struct point new_point2(int x , int y, int angulo); // Assumindo que new_point é a versao que usa a inversa

// Funcoes de Transformacao Geometrica
struct image * rotate_image_by_angle(struct image * im, int graus);
struct image * image_resize(struct image *im, int width, int height);


#endif // IMAGE_GEO_H