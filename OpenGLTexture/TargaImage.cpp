#include "stdafx.h"
#include "TargaImage.h"
#include <iostream>


CTargaImage::CTargaImage()
{
	m_pImageData = NULL;
}


CTargaImage::~CTargaImage()
{
	Release();
}

bool CTargaImage::Load(const char * fileName)
{
	FILE *pFile = fopen(fileName, "rb");
	if (!pFile) {
		return false;
	}

	tgaheader_t tgaHeader;
	fread(&tgaHeader, 1, sizeof(tgaheader_t), pFile);

	if ((tgaHeader.imageTypeCode != TGA_RGB) && (tgaHeader.imageTypeCode != TGA_GRAYSCALE_RLE)
		|| tgaHeader.colorMapType != 0) {
		fclose(pFile);
		return false;
	}

	m_width = tgaHeader.width;
	m_height = tgaHeader.height;

	int colorMode = tgaHeader.bpp / 8;
	if (colorMode == 3) {

	}
	else if (colorMode == 4) {

	}
	else {
		fclose(pFile);
		return false;
	}
	m_imageSize = m_width * m_height * colorMode;

	m_pImageData = new unsigned char[m_imageSize];

	if (tgaHeader.idLength > 0) {
		fseek(pFile, SEEK_CUR, tgaHeader.idLength);
	}

	if (tgaHeader.imageTypeCode == TGA_RGB) {
		fread(m_pImageData, 1, m_imageSize, pFile);
	}
	else
	{
		 
	}
	fclose(pFile);

	switch (tgaHeader.imageTypeCode)
	{
	case TGA_RGB:
		break;
	case TGA_RGB_RLE://—πÀı∏Ò Ω
		if (3 == colorMode) {
			m_imageDataFormat = IMAGE_RGB;
			m_imageDataType = IMAGE_DATA_UNSIGNED_BYTE;
			m_colorDepth = 24;
		}
		else {
			m_imageDataFormat = IMAGE_RGBA;
			m_imageDataType = IMAGE_DATA_UNSIGNED_BYTE;
			m_colorDepth = 32;
		}
		break;
	default:
		break;
	}

	if ((tgaHeader.imageDec & TOP_LEFT) == TOP_LEFT) {
		//FlipVertical();
	}

	//SwapRedBlue();

	return (m_pImageData != NULL);
}

void CTargaImage::Release()
{
	delete[] m_pImageData;
	m_pImageData = NULL;
}
