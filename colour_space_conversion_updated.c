#include <stdio.h>
#include <stdlib.h>

//rgb is a rows*cols*3 array of unsigned char (ie [0, 255])
//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toYCbCr(unsigned char *rgb, unsigned char *ycbcr, int rows, int cols)
{
	float r1, g1, b1, y1, cb1, cr1;
	float r2, g2, b2, y2, cb2, cr2;
	float r3, g3, b3, y3, cb3, cr3;
	float r4, g4, b4, y4, cb4, cr4;
	float cbOut, crOut;
	int j = 0; // ycbcr output array position 	
	int rgbLen = rows*cols*3;

	for(int i = 0; i < rgbLen; i += 12)
	{
		// Pixel 1
		r1 = (float) rgb[i];
		g1 = (float) rgb[i+1];
		b1 = (float) rgb[i+2];
		y1  =  16.0 + 0.257*r1 + 0.504*g1 + 0.098*b1; //should be guaranteed to be in range of [0, 255]
		cb1 = 128.0 - 0.148*r1 - 0.291*g1 + 0.439*b1; //should be guaranteed to be in range of [0, 255]
		cr1 = 128.0 + 0.439*r1 - 0.368*g1 - 0.071*b1; //should be guaranteed to be in range of [0, 255]
		 
		// Pixel 2
		r2 = (float) rgb[i+3];
		g2 = (float) rgb[i+4];
		b2 = (float) rgb[i+5];
		y2  =  16.0 + 0.257*r2 + 0.504*g2 + 0.098*b2; //should be guaranteed to be in range of [0, 255]
		cb2 = 128.0 - 0.148*r2 - 0.291*g2 + 0.439*b2; //should be guaranteed to be in range of [0, 255]
		cr2 = 128.0 + 0.439*r2 - 0.368*g2 - 0.071*b2; //should be guaranteed to be in range of [0, 255]

		// Pixel 3
		r3 = (float) rgb[i+6];
		g3 = (float) rgb[i+7];
		b3 = (float) rgb[i+8];
		y3  =  16.0 + 0.257*r3 + 0.504*g3 + 0.098*b3; //should be guaranteed to be in range of [0, 255]
		cb3 = 128.0 - 0.148*r3 - 0.291*g3 + 0.439*b3; //should be guaranteed to be in range of [0, 255]
		cr3 = 128.0 + 0.439*r3 - 0.368*g3 - 0.071*b3; //should be guaranteed to be in range of [0, 255]

		// Pixel 4
		r4 = (float) rgb[+9];
		g4 = (float) rgb[i+10];
		b4 = (float) rgb[i+11];
		y4  =  16.0 + 0.257*r4 + 0.504*g4 + 0.098*b4; //should be guaranteed to be in range of [0, 255]
		cb4 = 128.0 - 0.148*r4 - 0.291*g4 + 0.439*b4; //should be guaranteed to be in range of [0, 255]
		cr4 = 128.0 + 0.439*r4 - 0.368*g4 - 0.071*b4; //should be guaranteed to be in range of [0, 255]
		
		// Perform downsampling
		
		// simple
		cbOut = cb1; 
		crOut = cr1;
		
		// average
		cbOut = (cb1 + cb2 + cb3 + cb4) / 4;
		crOut = (cr1 + cr2 + cr3 + cr4) / 4;


		// Set output values
		// is using j++ better or worse than using j, j+1, j+2... then j+=6 after?
		ycbcr[j++] = (unsigned char) y1;	// pixel 1 Y'
		ycbcr[j++] = (unsigned char) cbOut; // pixel 1, 2, 3, 4 Cb
		ycbcr[j++] = (unsigned char) crOut; // pixel 1, 2, 3, 4 Cr
		ycbcr[j++] = (unsigned char) y2;	// pixel 2 Y'
		ycbcr[j++] = (unsigned char) y3;	// pixel 3 Y'
		ycbcr[j++] = (unsigned char) y4;	// pixel 4 Y'
	}
}

//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255])
//rgb is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toRGB(unsigned char *ycbcr, unsigned char *rgb, int rows, int cols)
{
	float r1, g1, b1;
	float r2, g2, b2;
	float r3, g3, b3;
	float r4, g4, b4;
	float y16_1, cb128, cr128, y16_2, y16_3, y16_4;
	float rP2, gP2, bP2;
	int j = 0; // rgb output array position 	
	int ycbcrLen = rows*cols*3/2;

	for(int i = 0; i < ycbcrLen; i += 6)
	{
		y16_1 = (float) ycbcr[i] - 16.0;
		cb128 = (float) ycbcr[i+1] - 128.0;
		cr128 = (float) ycbcr[i+2] - 128.0;		
		y16_2 = (float) ycbcr[i+3] - 16.0;
		y16_3 = (float) ycbcr[i+4] - 16.0;
		y16_4 = (float) ycbcr[i+5] - 16.0;

		rP2 = 1.596*cr128;
		gP2 = 0.813*cr128 - 0.391*cb128;
		bP2 = 2.018*cb128;

		r1 = 1.164*y16_1 + rP2; //this might exceed 255
		g1 = 1.164*y16_1 - gP2; //this could be less than 0 or greater than 255
		b1 = 1.164*y16_1 + bP2; //this might exceed 255

		r2 = 1.164*y16_2 + rP2; //this might exceed 255
		g2 = 1.164*y16_2 - gP2; //this could be less than 0 or greater than 255
		b2 = 1.164*y16_2 + bP2; //this might exceed 255

		r3 = 1.164*y16_3 + rP2; //this might exceed 255
		g3 = 1.164*y16_3 - gP2; //this could be less than 0 or greater than 255
		b3 = 1.164*y16_3 + bP2; //this might exceed 255

		r4 = 1.164*y16_4 + rP2; //this might exceed 255
		g4 = 1.164*y16_4 - gP2; //this could be less than 0 or greater than 255
		b4 = 1.164*y16_4 + bP2; //this might exceed 255

		// Possibly do error checking here to ensure rX, gX, bX are in range [0, 255]

		rgb[j++] = (unsigned char) r1;
		rgb[j++] = (unsigned char) g1;
		rgb[j++] = (unsigned char) b1;

		rgb[j++] = (unsigned char) r2;
		rgb[j++] = (unsigned char) g2;
		rgb[j++] = (unsigned char) b2;

		rgb[j++] = (unsigned char) r3;
		rgb[j++] = (unsigned char) g3;
		rgb[j++] = (unsigned char) b3;

		rgb[j++] = (unsigned char) r4;
		rgb[j++] = (unsigned char) g4;
		rgb[j++] = (unsigned char) b4;
	}
}

int main(void)
{
	int rows, cols, components;
	scanf("%d %d %d", &rows, &cols, &components);

	unsigned char rgb[rows*cols*components];
	unsigned char ycbcr[rows*cols*components];

	fprintf(stderr, "%lu\n", sizeof(rgb));

	unsigned char *ptrR = rgb;
	unsigned char *ptrG = rgb + 1;
	unsigned char *ptrB = rgb + 2;

	while(scanf("%hhu %hhu %hhu", ptrR, ptrG, ptrB) != EOF)
	{
		ptrR += 3;
		ptrG += 3;
		ptrB += 3;
	}

	fprintf(stderr, "Converting to YCbCr...\n");
	toYCbCr(rgb, ycbcr, rows, cols);

	for(int i = 0; i < rows*cols*components/2; ++i)
	{
		printf("%hhu ", ycbcr[i]);
	}
	
	printf("\n\n");

	fprintf(stderr, "Converting to RGB...\n");
	toRGB(ycbcr, rgb, rows, cols);

	fprintf(stderr, "%lu\n", sizeof(rgb));

	fprintf(stderr, "Writing output...\n");
	for(int i = 0; i < rows*cols*components; ++i)
	{
		printf("%hhu ", rgb[i]);
	}
	
	printf("\n\n");
}




