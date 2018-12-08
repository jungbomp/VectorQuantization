#pragma once

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

typedef unsigned char byte;

// Class structure of Image 
// Use to encapsulate an RGB image
class MyImage
{

private:
	int		Width;					// Width of Image
	int		Height;					// Height of Image
	int		depth;					// Depth of Image pixel
	char	ImagePath[_MAX_PATH];	// Image location
	byte*	Data;					// RGB data of the image


public:
	// Constructor
	MyImage();
	// Copy Constructor
	MyImage::MyImage(MyImage *otherImage);
	// Destructor
	~MyImage();

	// operator overload
	MyImage & operator= (const MyImage & otherImage);

	// Reader & Writer functions
	void setWidth(const int w) { Width = w; };
	void setHeight(const int h) { Height = h; };
	void setDepth(const int d) { depth = d; }
	void setImageData(const byte* img) { Data = (byte*)img; };
	void setImagePath(const char *path) { strcpy(ImagePath, path); }
	int getWidth() { return Width; };
	int getHeight() { return Height; };
	int getDepth() { return depth; }
	byte* getImageData() { return Data; };
	char* getImagePath() { return ImagePath; }

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

	// Modifications
	bool	Modify();

};