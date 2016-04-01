#ifndef LOADJPG_H
#define LOADJPG_H

/***************************************************************************/
/*                                                                         */
/*  File: loadjpg.cpp                                                      */
/*  Author: bkenwright@xbdev.net                                           */
/*  Date: 19-01-06                                                         */
/*                                                                         */
/*  Revised: 26-07-07                                                      */
/*                                                                         */
/***************************************************************************/
/*
About:
Simplified jpg/jpeg decoder image loader - so we can take a .jpg file
either from memory or file, and convert it either to a .bmp or directly
to its rgb pixel data information.

Simplified, and only deals with basic jpgs, but it covers all the
information of how the jpg format works :)

Can be used to convert a jpg in memory to rgb pixels in memory.

Or you can pass it a jpg file name and an output bmp filename, and it
loads and writes out a bmp file.

i.e.
ConvertJpgFile("cross.jpg", "cross.bmp")
*/
/***************************************************************************/

#pragma once

#include <stdio.h>        // sprintf(..), fopen(..)
#include <stdarg.h>     // So we can use ... (in dprintf)
#include <string.h>        // memset(..)
#include <math.h>        // sqrt(..), cos(..)


//extern void dprintf(const char *fmt, ...);
/*
__force void dprintf(const char *fmt, ...) 
{
va_list parms;
char buf[256];

// Try to print in the allocated space.
va_start(parms, fmt);
vsprintf (buf, fmt, parms);
va_end(parms);

// Write the information out to a txt file
FILE *fp = fopen("output.txt", "a+");
fprintf(fp, "%s",  buf);
fclose(fp);

}// End dprintf(..)
*/
/***************************************************************************/

#define DQT      0xDB    // Define Quantization Table
#define SOF      0xC0    // Start of Frame (size information)
#define DHT      0xC4    // Huffman Table
#define SOI      0xD8    // Start of Image
#define SOS      0xDA    // Start of Scan
#define EOI      0xD9    // End of Image, or End of File
#define APP0     0xE0

#define BYTE_TO_WORD(x) (((x)[0]<<8)|(x)[1])


#define HUFFMAN_TABLES        4
#define COMPONENTS            4

#define cY    1
#define cCb    2
#define cCr    3

static int ZigZagArray[64] = 
{
	0,   1,   5,  6,   14,  15,  27,  28,
	2,   4,   7,  13,  16,  26,  29,  42,
	3,   8,  12,  17,  25,  30,  41,  43,
	9,   11, 18,  24,  31,  40,  44,  53,
	10,  19, 23,  32,  39,  45,  52,  54,
	20,  22, 33,  38,  46,  51,  55,  60,
	21,  34, 37,  47,  50,  56,  59,  61,
	35,  36, 48,  49,  57,  58,  62,  63,
};

/***************************************************************************/


struct stBlock
{
	int value;                    // Decodes to.
	int length;                // Length in bits.
	unsigned short int code;    // 2 byte code (variable length)
};

/***************************************************************************/


struct stHuffmanTable
{
	unsigned char    m_length[17];        // 17 values from jpg file, 
	// k =1-16 ; L[k] indicates the number of Huffman codes of length k
	unsigned char    m_hufVal[257];        // 256 codes read in from the jpeg file

	int                m_numBlocks;
	stBlock            m_blocks[1024];
};


struct stComponent 
{
	unsigned int            m_hFactor;
	unsigned int            m_vFactor;
	float *                m_qTable;            // Pointer to the quantisation table to use
	stHuffmanTable*        m_acTable;
	stHuffmanTable*        m_dcTable;
	short int                m_DCT[65];            // DCT coef
	int                    m_previousDC;
};

struct color
{
	unsigned char r,g,b;
};

struct stJpegData
{
	unsigned char*      m_rgb;                // Final Red Green Blue pixel data
	unsigned int        m_width;            // Width of image
	unsigned int        m_height;            // Height of image

	const unsigned char*m_stream;            // Pointer to the current stream

	stComponent            m_component_info[COMPONENTS];

	float                m_Q_tables[COMPONENTS][64];    // quantization tables
	stHuffmanTable        m_HTDC[HUFFMAN_TABLES];        // DC huffman tables  
	stHuffmanTable        m_HTAC[HUFFMAN_TABLES];        // AC huffman tables

	// Temp space used after the IDCT to store each components
	unsigned char        m_Y[64*4];
	unsigned char        m_Cr[64];
	unsigned char        m_Cb[64];

	// Internal Pointer use for colorspace conversion, do not modify it !!!
	unsigned char *        m_colourspace;
};

/***************************************************************************/
// 
//  Returns the size of the file in bytes
//
/***************************************************************************/
int FileSize(FILE *fp);

/***************************************************************************/

// Clamp our integer between 0 and 255
unsigned char Clamp(int i);

/***************************************************************************/

void GenHuffCodes( int num_codes, stBlock* arr, unsigned char* huffVal );

/***************************************************************************/

float C(int u);

int func(int x, int y, const int block[8][8]);

void PerformIDCT(int outBlock[8][8], const int inBlock[8][8]);

/***************************************************************************/

void DequantizeBlock( int block[64], const float quantBlock[64] );

/***************************************************************************/

void DeZigZag(int outBlock[64], const int inBlock[64]);

/***************************************************************************/

void TransformArray(int outArray[8][8], const int inArray[64]);

/***************************************************************************/

void DumpDecodedBlock(int val[8][8]);
/***************************************************************************/

void DecodeSingleBlock(stComponent *comp, unsigned char *outputBuf, int stride);
/***************************************************************************/

/***************************************************************************/
//
// Save a buffer in 24bits Bitmap (.bmp) format 
//
/***************************************************************************/
void WriteBMP24(const char* szBmpFileName, int Width, int Height, unsigned char* RGB);
void WriteBMP24(const char* szBmpFileName, int Width, int Height, color* RGB);
/***************************************************************************/

// Takes two array of bits, and build the huffman table for size, and code

/***************************************************************************/
void BuildHuffmanTable(const unsigned char *bits, const unsigned char *stream, stHuffmanTable *HT);
/***************************************************************************/

void PrintSOF(const unsigned char *stream);

/***************************************************************************/

int ParseSOF(stJpegData *jdata, const unsigned char *stream);

/***************************************************************************/

void BuildQuantizationTable(float *qtable, const unsigned char *ref_table);

/***************************************************************************/

int ParseDQT(stJpegData *jdata, const unsigned char *stream);

/***************************************************************************/

int ParseSOS(stJpegData *jdata, const unsigned char *stream);

/***************************************************************************/

int ParseDHT(stJpegData *jdata, const unsigned char *stream);

/***************************************************************************/

int ParseJFIF(stJpegData *jdata, const unsigned char *stream);

/***************************************************************************/

int JpegParseHeader(stJpegData *jdata, const unsigned char *buf, unsigned int size);
/***************************************************************************/

void JpegGetImageSize(stJpegData *jdata, unsigned int *width, unsigned int *height);

/***************************************************************************/

void FillNBits(const unsigned char** stream, int& nbits_wanted);

short GetNBits(const unsigned char** stream, int nbits_wanted);

int LookNBits(const unsigned char** stream, int nbits_wanted);

void SkipNBits(const unsigned char** stream, int& nbits_wanted);

/***************************************************************************/

bool IsInHuffmanCodes(int code, int numCodeBits, int numBlocks, stBlock* blocks, int* outValue);

/***************************************************************************/

int DetermineSign(int val, int nBits);

/***************************************************************************/
char* IntToBinary(int val, int bits);

/***************************************************************************/

void DumpHufCodes(stHuffmanTable* table);
/***************************************************************************/

void DumpDCTValues(short dct[64]);

/***************************************************************************/

void ProcessHuffmanDataUnit(stJpegData *jdata, int indx);

/***************************************************************************/

void ConvertYCrCbtoRGB(int y, int cb, int cr,
	int* r, int* g, int* b);

/***************************************************************************/

void YCrCB_to_RGB24_Block8x8(stJpegData *jdata, int w, int h, int imgx, int imgy, int imgw, int imgh);

/***************************************************************************/
//
//  Decoding
//  .-------.
//  | 1 | 2 |
//  |---+---|
//  | 3 | 4 |
//  `-------'
//
/***************************************************************************/
void DecodeMCU(stJpegData *jdata, int w, int h);

/***************************************************************************/

int JpegDecode(stJpegData *jdata);

/***************************************************************************/
//
// Take Jpg data, i.e. jpg file read into memory, and decompress it to an
// array of rgb pixel values.
//
// Note - Memory is allocated for this function, so delete it when finished
//
/***************************************************************************/
int DecodeJpgFileData(const unsigned char* buf,  // Jpg file in memory
	const int sizeBuf,         // Size jpg in bytes in memory
	unsigned char** rgbpix,    // Output rgb pixels
	unsigned int* width,       // Output image width
	unsigned int* height);     // Output image heigh

/***************************************************************************/
//
// Load one jpeg image, and decompress it, and save the result.
//
/***************************************************************************/
int ConvertJpgFile(char* szJpgFileInName, char * szBmpFileOutName);

#endif