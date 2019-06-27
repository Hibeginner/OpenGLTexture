#pragma once
#ifndef __GL_COMPONENT
#define __GL_COMPONENT
#define PI 3.14159
#define TWO_PI PI*2.0
#define HALF_PI PI/2.0

class Robot;//前置声明
class CTargaImage;

class CGfxOpenGL {
private:
	Robot * theRobot;
	int m_windowWidth;
	int m_windowHeight;
	float rotateAngle;

	CTargaImage * m_tga;
	unsigned int m_textureObject;//OpenGL的纹理对象

public:
	CGfxOpenGL();
	virtual ~CGfxOpenGL();

	bool Init();
	bool Shutdown();

	void SetupProjection(int width, int height);

	void ResizeScene(int width, int height);//重置视口
	void UpdateProjection(bool toggle = FALSE);//正交投影 或 透视投影

	void DrawPlane(float size, float step);
	void DrawCube(float size, int resolution);

	void Prepare(float dt);
	void Render();
};

#endif // __GL_COMPONENT
