#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

int clamp(int n);

//rgb is a rows*cols*3 array of unsigned char (ie [0, 255])
//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toYCbCr(register uint8_t * restrict rgb, uint8_t * restrict ycbcr, uint16_t rows, uint16_t cols)
{
	uint8_t *ycbcrPtr = ycbcr;
	uint8_t r, g, b;
	uint32_t *rgbPtr = (uint32_t *) rgb;
	uint32_t rgb_value;

	for(int i = rows*cols*3 - 1; i > 0; i -= 12)
	{

		rgb_value = *rgbPtr;

		//Grab the 3 8-bit numbers that are stored in a 32-bit number
		r = rgb_value & 0x000000FF;
		g = (rgb_value << 16) >> 24;
		b = (rgb_value << 8) >> 24;
        
		//Calculate the Y, Cb, and Cr (8-bit) values and store them in the ycbcr array
		*ycbcrPtr++ =  16 + ((16483*r + 33030*g + 6423*b) >> 16);
		*ycbcrPtr++ = 128 + ((-9699*r - 19071*g + 28770*b) >> 16);
		*ycbcrPtr++ = 128 + ((28770*r - 24117*g - 4653*b) >> 16);

		//Move the rgb pointer up by 3 bytes to grab the next pixel as a 32-bit value
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);
		rgb_value = *rgbPtr;

		//Grab the 3 8-bit numbers that are stored in a 32-bit number
		r = rgb_value & 0x000000FF;
		g = (rgb_value << 16) >> 24;
		b = (rgb_value << 8) >> 24;
        
		//Calculate the Y (8-bit) value and store it in the ycbcr array
		*ycbcrPtr++ = 16 + ((16483*r + 33030*g + 6423*b) >> 16);

		//Move the rgb pointer up by 3 bytes to grab the next pixel as a 32-bit value
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);
		rgb_value = *rgbPtr;
        
		//Grab the 3 8-bit numbers that are stored in a 32-bit number
		r = rgb_value & 0x000000FF;
		g = (rgb_value << 16) >> 24;
		b = (rgb_value << 8) >> 24;
        
		//Calculate the Y (8-bit) value and store it in the ycbcr array
		*ycbcrPtr++ = 16 + ((16483*r + 33030*g + 6423*b) >> 16);
        
		//Move the rgb pointer up by 3 bytes to grab the next pixel as a 32-bit value
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);
		rgb_value = *rgbPtr;
        
		//Grab the 3 8-bit numbers that are stored in a 32-bit number
		r = rgb_value & 0x000000FF;
		g = (rgb_value << 16) >> 24;
		b = (rgb_value << 8) >> 24;
        
		//Calculate the Y (8-bit) value and store it in the ycbcr array
		*ycbcrPtr++ = 16 + ((16483*r + 33030*g + 6423*b) >> 16);
        
		//Move the rgb pointer up by 3 bytes to grab the next pixel as a 32-bit value
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);
	
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

	int32_t y16_1, y16_2, y16_3, y16_4, cb128, cr128;
	int32_t rP2, gP2, bP2, yP2;

	uint32_t *ycbcrPtr = (uint32_t *) ycbcr;
	uint8_t *rgbPtr = rgb;

	uint32_t ycbcr_value;

	for(int i = ((rows*cols*3) >> 1) - 1; i > 0; i -= 6)
	{
		
		ycbcr_value = *ycbcrPtr;

		//Grab the 3 8-bit numbers that are stored in a 32-bit number
		y16_1 = (ycbcr_value & 0x000000FF) - 16;
		cb128 = ((ycbcr_value << 16) >> 24) - 128;
		cr128 = ((ycbcr_value << 8) >> 24) - 128;		

		//These values are used several times when coverting from
		//YCbCr to RGB
		rP2 = 52298*cr128 >> 15;
		gP2 = (-53281*cr128 - 25625*cb128) >> 16;
		bP2 = 33063*cb128 >> 14;

		//Calculate the r, g, and b (8-bit) values and store it in the rgb array
		yP2 = 38142*y16_1 >> 15;
		*rgbPtr++ = clamp(yP2 + rP2);
		*rgbPtr++ = clamp(yP2 + gP2);
		*rgbPtr++ = clamp(yP2 + bP2);

		//Move the ycbcr pointer up by 3 bytes to grab the next 3 Y (8-bit) values as a 32-bit value
		ycbcrPtr = (uint32_t *) (((uint8_t *) ycbcrPtr) + 3);
		ycbcr_value = *ycbcrPtr;

		//Grab the 3 8-bit numbers that are stored in a 32-bit number
		y16_2 = (ycbcr_value & 0x000000FF) - 16;
		y16_3 = ((ycbcr_value << 16) >> 24) - 16;
		y16_4 = ((ycbcr_value << 8) >> 24) - 16;

		//Move the ycbcr pointer up by 3 bytes to grab the next Y, Cb, and Cr (8-bit) values as a 32-bit value
		ycbcrPtr = (uint32_t *) (((uint8_t *) ycbcrPtr) + 3);

		//Calculate the r, g, and b (8-bit) values and store it in the rgb array
		yP2 = 38142*y16_2 >> 15;
		*rgbPtr++ = clamp(yP2 + rP2);
		*rgbPtr++ = clamp(yP2 + gP2);
		*rgbPtr++ = clamp(yP2 + bP2);
        
		//Calculate the r, g, and b (8-bit) values and store it in the rgb array
		yP2 = 38142*y16_3 >> 15;
		*rgbPtr++ = clamp(yP2 + rP2);
		*rgbPtr++ = clamp(yP2 + gP2);
		*rgbPtr++ = clamp(yP2 + bP2);

		//Calculate the r, g, and b (8-bit) values and store it in the rgb array
		yP2 = 38142*y16_4 >> 15;
		*rgbPtr++ = clamp(yP2 + rP2);
		*rgbPtr++ = clamp(yP2 + gP2);
		*rgbPtr++ = clamp(yP2 + bP2);
	}
}

int main(void)
{
	int rows, cols, components;
	scanf("%d %d %d", &rows, &cols, &components);

	assert(rows % 2 == 0);
	assert(cols % 2 == 0);
	assert(components == 3);

	uint8_t *rgb = malloc(rows*cols*components*sizeof(uint8_t));
	uint8_t *ycbcr = malloc(rows*cols*components*sizeof(uint8_t) >> 1);

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

	memset(rgb, 0, rows*cols*components);

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
