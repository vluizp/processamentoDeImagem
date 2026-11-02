
#ifndef IMAGE_ANALYSIS_H
#define IMAGE_ANALYSIS_H

#include "image_types.h"
#include <math.h>

// Funcoes de Histograma
int * calculate_image_histogram(struct image *im);
double probabilidade_i_do_nivel_de_cinza(int gray_level, int height, int width, int * histogram);

// Funcoes de Limiarizacao (Metodo de Otsu/Pun)
double Probabilidade_1(int T, int * histogram, int width, int height);
double Probabilidade_2(int T, int * histogram, int width, int height);
double media_global(int * histogram , int width , int height);
double media_C1(int T, int * histogram, int width, int height);
double media_C2(int T, int * histogram, int width, int height);
double sigma2b(int T, struct image *im, int * histogram);
float Hb(int * histogram, int T, struct image * im); // Entropia de Pun
float Hw(int * histogram, int T, struct image * im); // Entropia de Pun

// Funcao de Binarizacao
struct image * binarizacao(float limiar, struct image * im);

#endif // IMAGE_ANALYSIS_H
