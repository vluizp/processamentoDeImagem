
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
