#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

int clamp(int n);

//rgb is a rows*cols*3 array of unsigned char (ie [0, 255])
//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toYCbCr(register uint8_t * restrict rgb, uint8_t * restrict ycbcr, uint16_t rows, uint16_t cols)
{
	//int r1, g1, b1, y1, cb1, cr1;
	//int r2, g2, b2, y2;
	//int r3, g3, b3, y3;
	//int r4, g4, b4, y4;
	uint8_t *ycbcrPtr = ycbcr;
	//int i = ((rows*cols*3) >> 2) - 3;
	int i = rows*cols*3 - 1;
	// Cast so 4 8-bit integers are loaded at once
	//uint32_t *rgb32 = (uint32_t *) rgb;

	// load the first 4 bytes
	//uint32_t rgb_tmp = rgb32[i];
	//uint32_t rgb = rgb_tmp;

	int r1, g1, b1, y1, cb1, cr1;
	uint32_t result;
	uint32_t *ptr = (uint32_t *) rgb;

	for(; i > 0; i -= 12)
	{

		r1 = *ptr & 0x000000FF;
		g1 = (*ptr << 16) >> 24;
		b1 = (*ptr << 8) >> 24;
		y1  =  16 + ((16483*r1 + 33030*g1 + 6423*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cb1 = 128 + ((-9699*r1 - 19071*g1 + 28770*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cr1 = 128 + ((28770*r1 - 24117*g1 - 4653*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		result = y1 | (cb1 << 8) | (cr1 << 16);

		//__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ycbcrPtr++ = result & 0x000000FF;
		*ycbcrPtr++ = (result << 16) >> 24;
		*ycbcrPtr++ = (result << 8) >> 24;

		ptr = (uint32_t *) (rgb + 3);
		//__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about

		r1 = *ptr & 0x000000FF;
		g1 = (*ptr << 16) >> 24;
		b1 = (*ptr << 8) >> 24;
		y1  =  16 + ((16483*r1 + 33030*g1 + 6423*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cb1 = 128 + ((-9699*r1 - 19071*g1 + 28770*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cr1 = 128 + ((28770*r1 - 24117*g1 - 4653*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		result = y1 | (cb1 << 8) | (cr1 << 16);
		*ycbcrPtr++ = result & 0x000000FF;

		ptr = (uint32_t *) (rgb + 6);
		//__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		r1 = *ptr & 0x000000FF;
		g1 = (*ptr << 16) >> 24;
		b1 = (*ptr << 8) >> 24;
		y1  =  16 + ((16483*r1 + 33030*g1 + 6423*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cb1 = 128 + ((-9699*r1 - 19071*g1 + 28770*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cr1 = 128 + ((28770*r1 - 24117*g1 - 4653*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		result = y1 | (cb1 << 8) | (cr1 << 16);
		*ycbcrPtr++ = result & 0x000000FF;


		ptr = (uint32_t *) (rgb + 9);
		//__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		r1 = *ptr & 0x000000FF;
		g1 = (*ptr << 16) >> 24;
		b1 = (*ptr << 8) >> 24;
		y1  =  16 + ((16483*r1 + 33030*g1 + 6423*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cb1 = 128 + ((-9699*r1 - 19071*g1 + 28770*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		cr1 = 128 + ((28770*r1 - 24117*g1 - 4653*b1) >> 16); //should be guaranteed to be in range of [0, 255]
		result = y1 | (cb1 << 8) | (cr1 << 16);
		*ycbcrPtr++ = result & 0x000000FF;

		ptr = (uint32_t *) (rgb + 12);
	
	}
}


int clamp(int n)
{
    if(n < 0)
    {
        return 0;
    }
    else if(n > 255)
    {
        return 255;
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
	uint8_t *rgbPtr = rgb + 11;
	int i = ((rows*cols*3) >> 3) - 1;
	// Cast so 4 8-bit integers are loaded at once
	uint32_t *ycbcr32 = (uint32_t *) ycbcr;

	uint32_t ycbcr_tmp = ycbcr32[i];

	for(; i > 0; i -= 3)
	{
		y16_4 = (ycbcr_tmp >> 24) - 16;
		y16_3 = ((ycbcr_tmp << 8) >> 24) - 16;
		y16_2 = ((ycbcr_tmp << 16) >> 24) - 16;
		cr128 = (ycbcr_tmp & 0x000000FF) - 128;

		ycbcr_tmp = ycbcr32[i-1];
		cb128 = (ycbcr_tmp >> 24) - 128;
		y16_1 = ((ycbcr_tmp << 8) >> 24) - 16;

		rP2 = 52298*cr128 >> 15;
		gP2 = (-53281*cr128 - 25625*cb128) >> 16;
		bP2 = 33063*cb128 >> 14;

		yP2 = 38142*y16_4 >> 15;
		b4 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b4;
		g4 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g4;
		r4 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = r4;

		yP2 = 38142*y16_3 >> 15;
		b3 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b3;
		g3 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g3;
		r3 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = r3;

		y16_4 = ((ycbcr_tmp << 16) >> 24) - 16;
		y16_3 = (ycbcr_tmp & 0x000000FF) - 16;
		ycbcr_tmp = ycbcr32[i-2];

		yP2 = 38142*y16_2 >> 15;
		b2 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b2;
		g2 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g2;
		r2 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = r2;

		yP2 = 38142*y16_1 >> 15;
		b1 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b1;
		g1 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g1;
		r1 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr = r1;

		rgbPtr += 23;

		y16_2 = (ycbcr_tmp >> 24) - 16;
		cr128 = ((ycbcr_tmp << 8) >> 24) - 128;	
		cb128 = ((ycbcr_tmp << 16) >> 24)- 128;
		y16_1 = (ycbcr_tmp & 0x000000FF) - 16;
		ycbcr_tmp = ycbcr32[i-3];

		rP2 = 52298*cr128 >> 15;
		gP2 = (-53281*cr128 - 25625*cb128) >> 16;
		bP2 = 33063*cb128 >> 14;

		yP2 = 38142*y16_4 >> 15;
		b4 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b4;
		g4 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g4;
		r4 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = r4;

		yP2 = 38142*y16_3 >> 15;
		b3 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b3;
		g3 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g3;
		r3 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = r3;

		yP2 = 38142*y16_2 >> 15;
		b2 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b2;
		g2 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g2;
		r2 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = r2;

		yP2 = 38142*y16_1 >> 15;
		b1 = clamp(yP2 + bP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = b1;
		g1 = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*rgbPtr-- = g1;
		r1 = clamp(yP2 + rP2); //this could be less than 0 or greater than 255
		*rgbPtr = r1;

		rgbPtr += 23;
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

	fprintf(stderr, "Converting to RGB...\n");
	toRGB(ycbcr, rgb, rows, cols);

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




