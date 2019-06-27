#include "stdafx.h"

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

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &m_textureObject);//获得纹理对象名称
	glBindTexture(GL_TEXTURE_2D, m_textureObject);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		m_tga->GetWidth(),
		m_tga->GetHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		m_tga->GetImage());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//放大过滤器，点采样。图片放大时，像素使用周边最近的
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//缩小过滤器，线性采样。图片放大时，像素使用周边的插值

	rotateAngle = 0;

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
}

void CGfxOpenGL::Render() {
	//3D绘图
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空背景。变黑
	glLoadIdentity();

	//gluLookAt(1.0f, 1.5f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt(0.0f, 0.0f, 8.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glColor3f(1, 1, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);//4字节解包对齐。4位一组开始读。告诉OpenGL，每一行能够被4整除的位置，去取数据。OpenGL会忽略补的字节

	glPixelZoom(1, 1);
	glWindowPos2i(100, 100);
	//glBitmap(32,32,0,0,0,, bigLetterF);

	glWindowPos2i(400, 200);
	//glDrawPixels(m_tga->GetWidth(), m_tga->GetHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, m_tga->GetImage());
	glDrawPixels(m_tga->GetWidth(), m_tga->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, m_tga->GetImage());


	glBindTexture(GL_TEXTURE_2D, m_textureObject);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( 1.0f, -1.0f, 1.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f,  1.0f, 1.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();
}