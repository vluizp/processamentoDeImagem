#include "image_types.h"
#include "image_analysis.h"
#include "image_io.h"
#include <stdlib.h>
#include <math.h>

//espera se receber uma imagem em tons de cinza
int * calculate_image_histogram(struct image *im)
{
	if (im == NULL)
		return NULL;
	
	int * histogram = calloc(256, sizeof(int));

	if (histogram == NULL) 
 		return NULL;

	for (int i = 0; i < im->height; i++)
	{	for (int j = 0; j < im->width; j++)
		{
			histogram[im->pixel_array[i][j].r]++;
		}
	}
	return histogram;
}

double probabilidade_i_do_nivel_de_cinza(int gray_level, int height, int width, int * histogram) 
{			
	
	double a  = histogram[gray_level];
	double b  = width * height;

	return (a / b);   
}

double Probabilidade_1(int T, int * histogram, int width, int height)
{
	

	double somatorio = 0.0;

	for (int i = 0; i < T; i++)
		somatorio = somatorio + probabilidade_i_do_nivel_de_cinza(i, width, height, histogram);

	//printf("somatorio in Probabilidade_1 =  %f\n", somatorio);
	return somatorio;
}

double Probabilidade_2(int T, int * histogram, int width, int height)
{
	//if (histogram == NULL)
		//return -1;
	double retorno = 1.0 - Probabilidade_1(T, histogram, width, height); 
	printf("retorno  in Probabilidade_2 =  %f\n", retorno);		
	return retorno;
}


double U_zero(int T, int * histogram, int width, int height)
{
	double somatorio = 0.0;
	
	for (double i = 0; i < T; i++)
		somatorio  = somatorio + i * probabilidade_i_do_nivel_de_cinza(i, width, height, histogram);
	
	return somatorio / Probabilidade_1(T, histogram, width, height);  

}


double media_global(int * histogram , int width , int height)
{
	double somatorio = 0.0;
	
	for (double i = 0; i < 256; i++)
		somatorio  = somatorio + i * probabilidade_i_do_nivel_de_cinza(i, width, height, histogram);

	return somatorio;
}

double U_um(int T, int * histogram, int width, int height)
{

	double somatorio = 0.0;
	
	for (double i = 0; i < T; i++)
		somatorio  = somatorio + i * probabilidade_i_do_nivel_de_cinza(i, width, height, histogram);
		
	
	double ut = media_global(histogram, width,height);
	
	return (ut - somatorio)/ (1.0 - Probabilidade_1(T, histogram, width, height));    

}

double media_C1(int T, int * histogram, int width, int height) 
{


	double somatorio = 0.0;
	
	for (double i = 0; i < T; i++)
		somatorio  = somatorio + i * probabilidade_i_do_nivel_de_cinza(i, width, height, histogram);
	
	return 1.0 / Probabilidade_1(T, histogram, width, height) * somatorio;
}


double media_C2(int T, int * histogram, int width, int height) 
{
	double somatorio = 0.0;
	
	for (double i = T + 1; i < 256; i++)
	{	somatorio  = somatorio + i * probabilidade_i_do_nivel_de_cinza(i, width, height, histogram);
	}	
	return (1.0 / Probabilidade_2(T, histogram, width, height)) * somatorio;
}


double sigma(int T, struct image *im, int * histogram)
{

	double p1= Probabilidade_1(T, histogram, im->width, im->height);
	double p2 = Probabilidade_2(T, histogram, im->width, im->height);
	double m1 = media_C1(T, histogram,im->width, im->height);
	double m2 = media_C2(T, histogram,im->width, im->height );
	double mg = media_global(histogram, im->width, im->height);

	return p1 * (pow(m1-mg, 2)) + p2 * (pow(m2 - mg, 2));   
}

double sigma2b(int T, struct image *im, int * histogram)
{

	double p1= Probabilidade_1(T, histogram, im->width, im->height);
	double p2 = Probabilidade_2(T, histogram, im->width, im->height);
	double u0 = U_zero(T, histogram,im->width, im->height);	
	double u1 = U_um(T, histogram,im->width, im->height);	
		
	return p1*p2*pow(u0-u1, 2);	
}

double sigma2t(struct image *im, int * histogram)
{

	double somatorio = 0.0;
	double ut = media_global(histogram, im->width,im->height);	

	for (int i = 0; i < 256; i++)
		somatorio = somatorio + pow(i - ut, 2) * probabilidade_i_do_nivel_de_cinza(i, im->width, im->height, histogram);

	return somatorio;
}
//metodo de pun
float Hb(int * histogram, int T, struct image * im)
{
	float somatorio = 0.0;
	double  logi;
	for(int i = 0; i < T; i++)
	{	double pi = probabilidade_i_do_nivel_de_cinza(i, im->width, im->height, histogram);
		
		if (pi > 0.0)
		{	logi = log(pi);		
			somatorio = somatorio + pi * logi;
		}//printf("%f %f %f\n", logi,pi, somatorio);	 
	}
	return -somatorio;
}


float Hw(int * histogram, int T, struct image * im)
{
	float somatorio = 0.0;
	double  logi;		
	
	for(int i = T + 1; i < 256; i++)
	{	//float pi = probabilidade_i_do_nivel_de_cinza(i, im->width, im->height, histogram);
		double pi = probabilidade_i_do_nivel_de_cinza(i, im->width, im->height, histogram);
		
		if (pi > 0.0)
		{	logi = log(pi);
			somatorio = somatorio +  pi * logi;
		}
		printf("%f %f %f\n", logi,pi, somatorio);	
		//getchar();  
	}
	return -somatorio;
}


struct image * binarizacao(float limiar, struct image * im)
{
	if (im == NULL)
		return NULL;

	struct image *new = image_new(im->width, im->height, im->maxcor, im->type);
	
	for (int i = 0; i < im->height; i++)
	{	for (int j = 0; j < im->width; j++)
		{
			
			if (im->pixel_array[i][j].r <= limiar)
			{	
				new->pixel_array[i][j].r = 0;			
				new->pixel_array[i][j].g = 0; 
				new->pixel_array[i][j].b = 0;
			}
			else if (im->pixel_array[i][j].r >= limiar)
			{	
				new->pixel_array[i][j].r = 255;
				new->pixel_array[i][j].g = 255;
				new->pixel_array[i][j].b = 255;
			}
		}		
	}


	return new;
}

