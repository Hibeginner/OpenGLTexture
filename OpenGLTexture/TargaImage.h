#ifndef _TARGA_LIB
#define _TARGA_LIB

enum TGATypes
{
	TGA_NODATA = 0,
	TGA_INDEXED = 1,
	TGA_RGB = 2,
	TGA_GRAYSCALE = 3,
	
	TGA_INDEXED_RLE = 9,
	TGA_RGB_RLE = 10,
	TGA_GRAYSCALE_RLE = 11,
};

#define IMAGE_RGB   0
#define IMAGE_RGBA  1

#define BOTTOM_LEFT  0x00
#define BOTTOM_RIGHT 0x10
#define TOP_LEFT     0x20
#define TOP_RIGHT    0x30

#define IMAGE_DATA_UNSIGNED_BYTE  0

struct tgaheader_t {
	unsigned char idLength;//��ʾ�ַ�������
	unsigned char colorMapType;//��ɫ�����͡���ɫ���Ѿ������ˡ����ڱ�����0
	unsigned char imageTypeCode;//ͼ�������롣
	unsigned char colorMapSpec[5];//��ɫ���׵�ַ����ɫ���ȡ���ɫ����λ��
	unsigned short xOrigin;//ͼ��X������ʼλ��
	unsigned short yOrigin;//ͼ��y������ʼλ��
	unsigned short width;
	unsigned short height;
	unsigned char bpp;//ͼ��ÿ���ش洢ռ��λ�� rgb rgba
	unsigned char imageDec;//ͼ����������������ʼ����λ��
};


class CTargaImage
{
private:
	unsigned char   m_colorDepth;//��ɫ��ȡ�������rgb24λ����rgba32λ
	unsigned char   m_imageDataType;//�������͡��޷����ֽ�
	unsigned char   m_imageDataFormat;//���ݸ�ʽ IMAGE_RGB IMAGE_RGBA
	unsigned char*  m_pImageData;//ͼ�������ָ�롣ͼ������ļ�ͷ���������������Ϣ
	unsigned short  m_width;
	unsigned short  m_height;
	unsigned long   m_imageSize;//ͼ�Ĵ�С ��x��x��3��4��
	
	//void SwapRedBlue();//��Ϊtga��BGR��A����ʽ����OpenGLϰ��ʹ��RGB��A����������������
public:
	CTargaImage();
	virtual ~CTargaImage();

	unsigned short GetWidth() {
		return m_width;
	}
	unsigned short GetHeight() {
		return m_height;
	}
	unsigned char GetImageFormat() {
		return m_imageDataFormat;
	}

	//bool ConvertRGBAToRGB();//ȥ��alphaͨ��
	//bool ConvertRGBToRGBA(const unsigned char alphaValue);

	unsigned char * GetImage() {
		return m_pImageData;
	}

	bool Load(const char* fileName);
	void Release();//�ͷ�ĳЩ�ڴ档����ͼ�����ݡ�

	//bool FlipVertical();//ͼ�ĵ�һ�����ؿ��������Ͻǡ����½ǡ����ϡ����½ǣ�����Ҫ��ת��OpenGL�ĸ�ʽ��OpenGL�Ƕ���ĵ�һ�����أ��������½ǣ�
};

#endif