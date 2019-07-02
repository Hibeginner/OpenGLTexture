#pragma once
#ifndef __GL_COMPONENT
#define __GL_COMPONENT
#define PI 3.14159
#define TWO_PI PI*2.0
#define HALF_PI PI/2.0
#define DEG2RAD(x) (x*PI)/180.f

class Robot;//ǰ������
class CTargaImage;

class CGfxOpenGL {
private:
	Robot * theRobot;
	int m_windowWidth;
	int m_windowHeight;
	float rotateAngle;
	float m_zPos;
	bool m_zMoveNegative;

	CTargaImage * m_tga;
	unsigned int m_textureObject;//OpenGL���������

	float m_height;
	float m_angle;
	float cameraX;
	float cameraY;
	float cameraZ;

public:
	CGfxOpenGL();
	virtual ~CGfxOpenGL();

	bool Init();
	bool Shutdown();

	void SetupProjection(int width, int height);

	void ResizeScene(int width, int height);//�����ӿ�
	void UpdateProjection(bool toggle = FALSE);//����ͶӰ �� ͸��ͶӰ

	void DrawPlane(float size, float step);
	void DrawCube(float size, int resolution);

	void Prepare(float dt);
	void Render();

	void MoveCameraRight() { m_angle += 1.0f; }
	void MoveCameraLeft() { m_angle -= 1.0f; }
	void MoveCameraUp() { m_height += .5f; }
	void MoveCameraDown() { m_height -= .5f; }
};

#endif // __GL_COMPONENT
