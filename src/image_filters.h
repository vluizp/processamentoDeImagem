// image_filters.h
#ifndef IMAGE_FILTERS_H
#define IMAGE_FILTERS_H

#include "image_types.h"

// Funcoes de Convolucao
float sum_scale(int kernel_width, int kernel_height, float kernel[kernel_height][kernel_width]);
unsigned char aplly_mask(struct image * im , int x, int y, int kernel_width, 
                        int kernel_height, float kernel[kernel_width][kernel_height], 
                        int sizekernel, int atual_color, int scale, int offset);
struct image * filter(struct image *im, int kernel_width, int kernel_height, 
                                float kernel[kernel_height][kernel_width], int sizekernel, int scale, int offset);
struct image * sobel2directions(struct image *im);

// Filtros de Cor e Intensidade
unsigned char to_grayscale(struct pixel *pi);
struct image * image_to_grayscale(struct image *im);
struct image * oil_painting_filter(struct image *im, int radius);
int calc_distance_between_colors(struct pixel pixel1, struct pixel pixel2);
struct image * fill_area_with_similar_colors(struct image *im, unsigned char rfill, 
                                    unsigned char gfill, unsigned char bfill, struct pixel pi, int limite);
// K-Means
struct centroide new_centroide(int id, unsigned char r,unsigned char g,unsigned char b);
struct image * k_means(struct image *im, int clusters, int max_iterations);

#endif // IMAGE_FILTERS_H
