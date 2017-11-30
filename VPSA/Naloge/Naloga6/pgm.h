#ifndef INC_PGM
#define INC_PGM
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define HI(num)	(((num) & 0x0000FF00) >> 8)
#define LO(num)	((num) & 0x000000FF)

//PODATKOVNE STRUKTURE
//Podatkovna struktura, ki hrani podatke o sliki
typedef struct {
	    int height;		//višina slike
	    int width;		//širina slike
	    int max_gray;	//maksimalna vrednost sivin
	    int *image;		//polje pikslov na hostu
} PGMData;
//--------------------------------------------------------//
//FUNKCIJE
//Funkcija prebere sliko z imenom filename in jo zapiše v podatkovno strukturo tipa PGMData
PGMData* readPGM(const char *filename, PGMData *data);

//Funkcija zapiše sliko, ki se nahaja v podatkovni stukturi tipa PGMData v datoteko z imenom filename
void writePGM(const char *filename, const PGMData *data);
//--------------------------------------------------------//



void SkipComments(FILE *fp)
	{
	    int ch;
	    char line[100];
	 
	    while ((ch = fgetc(fp)) != EOF && isspace(ch))
	        ;
	    if (ch == '#') {
	        fgets(line, sizeof(line), fp);
	        SkipComments(fp);
	    } else
	        fseek(fp, -1, SEEK_CUR);
	}

PGMData* readPGM(const char *filename, PGMData *data)
{
	FILE *pgmFile;
	char version[3];
	int i, j;
	int lo, hi;

	pgmFile = fopen(filename, "rb");
	if (pgmFile == NULL) {
		perror("Cannot open file to read");
		exit(EXIT_FAILURE);
	}

	fgets(version, sizeof(version), pgmFile);
	if (strcmp(version, "P5")) {
		fprintf(stderr, "Wrong file type!\n");
		exit(EXIT_FAILURE);
	}

	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->width);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->height);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->max_gray);
	fgetc(pgmFile);

	data->image = (int *)malloc(data->height*data->width*sizeof(int));
	if (data->max_gray > 255)
		for (i = 0; i < data->height; ++i)
			for (j = 0; j < data->width; ++j) {
				hi = fgetc(pgmFile);
				lo = fgetc(pgmFile);
				data->image[i*data->width+j] = (hi << 8) + lo;
			}
	else
		for (i = 0; i < data->height; ++i)
			for (j = 0; j < data->width; ++j) {
				lo = fgetc(pgmFile);
				data->image[i*data->width+j] = lo;
			}
	fclose(pgmFile);
	return data;

}

void writePGM(const char *filename, const PGMData *data)
{
	FILE *pgmFile;
	int i, j;
	int hi, lo;
	pgmFile = fopen(filename, "wb");
	if (pgmFile == NULL) {
		perror("Cannot open file to write");
		exit(EXIT_FAILURE);
	}

	fprintf(pgmFile, "P5 ");
	fprintf(pgmFile, "%d %d ", data->width, data->height);
	fprintf(pgmFile, "%d ", data->max_gray);
	if (data->max_gray > 255) {
		for (i = 0; i < data->height; ++i) {
			for (j = 0; j < data->width; ++j) {
				hi = HI(data->image[i*data->width+j]);
				lo = LO(data->image[i*data->width+j]);
				fputc(hi, pgmFile);
				fputc(lo, pgmFile);
			}

		}
	} else {
		for (i = 0; i < data->height; ++i)
			for (j = 0; j < data->width; ++j) {
				lo = LO(data->image[i*data->width+j]);
				fputc(lo, pgmFile);
			}
	}

	fclose(pgmFile);
	free(data->image);
}
#endif