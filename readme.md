## Image Processing Library in C

Uma biblioteca de processamento e manipulação de imagens desenvolvida em C, com foco na implementação de algoritmos de processamento digital de imagens, aplicação de filtros e transformações de cores e geometria.

O projeto reúne diferentes técnicas de processamento de imagens, desde operações básicas sobre pixels até algoritmos de segmentação e análise estatística de níveis de cinza.

## Arquitetura

A biblioteca se baseia em imagens `PPM` apenas no formato `raw P6` onde a imagem é represesntada por um cabeçalho seguido dos bytes brutos dos pixels que formam a imagem.

* Estrutura de dados para representação das imagens 
    * A imagem é representada por uma matriz  bidimensional `struct pixel **pixel_array` de estruturas pixel (`struct pixel`) que representam a menor unidadade da imagem.
    
    * A `struct image` representa a imagem por completo com campos para `altura` e `largura`, tipo da imagem `P6 raw` e `maxcor`, que formam a imagem `PPM`. 

        ```c

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
        ```

## Funcionalidades
###  Manipulação de imagens

* Criação e liberação de imagens na memória.
* Carregamento e salvamento de imagens.
* Cópia de imagens.
* Leitura e modificação de pixels individuais.
* Consulta das dimensões e propriedades da imagem.
* Recorte (crop) de regiões específicas.

###  Transformações de cores
* Conversão para escala de cinza (grayscale).
* Binarização por limiar (thresholding) via método de `Otsu`.
* Preenchimento de regiões com cores semelhantes,  utilizando distância entre cores RGB.
* Quantização de cores utilizando o algoritmo K-means.

###  Filtros e efeitos
* Filtro de pintura a óleo (oil painting).
* Detecção de bordas com o operador Sobel, nas direções horizontal e vertical.
* Aplicação de filtros personalizados por meio de matrizes de convolução.

###  Transformações geométricas
* Redimensionamento de imagens (resize).
* Rotação por ângulo.


###  Análise de imagens
* Cálculo de histogramas de níveis de cinza.
* Cálculo de probabilidades dos níveis de intensidade.
* Cálculo de médias globais e médias de classes.
* Cálculo de variâncias entre classes e total.
* Implementação de métricas relacionadas à limiarização, incluindo entropia e critérios de separação de classes.

##  Tecnologias utilizadas
Linguagem: C
* **Bibliotecas padrão:** stdio.h, stdlib.h, string.h
* **Biblioteca matemática:** math.h
* **Compilação:** GCC ou compilador C compatível

##  Como utilizar

1. Compile o projeto
```bash
gcc *.c -o image_processor -lm
```

2. Execute o programa
```bash
./image_processor
```

###  Exemplos de utilização

Os exemplos abaixo ilustram como utilizar algumas das funções da biblioteca. 

 * Criando uma imagem
```c
struct image *img = image_new(800, 600, 255, "P6");

if (img == NULL) {
    printf("Erro ao criar a imagem.\n");
    return 1;
}

/* Utilize a imagem... */

free_image(img);
```

 * Convertendo uma imagem para escala de cinza
```c
struct image *gray = image_to_grayscale(img);

if (gray != NULL) {
    save_image(gray, "grayscale.ppm");
    free_image(gray);
}
```

* Aplicando o filtro Sobel
``` c
struct image *edges = sobel2directions(img);

if (edges != NULL) {
    save_image(edges, "edges.ppm");
    free_image(edges);
}
```
* Redimensionando uma imagem

```c
struct image *resized = image_resize(img, 400, 300);

if (resized != NULL) {
    save_image(resized, "resized.ppm");
    free_image(resized);
}
```

* Aplicando K-means para quantização de cores
``` c
struct image *quantized = k_means(img, 8, 20);

if (quantized != NULL) {
    save_image(quantized, "quantized.ppm");
    free_image(quantized);
}

```

##  Conceitos explorados

Este projeto permite explorar conceitos importantes de processamento digital de imagens e programação em C, como:

* Representação de imagens por matrizes de pixels.
* Manipulação de canais de cor RGB.
* Alocação e liberação dinâmica de memória.
* Convolução com máscaras e kernels.
* Detecção de bordas e extração de características.
* Segmentação e agrupamento de cores com K-means.
* Histogramas, probabilidades e estatística de imagens.
* Transformações geométricas e amostragem de pixels.

## Um exemplo completo.

```c
#include "image_io.h"
#include "image_geo.h"
#include "image_filters.h"
#include "image_analysis.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void liberar_recursos(struct image *img) {
    if (img) free_image(img);
}

int main (void)
{
    
    char input_file[] = "input.ppm";
    
    printf("Carregando imagens: %s\n", input_file);
    struct image *original = image_load(input_file);
    
    
    if (original == NULL) {
        fprintf(stderr, "Erro ao carregar a imagem. Certifique-se de que '%s' existe.\n", input_file);
        return 1;
    }

    struct image *temp = NULL;
    struct image *final_img = NULL;
    
    // --- 2. Exemplo de Uso 1: Rotacao e Filtro Sobel (Deteccao de Bordas) ---
    printf("\n--- Exemplo 1: Rotacao e Deteccao de Bordas (Sobel) ---\n");
    
    // Rotaciona a imagem em 90 graus
    temp = rotate_image_by_angle(original, 90);
    if (temp) {
        printf("Imagem rotacionada em 90 graus.\n");
        // Aplica o filtro Sobel para detectar bordas
        final_img = sobel2directions(temp);
        if (final_img) {
            save_image(final_img, "output_rotacao_sobel.ppm");
            printf("Resultado salvo em: output_rotacao_sobel.ppm\n");
            liberar_recursos(final_img);
        }
        liberar_recursos(temp);
        temp = NULL;
    }
    
    // --- 3. Exemplo de Uso 2: Quantizacao de Cores (K-Means) ---
    printf("\n--- Exemplo 2: Quantizacao de Cores (K-Means) ---\n");
    
    int clusters = 8;
    int max_iter = 10;
    
    // Reduz a paleta de cores para 8 cores usando K-Means
    final_img = k_means(original, clusters, max_iter);
    if (final_img) {
        save_image(final_img, "output_kmeans_8cores.ppm");
        printf("Resultado salvo em: output_kmeans_8cores.ppm (%i clusters)\n", clusters);
        liberar_recursos(final_img);
        final_img = NULL;
    }
    
    // --- 4. Exemplo de Uso 3: Binarizacao com Limiar de Otsu ---
    printf("\n--- Exemplo 3: Binarizacao (Limiarizacao) ---\n");
    
    // Passo 1: Converter para tons de cinza (necessario para o histograma)
    temp = image_to_grayscale(original);
    
    if (temp) {
        // Passo 2: Calcular o histograma
        int *hist = calculate_image_histogram(temp);
        if (hist) {
            // Passo 3: Encontrar o melhor limiar (T) usando o método de Otsu
            // O limiar T é o que maximiza a variância interclasses (sigma2b)
            double max_sigma2b = 0.0;
            int best_T = 0;
            
            for (int T = 0; T < temp->maxcor; T++) {
                double current_sigma2b = sigma2b(T, temp, hist);
                if (current_sigma2b > max_sigma2b) {
                    max_sigma2b = current_sigma2b;
                    best_T = T;
                }
            }
            
            // Passo 4: Binarizar usando o limiar encontrado
            final_img = binarizacao(best_T, temp);
            if (final_img) {
                save_image(final_img, "output_binarizacao_otsu.ppm");
                printf("Resultado salvo em: output_binarizacao_otsu.ppm (Limiar: %d)\n", best_T);
                liberar_recursos(final_img);
            }
            free(hist);
        }
        liberar_recursos(temp);
    }
    
    // --- 5. Liberacao final ---
    liberar_recursos(original);
    
    return 0;
}
```



