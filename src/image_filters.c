
#include "image_filters.h"
#include "image_io.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Converte um pixel para tom de cinza usando pesos padrao
unsigned char to_grayscale(struct pixel *pi)
{
    // Formula de Luminosidade
    return (unsigned char)((0.3 * pi->r) + (0.6 * pi->g) + (0.1 * pi->b));
}

struct image * image_to_grayscale(struct image *im)
{
	if (im == NULL)
		return NULL;

	struct image * new = image_new(im->width, im->height,im->maxcor, im->type);

	if (new == NULL)	
		return NULL;

	for (int i = 0; i < im->height; i++)
	{	for (int j = 0; j< im->width; j++)
		{	new->pixel_array[i][j].r = 
			new->pixel_array[i][j].g = 
			new->pixel_array[i][j].b = to_grayscale(&im->pixel_array[i][j]); 
		}	
	}

	return new;
}

// Aplica uma mascara/kernel (Convolucao)

unsigned char aplly_mask(struct image * im , int x, int y, int kernel_width, 
					int kernel_height, float kernel[kernel_width][kernel_height], 
					int sizekernel, int atual_color, int scale, int offset)
{
	float sum = 0.0;

	for (int xlinha = x, aux_i = 0; xlinha < x + sizekernel; xlinha++, aux_i++)
	{	for (int ylinha = y, aux_j = 0; ylinha <  y + sizekernel; ylinha++, aux_j++)
		{	if (xlinha < 0 || xlinha >= im->height || ylinha < 0 || ylinha >= im->width)
				continue;
			if (atual_color == RED)
				sum = sum + im->pixel_array[xlinha][ylinha].r * kernel[aux_i][aux_j];	
			else if (atual_color == GREEN)
				sum = sum + im->pixel_array[xlinha][ylinha].g * kernel[aux_i][aux_j];
			else if (atual_color == BLUE)
				sum = sum + im->pixel_array[xlinha][ylinha].b * kernel[aux_i][aux_j];
		}
	}

	if (round((sum / scale) + offset) < 0)
		return 0;

	if (round((sum / scale) + offset) > 255)
		return 255;
	
	return round((sum / scale) + offset);
}



// Filtro generico de convolucao
struct image * filter(struct image *im, int kernel_width, int kernel_height,  
                                float kernel[kernel_height][kernel_width], int sizekernel, int scale, int offset)
{   
    // ... (Logica para aplicar aplly_mask a todos os pixels e canais) ...
    if (im == NULL) return NULL;
    struct image *new = image_new(im->width, im->height, im->maxcor, im->type);    
    if (new == NULL) return NULL;
    
    // ... (calculo de scale se for -1) ...
    if (scale == -1) scale = sum_scale(kernel_width, kernel_height, kernel);

    int fator_de_subtracao = sizekernel / DIV_MASK;  
    
    for (int i = 0; i < im->height; i++)
    {   for (int j = 0; j < im->width; j++)
        {
            int x = i - fator_de_subtracao;
            int y = j - fator_de_subtracao;
            new->pixel_array[i][j].r = aplly_mask(im, x, y, kernel_width, kernel_height, kernel, sizekernel, RED, scale, offset);
            new->pixel_array[i][j].g = aplly_mask(im, x, y, kernel_width, kernel_height, kernel, sizekernel, GREEN, scale, offset);
            new->pixel_array[i][j].b = aplly_mask(im, x, y, kernel_width, kernel_height, kernel, sizekernel, BLUE, scale, offset);
        }    
    }

    return new;
}


struct elem_centroide new_elem_centroide(int x, int y, 
						int centro_id, unsigned char r,unsigned char g,unsigned char b)
{
	struct elem_centroide new;
	new.pixel.r = r;
	new.pixel.g = g;
	new.pixel.b = b;
	new.x = x;
	new.y = y;
	new.centro_id = centro_id;
	return new;
}

int cmp_int(const void *a, const void *b)
{
	struct elem_centroide *aa = a;
	struct elem_centroide *bb = b;
	if (aa->centro_id < bb->centro_id)
		return -1;
	else  if (aa->centro_id > bb->centro_id)
		return 1;
	else 
		return 0;
}


int update_centroid(struct image *im, struct elem_centroide *base, struct centroide * centroide, int ini)
{
	double dr = 0.0, dg = 0.0, db = 0.0;
	int indice = ini;
	int count = 0;

	while (base[indice].centro_id == centroide->id && indice < (im->width * im->height))
	{	dr+= base[indice].pixel.r * base[indice].pixel.r;
		dg+= base[indice].pixel.g * base[indice].pixel.g;
		db+= base[indice].pixel.b * base[indice].pixel.b;
		count++;
		indice++;	
	}
	
	unsigned char newr;
	double oldr;

	if ((oldr = sqrt(dr /count)) > 255)
		newr = 255;
	else if (oldr < 0)
		newr = 0;	 

	double oldg;
	unsigned char newg;

	if ((oldg = sqrt(dg /count)) > 255)
		newg = 255;
	else if (oldg < 0)
		newg = 0;

	unsigned char newb;
	double oldb;

	if ((oldb = sqrt(db /count)) > 255)
		newb = 255;
	else if (oldb < 0)
		newb = 0;

	centroide->pixel.r = oldr;
	centroide->pixel.g = oldg;
	centroide->pixel.b = oldb;

	//printf("r:%i g:%i b:%i\n", centroide->pixel.r,centroide->pixel.g,centroide->pixel.b);

	return indice;
}




// Implementacao do algoritmo K-Means para quantizacao de cores
struct image * k_means(struct image *im,  int clusters, int max_iterations)
{
	if (im == NULL)
		return NULL;

	if (clusters >= 32)
		return NULL;

	struct centroide *centroides = malloc(sizeof(struct centroide) * clusters);		

	if (centroides == NULL)
		return NULL;
	
	srand(126);
			
	for (int i = 0; i < clusters; i++)
	{	int y = rand() % im->width;
		int x = rand() % im->height;
		struct pixel pi = im->pixel_array[x][y];
		centroides[i] = new_centroide(i, pi.r, pi.g, pi.b);
		printf("id :%i, r:%i g:%i b:%i\n",i, pi.r, pi.g, pi.b );
	}
	
	struct image * im_ret = image_new(im->width, im->height, im->maxcor, im->type);

	if (im_ret == NULL)
	{	free(centroides); 	
		return NULL;
	}
	
	int iterations = 0;
	int distance = 1000;
	int id_cluster = -1;
	int centroides_count = 0;

	struct elem_centroide *elems_centroides = malloc(sizeof(struct elem_centroide) * im->width * im->height);
	
	if (elems_centroides == NULL)
	{	free(centroides);
		free_image(im_ret);
		return NULL;
	}

	while (iterations < max_iterations)	 
	{	centroides_count = 0;
		for (int i = 0; i < im->height; i++)
		{	for (int j = 0; j < im->width; j++)
			{	struct pixel pix = im->pixel_array[i][j];
				for (int k = 0; k < clusters; k++)
				{	int atual = calc_distance_between_colors(pix, centroides[k].pixel);
					//printf("atual:%i distance:%i\n", atual, distance);
					if (atual < distance)
					{	distance   = atual;						
						id_cluster = k;
						//printf("%i \n",id_cluster);
					}
				}
				distance = 1000;
				elems_centroides[centroides_count] = new_elem_centroide(i, j, id_cluster, pix.r, pix.g, pix.b);
				centroides_count++;
			}	
		}
		
		qsort(elems_centroides,im->width*im->height, sizeof(struct elem_centroide), cmp_int);
		
		//for (int m = 0; m < im->width*im->height; m++)
			//printf("id:%i r:%i g:%i b:%i \n", elems_centroides[m].centro_id, elems_centroides[m].pixel.r,elems_centroides[m].pixel.g,elems_centroides[m].pixel.b);
	
		int l   = 0;
		int indice_in_elems_centroides = 0;
		
		while (l < clusters)
		{	indice_in_elems_centroides = update_centroid(im, elems_centroides, &centroides[l], indice_in_elems_centroides);
			l++;
		}	
		
		iterations++;
	}
	
	//atualiza os pixels na nova imagem
	for (int i = 0; i < im->width*im->height; i++)
		im_ret->pixel_array[elems_centroides[i].x][elems_centroides[i].y] = centroides[elems_centroides[i].centro_id].pixel;

	return im_ret;
}


struct image * fill_area_with_similar_colors(struct image *im, unsigned char rfill, 
									unsigned char gfill, unsigned char bfill, struct pixel pi,  int limite)
{
	if (im == NULL)
		return NULL;

	if (limite < 0)	
		return NULL;

	if (rfill > 255|| rfill < 0 || gfill > 255|| gfill < 0 || bfill > 255 || bfill < 0)
		return NULL;

	struct image * new = image_new(im->width, im->height, im->maxcor, im->type);
	
	if (new == NULL)
	{	puts("nao foi possivel criar a imagem");
		return NULL;
	}	
	
	int distance = 0;

	for (int i =0; i < im->height; i++)
	{	for (int j = 0; j< im->width; j++)
		{	
			distance = sqrt(pow(im->pixel_array[i][j].r - pi.r, 2) + 
							pow(im->pixel_array[i][j].g - pi.g, 2) +
                            pow(im->pixel_array[i][j].b - pi.b, 2));

			if (distance >= 0 && distance <= limite)
			{	new->pixel_array[i][j].r = rfill;
				new->pixel_array[i][j].g = gfill;
				new->pixel_array[i][j].b = bfill;	
			}
			else
				new->pixel_array[i][j] = im->pixel_array[i][j];
		}
	}
		
	return new;
}


struct image * oil_painting_filter(struct image *im, int radius)
{
	if (im == NULL)
		return NULL;
		
	struct image *new = image_new(im->width, im->height, im->maxcor, im->type);
	
	if (new == NULL)
		return NULL;

	int fator_subtraction = radius / 2; 
	int i = 0;
	int j = 0;

	for (i = 0; i < im->height; i++)	
	{	for (j = 0; j < im->width; j++)
		{	int intensity_count[INTENSITY_LEVEL+1] = {0};
			float averageR[INTENSITY_LEVEL+1] = {0.0};
			float averageG[INTENSITY_LEVEL+1] = {0.0};
			float averageB[INTENSITY_LEVEL+1] = {0.0}; 
			
			int x = i - fator_subtraction;
			int y = j - fator_subtraction;
			for (int il = x; il < x + radius; il++)
			{	for (int jl = y; jl < y + radius; jl++)
				{	if (il < 0 || il >= im->height || jl < 0 || jl >= im->width)
						continue;
					float r = im->pixel_array[il][jl].r;
					float g = im->pixel_array[il][jl].g;
					float b = im->pixel_array[il][jl].b;
					int cur_intensity = (int)((((r+g+b) / 3.0) * INTENSITY_LEVEL)/255.0);
					intensity_count[cur_intensity]++;
					averageR[cur_intensity]+= r;
					averageG[cur_intensity]+= g;
					averageB[cur_intensity]+= b;
				}	
			}
		
			int maxindex = 0;
			int max      = 0;	

			for (int k = 0; k < INTENSITY_LEVEL; k++)
			{	if (intensity_count[k] > max)			
				{	max      = intensity_count[k];
					maxindex = k;
				}
			}

			unsigned char new_r = (unsigned char) (averageR[maxindex] / max); 
			unsigned char new_g = (unsigned char) (averageG[maxindex] / max);
			unsigned char new_b = (unsigned char) (averageB[maxindex] / max);
			new->pixel_array[i][j].r = new_r;
			new->pixel_array[i][j].g = new_g;
			new->pixel_array[i][j].b = new_b;
		}	
	}

	return new;
	


}


float sum_scale(int kernel_width, int kernel_height, float kernel[kernel_height][kernel_width]) 
{
	float sum = 0;

	for (int i = 0; i< kernel_height; i++)
		for (int j = 0; j < kernel_width; j++)
			sum += kernel[i][j];
	return sum;
}


struct image * sobel2directions(struct image *im)
{

	if (im == NULL)
		return NULL;

	struct image * new = image_new(im->width, im->height, im->maxcor, im->type);
	
	if (new == NULL)
		return NULL;

	float kernelx[3][3] = { -1, 0,1,
							-2, 0,2, 
							-1, 0,1};

	float kernely[3][3] = { 1, 2, 1,
 							0, 0, 0,
						   -1,-2,-1 };
	float gx = 0;
	float gy = 0;

	for (int i = 0; i < im->height; i++)
	{	for (int j = 0; j < im->width; j++)
		{	int x = i - 1;
			int y = j - 1;
			gx = aplly_mask(im, x, y, 3, 3, kernelx, 3, RED, 1, 0);
			gy = aplly_mask(im, x, y, 3, 3, kernely, 3, RED, 1, 0);
			new->pixel_array[i][j].r = MIN(sqrt(gx * gx + gy * gy), 255);
			gx = aplly_mask(im, x, y, 3, 3, kernelx, 3, GREEN, 1,0);
			gy = aplly_mask(im, x, y, 3, 3, kernely, 3, GREEN, 1, 0);
			new->pixel_array[i][j].g = MIN(sqrt(gx * gx + gy * gy), 255);
			gx = aplly_mask(im, x, y, 3, 3, kernelx, 3, BLUE, 1, 0);
			gy = aplly_mask(im, x, y, 3, 3, kernely, 3, BLUE, 1, 0);
			new->pixel_array[i][j].b = MIN(sqrt(gx * gx + gy * gy),255);
		}	
	}	
	return new;
}


int calc_distance_between_colors(struct pixel pixel1, struct pixel pixel2)
{
	int distance = sqrt(pow(pixel1.r - pixel2.r, 2) + pow(pixel1.g - pixel2.g, 2) + pow(pixel1.b - pixel2.b, 2));
	return distance;	
}


struct centroide new_centroide(int id, unsigned char r,unsigned char g,unsigned char b)
{
	struct centroide new;			
	new.pixel.r = r;
	new.pixel.g = g;
	new.pixel.b = b;
	new.id = id;	
	return new;
} 



