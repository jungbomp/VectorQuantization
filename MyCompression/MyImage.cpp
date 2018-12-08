//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "stdafx.h"
#include "MyImage.h"

#define _CRT_SECURE_NO_WARNINGS


// Constructor and Desctructors
MyImage::MyImage()
{
	Data = NULL;
	Width = -1;
	Height = -1;
	depth = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if (Data)
		delete Data;
}

// Copy constructor
MyImage::MyImage(MyImage *otherImage) {
	Height = otherImage->Height;
	Width = otherImage->Width;
	depth = otherImage->depth;
	Data = new byte[Width*Height*(depth / 8)];
	strcpy(otherImage->ImagePath, ImagePath);

	for (int i = 0; i < (Height*Width*(depth / 8)); i++) {
		Data[i] = otherImage->Data[i];
	}
}

// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage) {
	Height = otherImage.Height;
	Width = otherImage.Width;
	depth = otherImage.depth;
	Data = new byte[Width*Height*(depth / 8)];
	strcpy((char*)otherImage.ImagePath, ImagePath);

	for (int i = 0; i < (Height*Width*(depth / 8)); i++) {
		Data[i] = otherImage.Data[i];
	}

	return *this;
}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}

	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if (IN_FILE == NULL)
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	if (depth == 8) {
		Data = new byte[Height*Width];

		for (i = 0; i < Width*Height; i++) {
			Data[i] = fgetc(IN_FILE);
			if (Data[i] < 0) {
				int a = 1;
			}
		}
	}
	else {
		byte *Rbuf = new byte[Height*Width];
		byte *Gbuf = new byte[Height*Width];
		byte *Bbuf = new byte[Height*Width];

		for (i = 0; i < Width*Height; i++) {
			Rbuf[i] = fgetc(IN_FILE);
		}

		for (i = 0; i < Width*Height; i++) {
			Gbuf[i] = fgetc(IN_FILE);
		}

		for (i = 0; i < Width*Height; i++) {
			Bbuf[i] = fgetc(IN_FILE);
		}

		// Allocate Data structure and copy
		Data = new byte[Width*Height * 3];
		for (i = 0; i < Height*Width; i++) {
			Data[3 * i] = Bbuf[i];
			Data[3 * i + 1] = Gbuf[i];
			Data[3 * i + 2] = Rbuf[i];
		}

		// Clean up and return
		delete Rbuf;
		delete Gbuf;
		delete Bbuf;
	}

	fclose(IN_FILE);

	return true;
}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}

	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if (OUT_FILE == NULL)
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	byte *Rbuf = new byte[Height*Width];
	byte *Gbuf = new byte[Height*Width];
	byte *Bbuf = new byte[Height*Width];

	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3 * i];
		Gbuf[i] = Data[3 * i + 1];
		Rbuf[i] = Data[3 * i + 2];
	}


	// Write data to file
	for (i = 0; i < Width*Height; i++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}


// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
bool MyImage::Modify()
{
	// TO DO by student




	// sample operation
	/*for ( int i=0; i<Width*Height; i++ )
	{
	Data[3*i] = 0;
	Data[3*i+1] = 0;

	}*/

	return false;
}

