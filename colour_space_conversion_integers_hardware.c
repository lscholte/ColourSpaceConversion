#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

//rgb is a rows*cols*3 array of unsigned char (ie [0, 255])
//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toYCbCr(register uint8_t * restrict rgb, uint8_t * restrict ycbcr, uint16_t rows, uint16_t cols)
{
	uint8_t *ycbcrPtr = ycbcr;

	uint32_t result;
	uint32_t *rgbPtr = (uint32_t *) rgb;

	int r1, g1, b1, y1, cb1, cr1;

	uint32_t *ycbcr32Ptr;
	uint16_t *ycbcr16Ptr;

	uint16_t result16;

	for(int i = rows*cols*3 - 1; i > 0; i -= 12)
	{
		//Converts an RGB value stored in the first 24 bits of a 32-bit value into
		//a YCbCr value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*rgbPtr));

		//Storing the YCbCr values all at once
		ycbcr32Ptr = (uint32_t *) ycbcrPtr;
		*ycbcr32Ptr = result;
		ycbcrPtr += 3;

		//Move to the next RGB triplet
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);


		//Converts an RGB value stored in the first 24 bits of a 32-bit value into
		//a YCbCr value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*rgbPtr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about

		//Take first Y value and store it in a 16-bit integer
		result16 = (uint16_t) (result & 0x000000FF);

		//Move to the next RGB triplet
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);

		//Converts an RGB value stored in the first 24 bits of a 32-bit value into
		//a YCbCr value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*rgbPtr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about

		//Take second Y value and store it a 16-bit integer
		result16 = result16 | ((uint16_t) ((result & 0x000000FF) << 8));

		//Storing the 2 Y values all at once
		ycbcr16Ptr = (uint16_t *) ycbcrPtr;
		*ycbcr16Ptr = result16;
		ycbcrPtr += 2;

		//Move to the next RGB triplet
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);

		//Converts an RGB value stored in the first 24 bits of a 32-bit value into
		//a YCbCr value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*rgbPtr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about

		//Storing the last Y value
		*ycbcrPtr++ = result & 0x000000FF;

		//Move to the next RGB triplet
		rgbPtr = (uint32_t *) (((uint8_t *) rgbPtr) + 3);
	}
}

//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255])
//rgb is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toRGB(uint8_t * restrict ycbcr, uint8_t * restrict rgb, uint16_t rows, uint16_t cols)
{
	uint32_t result;
	uint32_t *ycbcrPtr = (uint32_t *) ycbcr;
	uint8_t *rgbPtr = rgb;
	uint32_t ycbcr_value, yyy_value;

	int y16_1, cb128, cr128, rP2, gP2, bP2, yP2, r1, g1, b1;


	uint32_t *rgb32Ptr;
	uint16_t *rgb16Ptr;

	uint16_t result16;

	for(int i = ((rows*cols*3) >> 1) - 1; i > 0; i -= 6)
	{
		
		ycbcr_value = *ycbcrPtr;
 
		//Converts a YCbCr value stored in the first 24 bits of a 32-bit value into
		//a RGB value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value));

		//Storing the RGB values all at once
		rgb32Ptr = (uint32_t *) rgbPtr;
		*rgb32Ptr = result;
		rgbPtr += 3;

		//Move to the next triplet of values, which are 3 y components
		ycbcrPtr = (uint32_t *) (((uint8_t *) ycbcrPtr) + 3);
		yyy_value = *ycbcrPtr;

		//Move to the next triplet of values, which are 3 y, cb, and cr components
		ycbcrPtr = (uint32_t *) (((uint8_t *) ycbcrPtr) + 3);

		//Taking the first y component from the yyy triplet and replace the y value
		//from the ycbcr triplet with it
		ycbcr_value = (yyy_value & 0x000000FF) | (ycbcr_value & 0xFFFFFF00);

		//Converts a YCbCr value stored in the first 24 bits of a 32-bit value into
		//a RGB value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value));

		//Storing the RGB values all at once
		rgb32Ptr = (uint32_t *) rgbPtr;
		*rgb32Ptr = result;
		rgbPtr += 3;

		//Taking the second y component from the yyy triplet and replace the y value
		//from the ycbcr triplet with it
		ycbcr_value = ((yyy_value & 0x0000FF00) >> 8) | (ycbcr_value & 0xFFFFFF00);

 
		//Converts a YCbCr value stored in the first 24 bits of a 32-bit value into
		//a RGB value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value));

		//Take the RG values and store store them in a 16-bit integer
		result16 = (uint16_t) (result & 0x0000FFFF);

		//Storing the RGB values all at once
		rgb32Ptr = (uint32_t *) rgbPtr;
		*rgb32Ptr = result;
		rgbPtr += 3;

		//Taking the third y component from the yyy triplet and replace the y value
		//from the ycbcr triplet with it
		ycbcr_value = ((yyy_value & 0x00FF0000) >> 16) | (ycbcr_value & 0xFFFFFF00);


		//Converts a YCbCr value stored in the first 24 bits of a 32-bit value into
		//a RGB value stored in the first 24 bits of a 32-bit value
		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value));

		//Storing the RG values all at once
		rgb16Ptr = (uint16_t *) rgbPtr;
		*rgb16Ptr = result16;
		rgbPtr += 2;

		//Storing the B value
		*rgbPtr++ = (result << 8) >> 24;
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




