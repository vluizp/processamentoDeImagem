
#include "image_geo.h"
#include "image_io.h"
#include <math.h>
#include <stdlib.h>

double grau2rad(double grau) { return grau / 180.0 * M_PI; }

// Funcao de rotacao que usa a matriz inversa para mapear pixels
struct point new_point2(int x , int y, int angulo)
{
    // Matriz inversa de rotacao
    int px = x * cos(grau2rad(angulo)) + y * sin(grau2rad(angulo)); 
    int py = x * -sin(grau2rad(angulo)) + y * cos(grau2rad(angulo));

    struct point new = {px, py};
    return new;     
}

// Rotaciona a imagem usando a transformada inversa para evitar buracos
struct image * rotate_image_by_angle(struct image * im, int graus)
{
    // ... (Logica de rotacao, usando new_point2) ...
    if (im == NULL) return NULL;
    struct image *new_image = image_new(im->width, im->height, im->maxcor, im->type);
    if (new_image == NULL) return NULL;

    int centrox = (im->height / 2);
    int centroy = (im->width  / 2);  

    for (int x = 0; x < im->height; x++)
    {   for (int y = 0; y < im->width; y++)
        {   
            // 1. Translacao para a origem
            int  xlinha = x - centrox;
            int  ylinha = y - centroy;
            
            // 2. Rotacao inversa
            struct point pxy = new_point2(xlinha, ylinha, graus);
            
            // 3. Translacao de volta
            int new_coordinatex = pxy.x + centrox;
            int new_coordinatey = pxy.y + centroy;
            
            // 4. Mapeamento
            if ((new_coordinatex >= 0) && (new_coordinatex < im->height) && 
                (new_coordinatey >= 0) && ( new_coordinatey < im->width))
                 new_image->pixel_array[x][y] = im->pixel_array[new_coordinatex][new_coordinatey];
        }
    }
    return new_image;
}

// Redimensionamento por Vizinho Mais Próximo
struct image * image_resize(struct image *im, int width, int height)
{
    // ... (Logica de redimensionamento) ...
    if (im  == NULL) return NULL;
    struct image *image_resize = image_new(width, height, im->maxcor, im->type);
    if (image_resize == NULL) return NULL;

    float scale_y  = (float)image_resize->width  / im->width;
    float scale_x  = (float)image_resize->height / im->height;
    
    for (int x = 0; x < image_resize->height; x++)
    {   for (int y = 0; y < image_resize->width; y++)
        {   
            // Mapeamento inverso
            int x_nearest = (int)(round(x / scale_x));
            int y_nearest = (int)(round(y / scale_y));

            if ((x_nearest >= 0) && (x_nearest < im->height) && 
                (y_nearest >= 0) && (y_nearest < im->width))
                image_resize->pixel_array[x][y] = im->pixel_array[x_nearest][y_nearest];
        }
    }
    return image_resize;
}