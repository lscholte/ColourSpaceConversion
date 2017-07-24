#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "stb_image_write.h"

void zigzagDataToNormal(uint8_t * rgbZigzag, uint8_t * rgbNormal, int rows, int cols)
{
    int len = rows*cols*3;
    int rowLen = cols*3;
    int j = 0;
    while(j < len)
    {
        int initialJ = j;
        for(int k=0, i=initialJ; k < rowLen; i+=12, j+=6, k+=6)
        {
            rgbNormal[j] = rgbZigzag[i];
            rgbNormal[j+1] = rgbZigzag[i+1];
            rgbNormal[j+2] = rgbZigzag[i+2];

            rgbNormal[j+3] = rgbZigzag[i+3];
            rgbNormal[j+4] = rgbZigzag[i+4];
            rgbNormal[j+5] = rgbZigzag[i+5];
        }
        for(int k=0, i=initialJ+6; k < rowLen; i+=12, j+=6, k+=6)
        {
            rgbNormal[j] = rgbZigzag[i];
            rgbNormal[j+1] = rgbZigzag[i+1];
            rgbNormal[j+2] = rgbZigzag[i+2];

            rgbNormal[j+3] = rgbZigzag[i+3];
            rgbNormal[j+4] = rgbZigzag[i+4];
            rgbNormal[j+5] = rgbZigzag[i+5];
        }
    }
}

int main(int argc, char** argv)
{
    if(argc <= 1) {
        return 1;
    }
    
	int rows, cols, components;
	scanf("%d %d %d", &rows, &cols, &components);

	assert(rows % 2 == 0);
	assert(cols % 2 == 0);
	assert(components == 3);

	uint8_t *rgbZigzag = malloc(rows*cols*components*sizeof(unsigned char));
    uint8_t *rgbNormal = malloc(rows*cols*components*sizeof(unsigned char));

	uint8_t *ptrR = rgbZigzag;
	uint8_t *ptrG = rgbZigzag + 1;
	uint8_t *ptrB = rgbZigzag + 2;

	while(scanf("%hhu %hhu %hhu", ptrR, ptrG, ptrB) != EOF)
	{
		ptrR += 3;
		ptrG += 3;
		ptrB += 3;
	}   
    
    zigzagDataToNormal(rgbZigzag, rgbNormal, rows, cols);
    
    stbi_write_png(argv[1], cols, rows, 3, rgbNormal, cols * 3);

	free(rgbZigzag);
    free(rgbNormal);
}
