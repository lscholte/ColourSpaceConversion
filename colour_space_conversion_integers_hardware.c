#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

//rgb is a rows*cols*3 array of unsigned char (ie [0, 255])
//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toYCbCr(register uint8_t * restrict rgb, uint8_t * restrict ycbcr, uint16_t rows, uint16_t cols)
{
	uint8_t *ycbcrPtr = ycbcr;

	uint32_t result;
	uint32_t *ptr = (uint32_t *) rgb;

	for(int i = rows*cols*3 - 1; i > 0; i -= 12)
	{
		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ycbcrPtr++ = result & 0x000000FF;
		*ycbcrPtr++ = (result << 16) >> 24;
		*ycbcrPtr++ = (result << 8) >> 24;

		ptr = (uint32_t *) (rgb + 3);

		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ycbcrPtr++ = result & 0x000000FF;

		ptr = (uint32_t *) (rgb + 6);

		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ycbcrPtr++ = result & 0x000000FF;


		ptr = (uint32_t *) (rgb + 9);

		__asm__ __volatile__("RGB2YCBCR %0 %1" : "=r" (result) : "r" (*ptr)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ycbcrPtr++ = result & 0x000000FF;

		ptr = (uint32_t *) (rgb + 12);
	}
}

//ycbcr is a rows*cols*3 array of unsigned char (ie [0, 255])
//rgb is a rows*cols*3 array of unsigned char (ie [0, 255]). Its contents are modified by this function
//rows is the number of rows in the image
//cols is the number of columns in the image
void toRGB(uint8_t * restrict ycbcr, uint8_t * restrict rgb, uint16_t rows, uint16_t cols)
{
	uint32_t result;
	uint32_t *ptr = (uint32_t *) ycbcr;
	uint32_t ycbcr_value;

	for(int i = ((rows*cols*3) >> 1) - 1; i > 0; i -= 6)
	{
		
		ycbcr_value = *ptr;

		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ptr++ = result & 0x000000FF;
		*ptr++ = (result << 16) >> 24;
		*ptr++ = (result << 8) >> 24;

		ptr = (uint32_t *) (ycbcr + 3);

		ycbcr_value = (*ptr & 0x000000FF) | (ycbcr_value & 0xFFFFFF00);

		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ptr++ = result & 0x000000FF;
		*ptr++ = (result << 16) >> 24;
		*ptr++ = (result << 8) >> 24;

		ycbcr_value = (*ptr & 0x0000FF00) | (ycbcr_value & 0xFFFFFF00);

		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ptr++ = result & 0x000000FF;
		*ptr++ = (result << 16) >> 24;
		*ptr++ = (result << 8) >> 24;

		ycbcr_value = (*ptr & 0x00FF0000) | (ycbcr_value & 0xFFFFFF00);

		__asm__ __volatile__("YCBCR2RGB %0 %1" : "=r" (result) : "r" (ycbcr_value)); //rgb_tmp has first 3 bytes as rgb and last byte we don't care about
		*ptr++ = result & 0x000000FF;
		*ptr++ = (result << 16) >> 24;
		*ptr++ = (result << 8) >> 24;
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




