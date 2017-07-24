#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

//rgb is a rows*cols*3 array of unsigned char (ie [0, 255])
//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toYCbCr(uint8_t * restrict rgb, uint8_t * restrict ycbcr, uint16_t rows, uint16_t cols)
{
	int r1, g1, b1, y1, cb1, cr1;
	int r2, g2, b2, y2, cb2, cr2;
	int r3, g3, b3, y3, cb3, cr3;
	int r4, g4, b4, y4, cb4, cr4;
	int cbOut, crOut;
	int j = 0; // ycbcr output array position 	
	int rgbLen = rows*cols*3;

	for(int i = rgbLen - 12; i > 0; i -= 12)
	{
		// Pixel 1
		r1 = rgb[i];
		g1 = rgb[i+1];
		b1 = rgb[i+2];
		y1  =  16 + ((16483*r1 + 33030*g1 + 6423*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cb1 = 128 + ((-9699*r1 - 19071*g1 + 28770*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cr1 = 128 + ((28770*r1 - 24117*g1 - 4653*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		 
		// Pixel 2
		r2 = rgb[i+3];
		g2 = rgb[i+4];
		b2 = rgb[i+5];
		y2  =  16 + ((16483*r2 + 33030*g2 + 6423*b2) >> 16); //should be guaranteed to be in range of [0, 255]
		cb2 = 128 + ((-9699*r2 - 19071*g2 + 28770*b2) >> 16); //should be guaranteed to be in range of [0, 255]
		cr2 = 128 + ((28770*r2 - 24117*g2 - 4653*b2) >> 16); //should be guaranteed to be in range of [0, 255]

		// Pixel 3
		r3 = rgb[i+6];
		g3 = rgb[i+7];
		b3 = rgb[i+8];
		y3  =  16 + ((16483*r3 + 33030*g3 + 6423*b3) >> 16); //should be guaranteed to be in range of [0, 255]
		cb3 = 128 + ((-9699*r3 - 19071*g3 + 28770*b3) >> 16); //should be guaranteed to be in range of [0, 255]
		cr3 = 128 + ((28770*r3 - 24117*g3 - 4653*b3) >> 16); //should be guaranteed to be in range of [0, 255]

		// Pixel 4
		r4 = rgb[i+9];
		g4 = rgb[i+10];
		b4 = rgb[i+11];
		y4  =  16 + ((16483*r4 + 33030*g4 + 6423*b4) >> 16); //should be guaranteed to be in range of [0, 255]
		cb4 = 128 + ((-9699*r4 - 19071*g4 + 28770*b4) >> 16); //should be guaranteed to be in range of [0, 255]
		cr4 = 128 + ((28770*r4 - 24117*g4 - 4653*b4) >> 16); //should be guaranteed to be in range of [0, 255]
		
		// Perform downsampling
	
		// simple (From my testing, this actually seems to result in better images. Strange...)
//		cbOut = cb1; 
//		crOut = cr1;
		
		// average
		cbOut = (cb1 + cb2 + cb3 + cb4) / 4;
		crOut = (cr1 + cr2 + cr3 + cr4) / 4;


		// Set output values
		// is using j++ better or worse than using j, j+1, j+2... then j+=6 after?
		ycbcr[j++] = y1;	// pixel 1 Y'
		ycbcr[j++] = cbOut; // pixel 1, 2, 3, 4 Cb
		ycbcr[j++] = crOut; // pixel 1, 2, 3, 4 Cr
		ycbcr[j++] = y2;	// pixel 2 Y'
		ycbcr[j++] = y3;	// pixel 3 Y'
		ycbcr[j++] = y4;	// pixel 4 Y'
	}
}

//It turns out this actually is important because without it
//we get some funky results
int clamp(int n)
{
    if(n > 255)
    {
        fprintf(stderr, "int %d out of range\n", n);
        return 255;
    }
    else if(n < 0)
    {
        fprintf(stderr, "int %d out of range\n", n);
        return 0;
    }
    return n;
}

//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255])
//rgb is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toRGB(uint8_t * restrict ycbcr, uint8_t * restrict rgb, uint16_t rows, uint16_t cols)
{
	int r1, g1, b1;
	int r2, g2, b2;
	int r3, g3, b3;
	int r4, g4, b4;
	int y16_1, cb128, cr128, y16_2, y16_3, y16_4;
	int rP2, gP2, bP2, yP2;
	int j = 0; // rgb output array position 	
	int ycbcrLen = rows*cols*3 >> 1;

	for(int i = ycbcrLen - 6; i > 0; i -= 6)
	{
		y16_1 = ycbcr[i] - 16;
		cb128 = ycbcr[i+1] - 128;
		cr128 = ycbcr[i+2] - 128;		
		y16_2 = ycbcr[i+3] - 16;
		y16_3 = ycbcr[i+4] - 16;
		y16_4 = ycbcr[i+5] - 16;

		rP2 = 52298*cr128 >> 15;
		gP2 = (-53281*cr128 - 25625*cb128) >> 16;
		bP2 = 33063*cb128 >> 14;

		yP2 = 38142*y16_1 >> 15;
		r1 = clamp(yP2 + rP2); //this might exceed 255
		g1 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		b1 = clamp(yP2 + bP2); //this might exceed 255

		yP2 = 38142*y16_2 >> 15;
		r2 = clamp(yP2 + rP2); //this might exceed 255
		g2 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		b2 = clamp(yP2 + bP2); //this might exceed 255

		yP2 = 38142*y16_3 >> 15;
		r3 = clamp(yP2 + rP2); //this might exceed 255
		g3 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		b3 = clamp(yP2 + bP2); //this might exceed 255

		yP2 = 38142*y16_4 >> 15;
		r4 = clamp(yP2 + rP2); //this might exceed 255
		g4 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		b4 = clamp(yP2 + bP2); //this might exceed 255

		// Possibly do error checking here to ensure rX, gX, bX are in range [0, 255]

		rgb[j++] = r1;
		rgb[j++] = g1;
		rgb[j++] = b1;

		rgb[j++] = r2;
		rgb[j++] = g2;
		rgb[j++] = b2;

		rgb[j++] = r3;
		rgb[j++] = g3;
		rgb[j++] = b3;

		rgb[j++] = r4;
		rgb[j++] = g4;
		rgb[j++] = b4;
	}
}

int main(void)
{
	int rows, cols, components;
	scanf("%d %d %d", &rows, &cols, &components);

	assert(rows % 2 == 0);
	assert(cols % 2 == 0);
	assert(components == 3);

	uint8_t *rgb = malloc(rows*cols*components*sizeof(unsigned char));
	uint8_t *ycbcr = malloc(rows*cols*components*sizeof(unsigned char) >> 1);

	fprintf(stderr, "%lu\n", sizeof(rgb));

	uint8_t *ptrR = rgb;
	uint8_t *ptrG = rgb + 1;
	uint8_t *ptrB = rgb + 2;

	while(scanf("%hhu %hhu %hhu", ptrR, ptrG, ptrB) != EOF)
	{
		ptrR += 3;
		ptrG += 3;
		ptrB += 3;
	}

	fprintf(stderr, "Converting to YCbCr...\n");
	toYCbCr(rgb, ycbcr, rows, cols);
	fprintf(stderr, "Size of YCbCr: %lu\n", sizeof(rgb));	

	fprintf(stderr, "Converting to RGB...\n");
	toRGB(ycbcr, rgb, rows, cols);
	fprintf(stderr, "Size of RGB: %lu\n", sizeof(rgb));

	fprintf(stderr, "Writing output...\n");
    printf("%d %d %d\n", rows, cols, components);
	for(int i = 0; i < rows*cols*components; ++i)
	{
		printf("%hhu ", rgb[i]);
	}
	
	printf("\n\n");

	free(rgb);
	free(ycbcr);
}




