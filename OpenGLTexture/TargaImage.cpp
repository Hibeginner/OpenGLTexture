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

bool CTargaImage::ConvertRGBAToRGB()
{
	if ((m_colorDepth == 32) && (m_imageDataFormat == IMAGE_RGBA)) {
		rgb_t * newImageData = new rgb_t[m_width * m_height];
		if (!newImageData) {
			return false;
		}
		rgb_t * dest = newImageData;
		rgba_t * src = (rgba_t *)m_pImageData;
		for (int x = 0; x < m_height; x++) {
			for (int y = 0; y < m_width; y++) {
				dest->r = src->r;
				dest->g = src->g;
				dest->b = src->b;

				++src;
				++dest;
			}
		}
		delete[] m_pImageData;
		m_pImageData = (unsigned char *)newImageData;
		m_colorDepth = 24;
		m_imageDataFormat = IMAGE_RGB;

		return true;
	}
	return false;
}

bool CTargaImage::ConvertRGBToRGBA(const unsigned char alphaValue)
{
	if ((m_colorDepth == 24) && (m_imageDataFormat == IMAGE_RGB)) {
		rgba_t * newImageData = new rgba_t[m_width * m_height];
		if (!newImageData) {
			return false;
		}
		rgba_t * dest = newImageData;
		rgba_t * src = (rgba_t *)m_pImageData;
		for (int x = 0; x < m_height; x++) {
			for (int y = 0; y < m_width; y++) {
				dest->r = src->r;
				dest->g = src->g;
				dest->b = src->b;
				dest->a = alphaValue;
				
				++src;
				++dest;
			}
		}
		delete[] m_pImageData;
		m_pImageData = (unsigned char *)newImageData;
		m_colorDepth = 32;
		m_imageDataFormat = IMAGE_RGBA;

		return true;
	}
	return false;
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
	else if(tgaHeader.imageTypeCode == TGA_RGB_RLE)
	{
		unsigned char header;
		unsigned char length;
		rgba_t color = {0,0,0,0};
		unsigned int i = 0;
		while (i<m_imageSize)
		{
			header = fgetc(pFile);
			if (header >= 128) {
				length = (unsigned char)(header - 127);

				color.b = (unsigned char)fgetc(pFile);
				color.g = (unsigned char)fgetc(pFile);
				color.r = (unsigned char)fgetc(pFile);
				if (colorMode == 4) {
					color.a = (unsigned char)fgetc(pFile);
				}

				while (length > 0) {

					m_pImageData[i++] = color.b;
					m_pImageData[i++] = color.g;
					m_pImageData[i++] = color.r;
					if (colorMode == 4) {
						m_pImageData[i++] = color.a;
					}

					length--;
				}
			}
			else {
				length = (unsigned char)(header + 1);
				while (length > 0) {
					color.b = (unsigned char)fgetc(pFile);
					color.g = (unsigned char)fgetc(pFile);
					color.r = (unsigned char)fgetc(pFile);
					if (colorMode == 4) {
						color.a = (unsigned char)fgetc(pFile);
					}

					m_pImageData[i++] = color.b;
					m_pImageData[i++] = color.g;
					m_pImageData[i++] = color.r;
					if (colorMode == 4) {
						m_pImageData[i++] = color.a;
					}

					length--;
				}
			}
		}
	}
	fclose(pFile);

	switch (tgaHeader.imageTypeCode)
	{
	case TGA_RGB:
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
		FlipVertical();
	}

	SwapRedBlue();

	return (m_pImageData != NULL);
}

void CTargaImage::Release()
{
	delete[] m_pImageData;
	m_pImageData = NULL;
}

bool CTargaImage::FlipVertical()
{
	if (!m_pImageData) {
		return false;
	}
	if (m_colorDepth == 32) {
		rgba_t * tempBits = new rgba_t[m_width];
		if (!tempBits) {
			return false;
		}
		int lineWidth = m_width * 4;
		rgba_t * top = (rgba_t *)m_pImageData;
		rgba_t * bottom = (rgba_t *)(m_pImageData + lineWidth*(m_height - 1));
		for (int i = 0; i < (m_height / 2); i++) {
			memcpy(tempBits, top, lineWidth);
			memcpy(top, bottom, lineWidth);
			memcpy(bottom, tempBits, lineWidth);

			top = (rgba_t *)((unsigned char*)top + lineWidth);
			bottom = (rgba_t *)((unsigned char *)bottom - lineWidth);
		}
		delete[] tempBits;
		tempBits = 0;
	}
	else if(m_colorDepth == 24)
	{
		rgb_t * tempBits = new rgb_t[m_width];
		if (!tempBits) {
			return false;
		}
		int lineWidth = m_width * 3;
		rgb_t * top = (rgb_t *)m_pImageData;
		rgb_t * bottom = (rgb_t *)(m_pImageData + lineWidth*(m_height - 1));
		for (int i = 0; i < (m_height / 2); i++) {
			memcpy(tempBits, top, lineWidth);
			memcpy(top, bottom, lineWidth);
			memcpy(bottom, tempBits, lineWidth);

			top = (rgb_t *)((unsigned char*)top + lineWidth);
			bottom = (rgb_t *)((unsigned char *)bottom - lineWidth);
		}
		delete[] tempBits;
		tempBits = 0;
	}
	return true;
}

void CTargaImage::SwapRedBlue() {
	unsigned char temp;
	rgba_t *source32;
	rgb_t * source24;
	switch (m_colorDepth)
	{
	case 32:
		source32 = (rgba_t *)m_pImageData;
		for (int pixel = 0; pixel < m_width * m_height; pixel++) {
			temp = source32[pixel].b;
			source32[pixel].b = source32[pixel].r;
			source32[pixel].r = temp;
		}
		break;
	case 24:
		source24 = (rgb_t *)m_pImageData;
		for (int pixel = 0; pixel < m_width * m_height; pixel++) {
			temp = source24[pixel].b;
			source24[pixel].b = source24[pixel].r;
			source24[pixel].r = temp;
		}
		break;
	default:
		break;
	}
}
