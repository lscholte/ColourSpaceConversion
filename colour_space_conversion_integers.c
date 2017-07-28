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
	uint8_t *ycbcrPtr = ycbcr;
	int r, g, b;
	uint32_t *ptr = (uint32_t *) rgb;

	for(int i = rows*cols*3 - 1; i > 0; i -= 12)
	{

		r = *ptr & 0x000000FF;
		g = (*ptr << 16) >> 24;
		b = (*ptr << 8) >> 24;
        
		*ycbcrPtr++ =  16 + ((16483*r + 33030*g + 6423*b) >> 16); //should be guaranteed to be in range of [0, 255]
		*ycbcrPtr++ = 128 + ((-9699*r - 19071*g + 28770*b) >> 16); //should be guaranteed to be in range of [0, 255]
		*ycbcrPtr++ = 128 + ((28770*r - 24117*g - 4653*b) >> 16); //should be guaranteed to be in range of [0, 255]

		ptr = (uint32_t *) (rgb + 3);

		r = *ptr & 0x000000FF;
		g = (*ptr << 16) >> 24;
		b = (*ptr << 8) >> 24;
        
		*ycbcrPtr++ = 16 + ((16483*r + 33030*g + 6423*b) >> 16); //should be guaranteed to be in range of [0, 255]

		ptr = (uint32_t *) (rgb + 6);
        
		r = *ptr & 0x000000FF;
		g = (*ptr << 16) >> 24;
		b = (*ptr << 8) >> 24;
        
		*ycbcrPtr++ = 16 + ((16483*r + 33030*g + 6423*b) >> 16); //should be guaranteed to be in range of [0, 255]
        
		ptr = (uint32_t *) (rgb + 9);
        
		r = *ptr & 0x000000FF;
		g = (*ptr << 16) >> 24;
		b = (*ptr << 8) >> 24;
        
		*ycbcrPtr++ = 16 + ((16483*r + 33030*g + 6423*b) >> 16); //should be guaranteed to be in range of [0, 255]
        
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

	int y16_1, y16_2, y16_3, y16_4, cb128, cr128, rP2, gP2, bP2, yP2;

	uint32_t *ptr = (uint32_t *) ycbcr;
	uint32_t ycbcr_value;

	for(int i = ((rows*cols*3) >> 1) - 1; i > 0; i -= 6)
	{
		
		ycbcr_value = *ptr;

		y16_1 = (ycbcr_value & 0x000000FF) - 16;
		cb128 = ((ycbcr_value << 16) >> 24) - 128;
		cr128 = ((ycbcr_value << 8) >> 24) - 128;		

		rP2 = 52298*cr128 >> 15;
		gP2 = (-53281*cr128 - 25625*cb128) >> 16;
		bP2 = 33063*cb128 >> 14;

		yP2 = 38142*y16_1 >> 15;
		*ptr++ = clamp(yP2 + rP2); //this might exceed 255
		*ptr++ = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*ptr++ = clamp(yP2 + bP2); //this might exceed 255

		ptr = (uint32_t *) (ycbcr + 3);

		ycbcr_value = *ptr;

		y16_2 = (ycbcr_value & 0x000000FF) - 16;
		y16_3 = ((ycbcr_value << 16) >> 24) - 16;
		y16_4 = ((ycbcr_value << 8) >> 24) - 16;		

		yP2 = 38142*y16_2 >> 15;
		*ptr++ = clamp(yP2 + rP2); //this might exceed 255
		*ptr++ = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*ptr++ = clamp(yP2 + bP2); //this might exceed 255
        
		yP2 = 38142*y16_3 >> 15;
		*ptr++ = clamp(yP2 + rP2); //this might exceed 255
		*ptr++ = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*ptr++ = clamp(yP2 + bP2); //this might exceed 255

		yP2 = 38142*y16_4 >> 15;
		*ptr++ = clamp(yP2 + rP2); //this might exceed 255
		*ptr++ = clamp(yP2 + gP2); //this could be less than 0 or greater than 255
		*ptr++ = clamp(yP2 + bP2); //this might exceed 255
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