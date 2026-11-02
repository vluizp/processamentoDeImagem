

#include "image_io.h"
#include <string.h>
#include <stdlib.h>


void free_image(struct image *im)
{	if (im == NULL)
		return;	

	for (int i = 0; i < im->height; i++)
		free(im->pixel_array[i]);

	free(im->pixel_array);
	free(im);
	return;
}
void show_image_fields(struct image *im)
{
	if (im == NULL)
	{	printf("image == NULL");
		return;
	}
	printf("type: %s\nwidth: %d, height: %d\nmaxcor: %d\n", im->type, im->width, im->height, im->maxcor);	
	return;
}


// Cria uma nova estrutura de imagem e aloca a matriz de pixels
struct image * image_new(int width, int height, int maxcor, char *type)
{
    if (width <= 0 || height <= 0 || maxcor <= 0 || maxcor >= 256) return NULL;
    
    struct image *im = (struct image *)malloc(sizeof(struct image));
    if (!im) return NULL;
      
    strcpy(im->type, type ? type : "P6");
    im->width  = width;
    im->height = height;
    im->maxcor = maxcor; 

    im->pixel_array = (struct pixel **)malloc(sizeof(struct pixel *) * height);
    if (!im->pixel_array) { free(im); return NULL; }

    for (int i = 0; i < im->height; i++) {
        im->pixel_array[i] = (struct pixel *)malloc(sizeof(struct pixel) * im->width);
        if (!im->pixel_array[i]) { 
            for (int j = 0; j < i; j++) free(im->pixel_array[j]);
            free(im->pixel_array); free(im); return NULL;
        }    
    }
    return im;
}


struct pixel get_pixel(struct image *im, int x, int y)
{
	struct pixel pixel_default = {0,0,0}; 
	
	if (im == NULL)	
		return pixel_default;

	if ((x >= 0) && (x < im->height) &&  (y >= 0) && (y < im->width))		
		return im->pixel_array[x][y];
 
	return pixel_default;
}		

void put_pixel(struct image *im, unsigned char r, unsigned char g, unsigned int b, int x, int y)
{
	if (im == NULL)
		return;
			
	if ((x >= 0) && (x < im->height) &&  (y >= 0) && (y < im->width))		
	{	im->pixel_array[x][y].r = r;
		im->pixel_array[x][y].g = g;
		im->pixel_array[x][y].b = b;
		return;
	}
}

void save_image(struct image *im, char *name)
{
	if (im == NULL || name == NULL)
	{	
		return;
	}

	FILE *fd = fopen(name, "w");

	if (fd == NULL)
	{	printf("nao foi possivel criar a imagem");
		return;
	}

	fprintf(fd, "%s\n", im->type);
	fprintf(fd, "%d %d\n", im->width, im->height);
	fprintf(fd,"%d\n", im->maxcor);

	for (int i = 0; i < im->height; i++)				
	{	if ((fwrite(im->pixel_array[i], sizeof(struct pixel), im->width, fd)) != im->width)
		{	puts("falha na escrita da imagem");			
			fclose(fd);			
			break;
		}
	}
	fclose(fd);
	return;		
}


struct image * image_load(char *image_name)
{
	if (image_name == NULL)
		return NULL;

	char *line = malloc(LINE_SIZE); 

	if (line == NULL)
	{	puts("falha na alocaçao de memoria");
		return NULL;
	}

	struct image * im = malloc (sizeof(struct image));
	
	if (im == NULL)
	{	puts("falha na alocaçao de memoria");
		free(line);
		return NULL;
	}
			
	FILE * file = fopen(image_name, "r");

	if (file == NULL)
	{	puts("nao foi possivel abrir a imagem");
		free(im);
		free(line);
		return NULL;
	}

	fgets(line, LINE_SIZE, file);
	sscanf(line, "%s", im->type);
	fgets(line, LINE_SIZE, file); 	  	
	sscanf(line, "%d%d", &im->width, &im->height);
	fgets(line, LINE_SIZE, file);
	sscanf(line, "%d", &im->maxcor);

	if ((im->pixel_array = malloc (sizeof(struct pixel*) * im->height)) == NULL) 
	{	puts("falha na  alocaçao de memoria");	
		free(im);
		free(line);
		return NULL;
	}	

	for(int i = 0; i < im->height; i++)
	{	im->pixel_array[i] = malloc(sizeof(struct pixel)* im->width);		
		fread(im->pixel_array[i], sizeof(struct pixel), im->width, file);
	}

	printf("%s\nwidth :%i height :%i\nmaxcolor %i\n", im->type, im->width, im->height, im->maxcor);

	fclose(file);

	return im;	
} 

struct image * image_copy(struct image *src, struct image *dest)
{
	if (src == NULL || dest == NULL)
		return NULL;

	if ((src->width != dest->width) || (src->height != dest->height))
		return NULL;

	for (int i = 0; i < src->height; i++)
	{	for (int j = 0; j < src->width; j++)
			dest->pixel_array[i][j] = src->pixel_array[i][j];
	}

	return dest;
}


