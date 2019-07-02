#include "stdafx.h"
#include <cmath>

/*
(x, y, z)物体的顶点坐标经过
模型矩阵，平移缩放等，变成世界坐标
视图矩阵，根据摄像机的位置，变成眼睛坐标、相机坐标
投影矩阵，相机的视椎体做投影变换，远近裁剪面，梯形变成-1到1的规范的标准立方体，物体坐标就在立方体内
视口变换，变成窗口坐标
*/

//禁止把单精度变双精度
#pragma warning(disable:4305)

void ShowFloat(float f) {
	TCHAR a[32];
	swprintf(a, 32, _T("%f"), f);

	MessageBox(NULL, a, _T("Test"), MB_OK);
}

CGfxOpenGL::CGfxOpenGL() {
	cameraX = cameraY = cameraZ = 0.0f;
}

CGfxOpenGL::~CGfxOpenGL() {
	
}

void CGfxOpenGL::DrawPlane(float size, float step) {
	
	glBegin(GL_QUADS);

	glNormal3f(0.0f, -1.0f, 0.0f);
	
	for (float z = 0.0; z < size; z += step) {
		for (float x = 0.0; x < size; x += step) {
			glVertex3f(x, 0.0f, z);
			glVertex3f(x + step, 0.0f, z);
			glVertex3f(x + step, 0.0f, z + step);
			glVertex3f(x, 0.0f, z + step);
		}
	}
	
	glEnd();
}

void CGfxOpenGL::DrawCube(float size, int resolution)
{
	float step = size / resolution;
	glPushMatrix();
		glTranslatef(-size / 2, -size / 2, -size / 2);
		//top
		glPushMatrix();
			glTranslatef(0.0f, size, 0.0f);
			glScalef(1.0f, -1.0f, 1.0f);
			DrawPlane(size, step);
		glPopMatrix();

		//left
		glPushMatrix();
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			glScalef(1.0f, -1.0f, 1.0f);
			DrawPlane(size, step);
		glPopMatrix();

		//right
		glPushMatrix();
			glTranslatef(size, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			DrawPlane(size, step);
		glPopMatrix();

		//front
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, size);//3. 再平移
			glRotatef(90.0f, -1.0f, 0.0f, 0.0f);//2. 在翻转
			DrawPlane(size, step);//1. 先画出来
		glPopMatrix();

		//back
		glPushMatrix();
			glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
			glScalef(1.0f, -1.0f, 1.0f);
			DrawPlane(size, step);
		glPopMatrix();

		//bottom
		DrawPlane(size, step);
	glPopMatrix();
}

static GLfloat light1_Pos[4] = { -5.0f, 10.0f, 10.0f, 1.0f };
static GLfloat spot_dir[3] = { -1.0f, -2.5f, -10.0f };
bool CGfxOpenGL::Init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);

	m_tga = new CTargaImage();
	if (!m_tga->Load("test.tga")) {
		return false;
	}

	//static float plane_s[4] = {1,0,0,0};//平面法向量,第四个参数是面的位置
	//static float plane_t[4] = { 0,1,0,0 };
	//glEnable(GL_TEXTURE_GEN_S);//让OpenGL自动生成纹理坐标
	//glEnable(GL_TEXTURE_GEN_T);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);//x坐标生成算法
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);//y坐标生成算法
	//glTexGenfv(GL_S, GL_OBJECT_LINEAR, plane_s);//告诉这个算法，用这个平面计算距离，生成x的纹理坐标
	//glTexGenfv(GL_T, GL_OBJECT_LINEAR, plane_t);

	/*glGenTextures(1, &m_1D);
	glBindTexture(GL_TEXTURE_1D, m_1D);
	glTexImage1D(GL_TEXTURE_1D, 0,
		GL_RGBA, image.GetWidth(),
		0, GL_RGBA, GL_UNSIGNED_BYTE,
		image.GetImage());
	glGenerateMipmap(GL_TEXTURE_1D);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/


	glActiveTexture(GL_TEXTURE0);//使用纹理单元0
	glEnable(GL_TEXTURE_2D);//激活纹理单元。这是2D纹理，还有1D纹理，如果1D 2D同时启用，会优先使用2D
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//该纹理单元里的纹理 与之前的进行颜色叠加GL_MODULATE.还有GL_REPLACE替换之前的纹理单元颜色 GL_ADD GL_BLEND等
	glGenTextures(1, &m_textureObject);//获得1个纹理对象名称。获得多个的话，需要传一个数组进去
	glBindTexture(GL_TEXTURE_2D, m_textureObject);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,/*参数0是，0层，“层级贴图”的0层，近大远小*/
		m_tga->GetWidth(),
		m_tga->GetHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		m_tga->GetImage());
	glGenerateMipmap(GL_TEXTURE_2D);//OpenGL自动生成“层级渐远”图片出来
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//放大过滤器，点采样。图片放大时，像素使用周边最近的。GL_NEAREST对0级贴图有效
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//缩小过滤器，线性采样。图片放大时，像素使用周边的插值。GL_LINEAR对0级贴图有效

	/*gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA,
		m_tga->GetWidth(),
		m_tga->GetHeight(),
		GL_RGBA, GL_UNSIGNED_BYTE,
		m_tga->GetImage());*///OpenGL工具函数，可以直接生成层级贴图

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR_MIPMAP_LINEAR告诉OpenGL，放大过滤器，使用层级贴图
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);//"各向异性"，采样时取周围8个像素进行计算。梯形的远端更清晰。12 16都可以。通过glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, float& param);可以查询各向异性最多采用几个像素，可能是16，根据显卡不同
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//默认是GL_REPEAT，重复
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//水平方向，镜像重复
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//垂直方向，镜像重复
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);//水平方向，大于1的纹理坐标，用纹理边缘的像素铺过去
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);//垂直方向，

	//GLfloat borderColor[] = {1,1,1,1};//写一个边框颜色白色。默认是黑。
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);//设置“边框颜色”，用于接下来的延伸色
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);//大于1的纹理坐标，用设定的边框色填充
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	

	rotateAngle = 0;

	m_zPos = 0;
	m_zMoveNegative = true;

	//glEnable(GL_DEPTH_TEST);//深度测试，近的挡住远的
	//glDepthFunc(GL_LEQUAL);//深度测试小于等于算法。只有后者距离小于等于前者，就显示后者。距离摄像机的距离越小，越优先显示

	//glEnable(GL_NORMALIZE);//法线标准化处理
	////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_LIGHTING);//启用光照。还需要激活灯光
	//
	//GLfloat light_ambient[4] = { 0.3f, 0.3f, 0.3f, 1.0f};//环境光 0.3弱
	//GLfloat light_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//漫反射光
	//GLfloat light_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//镜面反射光

	//glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);//定义灯光的属性
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	//GLfloat lightPos[4] = { 0,100,100,1 };//默认0,0,1的位置。第四个参数为1，为定点光。0平行光
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);//设置衰减公式系数
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01f);//设置衰减公式系数
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);//设置衰减公式系数

	//glEnable(GL_LIGHT0);//激活灯光


	//glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	//
	//glLightfv(GL_LIGHT1, GL_POSITION, light1_Pos);//默认0,0,1的位置。第四个参数为1，为定点光。0平行光。聚光灯是一个特殊的定点光
	//
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);//设置聚光灯方向

	//glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 7.5f);//设置聚光灯角度。圆锥角横截面角度的一半
	//glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 128);//聚光灯强度，中间强，周边弱

	//glEnable(GL_LIGHT1);//把这个做成聚光灯

	return true;
}

bool CGfxOpenGL::Shutdown() {
	glDeleteTextures(1, &m_textureObject);//清理OpenGL里的纹理
	m_tga->Release();
	delete m_tga;
	return true;
}

void CGfxOpenGL::SetupProjection(int width, int height) {


	if (height == 0) {
		height = 1;
	}

	glViewport(0, 0, width, height);//设置OpenGL视口大小。看得见的区域。OpenGL可以很大。矩阵变换中的最后一步，视口变换，视口矩阵，物体本地坐标变成窗口坐标

	glMatrixMode(GL_PROJECTION);//选择投影矩阵
	glLoadIdentity();//投影矩阵设为单位矩阵。（即把相机放回原点，朝向Z轴负方向。这一句解释可能不对）

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);//透视，近大远小。近裁剪面，远裁剪面。投影变换

	glMatrixMode(GL_MODELVIEW);//模型视图矩阵。OpenGL将模型矩阵和视图矩阵合二为一。
	glLoadIdentity();

	m_windowWidth = width;
	m_windowHeight = height;
}

void CGfxOpenGL::ResizeScene(int width, int height) {
	if (height == 0) {
		height = 1;
	}

	glViewport(0, 0, width, height);//X，Y————以像素为单位，指定了视口的左下角（在第一象限内，以（0，0）为原点的）位置。width，height————表示这个视口矩形的宽度和高度，根据窗口的实时变化重绘窗口。
	
	m_windowWidth = width;
	m_windowHeight = height;
	UpdateProjection(false);
}

void CGfxOpenGL::UpdateProjection(bool toggle) {

	static bool useProjective = true;
	if (toggle) {
		useProjective = !useProjective;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (useProjective) {
		//glFrustum(-1.0, 1.0, -1.0, 1.0, 1, 1000.0);//用来设置视椎体。透视
		gluPerspective(60.0f, (GLfloat)m_windowWidth / (GLfloat)m_windowHeight, 1.0f, 1000.0f);//透视，近大远小。近裁剪面，远裁剪面。投影变换
	}
	else
	{
		glOrtho(-1.0, 1.0, -1.0, 1.0, 1, 1000.0);//正交
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CGfxOpenGL::Prepare(float dt) {
	if (m_zMoveNegative) {
		m_zPos -= 2.0f * dt;
	}
	else
	{
		m_zPos += 2.0f * dt;
	}
	if (m_zPos > 0) {
		m_zPos = 0;
		m_zMoveNegative = true;
	}
	if (m_zPos < -10) {
		m_zPos = -10;
		m_zMoveNegative = false;
	}

	cameraY = m_height;
	cameraX = sin(DEG2RAD(m_angle));
	cameraZ = cos(DEG2RAD(m_angle));
}

void CGfxOpenGL::Render() {
	//3D绘图
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空背景。变黑
	glLoadIdentity();

	//gluLookAt(1.0f, 1.5f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt(cameraX, cameraY, cameraZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);//前三个参数是相机位置。相机是看向原点的，中间三个参数

	glColor3f(1, 1, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);//4字节解包对齐。4位一组开始读。告诉OpenGL，每一行能够被4整除的位置，去取数据。OpenGL会忽略补的字节

	//glPixelZoom(1, 1);
	//glWindowPos2i(100, 100);
	//glBitmap(32,32,0,0,0,, bigLetterF);

	//glWindowPos2i(400, 200);
	//glDrawPixels(m_tga->GetWidth(), m_tga->GetHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_tga->GetImage());
	//glDrawPixels(m_tga->GetWidth(), m_tga->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, m_tga->GetImage());


	glBindTexture(GL_TEXTURE_2D, m_textureObject);

	//glMatrixMode(GL_TEXTURE);//选择纹理矩阵
	//glLoadIdentity();
	//glRotatef(10,0,0,1);//纹理矩阵绕z轴旋转10度
	//glMatrixMode(GL_MODELVIEW);//选择模型视图矩阵 

	//glTranslatef(0, 0, m_zPos);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);//该函数指定的是纹理单元0的纹理坐标
		//glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f);//指定纹理单元1的坐标也绑定到这个顶点上
		glVertex3f(-1.0f, -1.0f, -1.0f);

		glTexCoord2f(1.0f, 0.0f);//扭曲的话 OpenGL会进行插值计算？
		glVertex3f( 1.0f, -1.0f, -1.0f);

		glTexCoord2f(3.0f, 3.0f);
		glVertex3f(1.0f,  1.0f, -1.0f);

		glTexCoord2f(0.0f, 3.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
}