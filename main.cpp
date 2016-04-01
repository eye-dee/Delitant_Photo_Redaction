#include <iostream>
#include <fstream>

#include "loadjpg.h"

const std :: string inp = "in2.jpg";
const std :: string outp = "out.txt";
const std :: string outp2 = "out2.txt";

int main()
{
	FILE *fp;
    unsigned int lengthOfFile;
    unsigned char *buf;
    

    // Load the Jpeg into memory
	fp = fopen(inp.c_str(), "rb");
    if (fp == NULL)
    {
        //printf("Cannot open jpg file: %s\n", szJpgFileInName);
        return 0;
    }

    lengthOfFile = FileSize(fp);
    buf = new unsigned char[lengthOfFile + 4];
    if (buf == NULL)
    {
        //printf("Not enough memory for loading file\n");
        return 0;
    }
    fread(buf, lengthOfFile, 1, fp);
    fclose(fp);

    unsigned char* rgbpix = NULL;
	color * rgb2 = NULL;
    unsigned int width  = 0;
    unsigned int height = 0;
    
	DecodeJpgFileData(buf, lengthOfFile, &rgbpix, &width, &height);
	WriteBMP24("out1.bmp", width, height, rgbpix);

	rgb2 = new color[width*height];
	std :: ofstream ou(outp);
	for (int i = 0; i < width*height; ++i)
		ou << (int)rgbpix[i] << ' ';
	ou.close();
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			int ind = i*3*width + 3*j;
			int ind2 = i*width + j;
			rgb2[ind2].r = rgbpix[ind];
			rgb2[ind2].g = rgbpix[ind+1];
			rgb2[ind2].b = rgbpix[ind+2];
		}

	for (int i = 0; i < width*height; ++i)

	WriteBMP24("out2.bmp", width, height, rgb2);

	std :: ofstream ou2(outp2);
	for (int i = 0; i < width*height; ++i)
		ou2 << (int)rgb2[i].r << ' ' 
		<< (int)rgb2[i].g << ' '
		<< (int)rgb2[i].b << ' ';
	ou2.close();

}