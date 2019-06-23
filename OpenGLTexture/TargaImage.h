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
	unsigned char idLength;//表示字符串长度
	unsigned char colorMapType;//颜色表类型。颜色表已经不用了。现在必须是0
	unsigned char imageTypeCode;//图像类型码。
	unsigned char colorMapSpec[5];//颜色表首地址、颜色表长度、颜色表项位数
	unsigned short xOrigin;//图像X坐标起始位置
	unsigned short yOrigin;//图像y坐标起始位置
	unsigned short width;
	unsigned short height;
	unsigned char bpp;//图像每像素存储占用位数 rgb rgba
	unsigned char imageDec;//图像描述符。表名起始像素位置
};


class CTargaImage
{
private:
	unsigned char   m_colorDepth;//颜色深度。表名是rgb24位还是rgba32位
	unsigned char   m_imageDataType;//数据类型。无符号字节
	unsigned char   m_imageDataFormat;//数据格式 IMAGE_RGB IMAGE_RGBA
	unsigned char*  m_pImageData;//图像的内容指针。图里除了文件头等无用数据外的信息
	unsigned short  m_width;
	unsigned short  m_height;
	unsigned long   m_imageSize;//图的大小 宽x高x（3或4）
	
	//void SwapRedBlue();//因为tga是BGR（A）格式。但OpenGL习惯使用RGB（A），所以做个交换
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

	//bool ConvertRGBAToRGB();//去掉alpha通道
	//bool ConvertRGBToRGBA(const unsigned char alphaValue);

	unsigned char * GetImage() {
		return m_pImageData;
	}

	bool Load(const char* fileName);
	void Release();//释放某些内存。比如图的内容。

	//bool FlipVertical();//图的第一个像素可能是左上角、左下角、右上、右下角，所以要翻转成OpenGL的格式（OpenGL是读入的第一个像素，画在左下角）
};

#endif