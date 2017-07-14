#include <stdio.h>
#include <stdlib.h>

//rgb is a rows*cols*3 array of unsigned char (ie [0, 255])
//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toYCbCr(unsigned char *rgb, unsigned char *ycbcr, int rows, int cols)
{
	for(int i = 0; i < rows*cols*3; i += 3)
	{
		float r = (float) rgb[i];
		float g = (float) rgb[i+1];
		float b = (float) rgb[i+2];

		float y = 16.0 + 0.257*r + 0.504*g + 0.098*b; //should be guaranteed to be in range of [0, 255]
		float cb = 128.0 - 0.148*r - 0.291*g + 0.439*b; //should be guaranteed to be in range of [0, 255]
		float cr = 128.0 + 0.439*r - 0.368*g - 0.071*b; //should be guarantted to be in range of [0, 255]

		ycbcr[i] = (unsigned char) y;
		ycbcr[i+1] = (unsigned char) cb;
		ycbcr[i+2] = (unsigned char) cr;

		//Apparently we are supposed to do some sort of downsampling by 4.
		//My guess is that the purpose of the downsampling is to reduce the number of bytes
		//required to store the pixels by a factor of 4 and that when you convert back to RGB
		//by later upsampling, we ideally want a minimal amount of visual artifacts. In other words,
		//if I understand what we are supposed to do correctly, then really our ycbcr array should be
		//about 1/4 as long as the rgb array
	}
}

//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255])
//rgb is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toRGB(unsigned char *ycbcr, unsigned char *rgb, int rows, int cols)
{
	for(int i = 0; i < rows*cols*3; i += 3)
	{
		float y16 = (float) ycbcr[i] - 16.0;
		float cb128 = (float) ycbcr[i+1] - 128.0;
		float cr128 = (float) ycbcr[i+2] - 128.0;

		float r = 1.164*y16 + 1.596*cr128; //this might exceed 255
		float g = 1.164*y16 - 0.813*cr128 - 0.391*cb128; //this could be less than 0 or greater than 255
		float b = 1.164*y16 + 2.018*cb128; //this might exceed 255

		if(r > 255)
		{
			r = 255.0;
		}

		if(g < 0)
		{
			g = 0;
		}
		else if(g > 255)
		{
			g = 255;
		}

		if(b > 255)
		{
			b = 255;
		}

		rgb[i] = (unsigned char) r;
		rgb[i+1] = (unsigned char) g;
		rgb[i+2] = (unsigned char) b;

		//We are supposed to do some sort of upsampling. When we do this
		//the new rgb array should look mostly like the original rgb array
		//although there would likely be some artifacts due to the fact that
		//the downsampling process (during toYCbCr) will cause us to lose some information
	}
}

int main(void)
{

	FILE *fptr = fopen("../data/rgb/7wonders.txt", "r");
	if (fptr == NULL)
	{
		printf("Failed to open file\n");
		return 1;
	}

	int rows, cols, components;
	fscanf(fptr, "%d %d %d", &rows, &cols, &components);

	unsigned char rgb[rows*cols*components];
	unsigned char ycbcr[rows*cols*components];

	fprintf(stderr, "%lu\n", sizeof(rgb));

	unsigned char *ptrR = rgb;
	unsigned char *ptrG = rgb + 1;
	unsigned char *ptrB = rgb + 2;


	while(fscanf(fptr, "%hhu %hhu %hhu", ptrR, ptrG, ptrB) != EOF)
	{
		ptrR += 3;
		ptrG += 3;
		ptrB += 3;
	}

	toYCbCr(rgb, ycbcr, rows, cols);

	toRGB(ycbcr, rgb, rows, cols);

	fprintf(stderr, "%lu\n", sizeof(rgb));

	for(int i = 0; i < rows*cols*components; ++i)
	{
		printf("%hhu ", rgb[i]);
	}
}




