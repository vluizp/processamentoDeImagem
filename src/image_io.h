// image_io.h
#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include "image_types.h"
#include <stdlib.h>

// Funcoes de Gerenciamento de Imagem
struct image * image_new(int width, int height, int maxcor, char *type);
void free_image(struct image *im);

// Funcoes de I/O
struct image * image_load(char *image_name);
void save_image(struct image *im, char *name);

// Funcoes de Manipulacao de Pixel
struct pixel get_pixel(struct image *im, int x, int y);
void put_pixel(struct image *im, unsigned char r, unsigned char g, unsigned int b, int x, int y);
struct image * image_copy(struct image *src, struct image *dest);

// Funcoes Auxiliares
void show_image_fields(struct image *im);

#endif // IMAGE_IO_H