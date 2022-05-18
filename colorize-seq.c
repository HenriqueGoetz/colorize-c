#include <stdio.h>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int dest_width, dest_height;
unsigned char *dest_gs_image;
unsigned char *dest_cf_image;

int orig_width, orig_height;
unsigned char *orig_gs_image;
unsigned char *orig_cf_image;

void clear_images() {

	/* Libera a memória utilizada */

	stbi_image_free(dest_gs_image);
	stbi_image_free(dest_cf_image);
	stbi_image_free(orig_gs_image);
	stbi_image_free(orig_cf_image);
}

int generate_orig_grayscale() {

	/* Calcula os pixels e gera a partir da imagem colorida sua respectiva imagem em tons de cinza. */

	int i, j;
	float r, g, b;

	orig_gs_image = malloc(orig_width * orig_height * 1);
	
	if(orig_gs_image == NULL) {

		return 0;
	}

	for(j = 0; j < orig_height; j++) {

		for(i = 0; i < orig_width; i++) {

			r = orig_cf_image[(orig_width * 3 * j) + (i * 3)];
			g = orig_cf_image[(orig_width * 3 * j) + (i * 3) + 1];
			b = orig_cf_image[(orig_width * 3 * j) + (i * 3) + 2];
			orig_gs_image[(orig_width * j) + i] = (uint8_t) (r * 0.299 + g * 0.587 + b * 0.114);
		}
	}

	return 1;
}

void paint_borders() {

	/* Pinta as bordas da imagem resultado com cor branca. */

	int i, j;

	for(i = 0; i < dest_width * 3; i++) {

		dest_cf_image[i] = (uint8_t) 255;
		dest_cf_image[(dest_width * 3 * (dest_height - 1)) + i] = (uint8_t) 255;
	}

	for(j = 1; j < dest_height - 1; j++) {

		dest_cf_image[(dest_width * 3 * j) + 0] = (uint8_t) 255;
		dest_cf_image[(dest_width * 3 * j) + 1] = (uint8_t) 255;
		dest_cf_image[(dest_width * 3 * j) + 2] = (uint8_t) 255;
		dest_cf_image[(dest_width * 3 * j) + ((dest_width - 1) * 3)] = (uint8_t) 255;
		dest_cf_image[(dest_width * 3 * j) + ((dest_width - 1) * 3) + 1] = (uint8_t) 255;
		dest_cf_image[(dest_width * 3 * j) + ((dest_width - 1) * 3) + 2] = (uint8_t) 255;
	}
}

void get_more_related_pixel(int m[3][3], float *r, float *g, float *b) {

	int i, j;
	int smaller_diff = 255 * 9; // 255 * 9 seria a maior distância possível
	int new_diff;

	/* Para todos os pixels, verifica qual o pixel mais semelhante de acordo com a vizinhança fornecida. */

	for(j = 1; j < orig_height; j++) {

		for(i = 1; i < orig_width; i++) {

			new_diff = 0;
			new_diff += abs(m[0][0] - orig_gs_image[(orig_width * (j-1)) + i - 1]); 
			new_diff += abs(m[0][1] - orig_gs_image[(orig_width * (j-1)) + i]); 
			new_diff += abs(m[0][2] - orig_gs_image[(orig_width * (j-1)) + i + 1]);
			new_diff += abs(m[1][0] - orig_gs_image[(orig_width * (j)) + i - 1]); 
			new_diff += abs(m[1][1] - orig_gs_image[(orig_width * (j)) + i]); 
			new_diff += abs(m[1][2] - orig_gs_image[(orig_width * (j)) + i + 1]); 
			new_diff += abs(m[2][0] - orig_gs_image[(orig_width * (j+1)) + i - 1]); 
			new_diff += abs(m[2][1] - orig_gs_image[(orig_width * (j+1)) + i]); 
			new_diff += abs(m[2][2] - orig_gs_image[(orig_width * (j+1)) + i + 1]);

			/* Se o pixel em análise é mais semelhante, atualiza o smaller_diff e o RGB a ser retornado. */
			if(new_diff < smaller_diff) {

				smaller_diff = new_diff;
				*r = orig_cf_image[(orig_width * 3 * j) + (i * 3)];
				*g = orig_cf_image[(orig_width * 3 * j) + (i * 3) + 1];
				*b = orig_cf_image[(orig_width * 3 * j) + (i * 3) + 2];
			}
		}
	}
}

void colorize_image() {

	int i;

	for(i = 1; i < dest_width - 1; i++) {
	
		int j;

		for(j = 1; j < dest_height - 1; j++) {
					
			float r, g, b;

			/*
				Matriz com o valor dos pixels vizinhos utilizada
			 	para verificar a semelhança dos pixels
			*/

			int m[3][3];
			m[0][0] = dest_gs_image[(dest_width * (j-1) + (i-1))];
			m[0][1] = dest_gs_image[(dest_width * (j-1) + (i))];
			m[0][2] = dest_gs_image[(dest_width * (j-1) + (i+1))];
			m[1][0] = dest_gs_image[(dest_width * (j) + (i+1))];
			m[1][1] = dest_gs_image[(dest_width * (j) + (i))];
			m[1][2] = dest_gs_image[(dest_width * (j) + (i-1))];
			m[2][0] = dest_gs_image[(dest_width * (j+1) + (i-1))];
			m[2][1] = dest_gs_image[(dest_width * (j+1) + (i))];
			m[2][2] = dest_gs_image[(dest_width * (j+1) + (i+1))];


			/* Coleta o RGB do pixel mais semelhante */
			get_more_related_pixel(m, &r, &g, &b);

			/* Pinta o pixel da imagem resultado com o RGB recolhido. */
			dest_cf_image[(dest_width * 3 * j) + (i * 3)] = (uint8_t) r;
			dest_cf_image[(dest_width * 3 * j) + (i * 3) + 1] = (uint8_t) g;
			dest_cf_image[(dest_width * 3 * j) + (i * 3) + 2] = (uint8_t) b;
		}
	}
}

int main(int argc, char *argv[]) {
	
	int dest_channels, orig_channels;

	if(argc != 4) {
		printf("Formato inválido.\nChame colorize grayscale.png colorful.png output\n");
		exit(1);
	}

	/* Carrega as imagens fornecidas com parâmetros*/

	dest_gs_image = stbi_load(argv[1], &dest_width, &dest_height, &dest_channels, 1);
	orig_cf_image = stbi_load(argv[2], &orig_width, &orig_height, &orig_channels, 3);
	
	if(dest_gs_image == NULL || orig_cf_image == NULL) {

		printf("Erro ao carregar as imagens.\n");
		clear_images();
		exit(1);
	}

	if(dest_channels != 1) {

		printf("A imagem em tons de cinza precisa utilizar apenas 1 canal.\n");
		clear_images();
		exit(1);
	}

	if(orig_channels != 3) {

		printf("A imagem colorida precisa utilizar 3 canais.\n");
		clear_images();
		exit(1);
	}

	/* Gera a partir da imagem colorida, sua respectiva imagem em tons de cinza. */
	if(!generate_orig_grayscale()) {

		printf("Erro ao gerar a imagem preto e branco da origem.\n");
		clear_images();
		exit(1);
	}

	/* Aloca memória e pinta as bordas da imagem resultado como a cor branca. */
	dest_cf_image = malloc(dest_width * dest_height * 3);
	paint_borders();

	/* Inicializa o timer */
	double t1, t2;
	t1 = omp_get_wtime();
	
	/* Realiza a tarefa de colorir a imagem grayscale. (Maior complexidade) */
	colorize_image();

	/* Finaliza o timer */
	t2 = omp_get_wtime();
	printf("%f\n", t2 - t1);
	
	/* Escreve a imagem resultado conforme o parêmtro fornecido */

	stbi_write_png(argv[3], dest_width, dest_height, 3, dest_cf_image, dest_width * 3);
	
	/* Libera a memória utilizada. */
	clear_images();

	return 0;
}